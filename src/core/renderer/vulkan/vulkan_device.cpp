#include <core/application/logger.hpp>
#include "vulkan_queue.hpp"
#include "vulkan_rendering_device.hpp"
#include "vulkan_device.hpp"
#include "vk_mem_alloc.h"

namespace Core::Graphics {
  VulkanDevice::VulkanDevice(const VulkanRenderingDevice &renderingDevice): renderingDevice(renderingDevice) {
  }

  VulkanDevice::~VulkanDevice() {
    if (logicalDevice != VK_NULL_HANDLE) {
      vkDeviceWaitIdle(logicalDevice);
      for (auto &graphicsQueue: graphicsQueues)
        graphicsQueue.destroy();
      for (auto &computeQueue: computeQueues)
        computeQueue.destroy();
      for (auto &transferQueue: transferQueues)
        transferQueue.destroy();

      vmaDestroyAllocator(memoryAllocator);
      vkDestroyDevice(logicalDevice, nullptr);
      logicalDevice = VK_NULL_HANDLE;
      physicalDevice = VK_NULL_HANDLE;
    }

    if (debugMessenger)
      vkDestroyDebugUtilsMessengerEXT(renderingDevice.getInstance(), debugMessenger, nullptr);
  }

  void VulkanDevice::createPhysicalDevice(VkPhysicalDevice physicalDevice) {
    this->physicalDevice = physicalDevice;
    vkGetPhysicalDeviceMemoryProperties(this->physicalDevice, &deviceMemoryProperties);
    vkGetPhysicalDeviceProperties(this->physicalDevice, &deviceProperties);
    vkGetPhysicalDeviceFeatures(this->physicalDevice, &deviceFeatures);

    if (!deviceFeatures.geometryShader)
      supportedStages ^= VK_PIPELINE_STAGE_GEOMETRY_SHADER_BIT;

    if (!deviceFeatures.tessellationShader)
      supportedStages ^= VK_PIPELINE_STAGE_TESSELLATION_CONTROL_SHADER_BIT |
          VK_PIPELINE_STAGE_TESSELLATION_EVALUATION_SHADER_BIT;

    std::uint32_t queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, nullptr);
    queueFamilyProperties.resize(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, queueFamilyProperties.data());
  }

  void VulkanDevice::createLogicalDevice(VkDebugUtilsMessengerEXT debugMessenger) {
    this->debugMessenger = debugMessenger;

    const std::uint32_t queueFamilyCount = queueFamilyProperties.size();
    std::vector<VkDeviceQueueCreateInfo> queuesCreateInfo;

    // Track the number of queues for each family. The index is the same as the queue family index.
    std::vector<std::uint32_t> usedQueuesCount(queueFamilyCount, 0);

    findGraphicsQueue(usedQueuesCount);
    findComputeQueue(usedQueuesCount);
    findTransferQueues(usedQueuesCount);

    for (size_t queueIndex = 0; queueIndex < queueFamilyCount; queueIndex++) {
      const VkDeviceQueueCreateInfo deviceQueueCreateInfo {
        .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
        .queueFamilyIndex = static_cast<std::uint32_t>(queueIndex),
        .queueCount = usedQueuesCount[queueIndex]
      };

      if (deviceQueueCreateInfo.queueCount > 0)
        queuesCreateInfo.push_back(deviceQueueCreateInfo);
    }

    std::vector<std::vector<float>> queuePriorities;
    queuePriorities.resize(queuesCreateInfo.size());
    for (size_t infoIndex = 0; infoIndex < queuesCreateInfo.size(); infoIndex++) {
      queuePriorities[infoIndex].resize(queuesCreateInfo[infoIndex].queueCount, 1.0f);
      queuesCreateInfo[infoIndex].pQueuePriorities = queuePriorities[infoIndex].data();
    }

    const std::vector<const char*> deviceExtensions = getExtensions();
    const VkDeviceCreateInfo deviceCreateInfo{
      .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
      .queueCreateInfoCount = static_cast<std::uint32_t>(queuesCreateInfo.size()),
      .pQueueCreateInfos = queuesCreateInfo.data(),
      .enabledExtensionCount = static_cast<std::uint32_t>(deviceExtensions.size()),
      .ppEnabledExtensionNames = deviceExtensions.data(),
      .pEnabledFeatures = &deviceFeatures
    };

    VULKAN_CHECK(vkCreateDevice(physicalDevice, &deviceCreateInfo, nullptr, &logicalDevice));

    volkLoadDevice(logicalDevice);

    initializeQueues();

    VmaVulkanFunctions vulkanFunctions {
      .vkGetInstanceProcAddr = vkGetInstanceProcAddr,
      .vkGetDeviceProcAddr = vkGetDeviceProcAddr,
      .vkGetPhysicalDeviceProperties = vkGetPhysicalDeviceProperties,
      .vkGetPhysicalDeviceMemoryProperties = vkGetPhysicalDeviceMemoryProperties,
      .vkAllocateMemory = vkAllocateMemory,
      .vkFreeMemory = vkFreeMemory,
      .vkMapMemory = vkMapMemory,
      .vkUnmapMemory = vkUnmapMemory,
      .vkFlushMappedMemoryRanges = vkFlushMappedMemoryRanges,
      .vkInvalidateMappedMemoryRanges = vkInvalidateMappedMemoryRanges,
      .vkBindBufferMemory = vkBindBufferMemory,
      .vkBindImageMemory = vkBindImageMemory,
      .vkGetBufferMemoryRequirements = vkGetBufferMemoryRequirements,
      .vkGetImageMemoryRequirements = vkGetImageMemoryRequirements,
      .vkCreateBuffer = vkCreateBuffer,
      .vkDestroyBuffer = vkDestroyBuffer,
      .vkCreateImage = vkCreateImage,
      .vkDestroyImage = vkDestroyImage,
      .vkCmdCopyBuffer = vkCmdCopyBuffer,
    };

    VmaAllocatorCreateInfo allocatorCreateInfo = {
      .physicalDevice = physicalDevice,
      .device = logicalDevice,
      .pVulkanFunctions = &vulkanFunctions,
      .instance = renderingDevice.getInstance()
    };
    vmaCreateAllocator(&allocatorCreateInfo, &memoryAllocator);
  }

  void VulkanDevice::initializeQueues() {
    const std::uint32_t frameBufferCount = 4;
    for (auto &graphicsQueue: graphicsQueues) {
      vkGetDeviceQueue(logicalDevice, graphicsQueue.familyIndex, graphicsQueue.queueIndex, &graphicsQueue.queue);
      graphicsQueue.initialize(graphicsQueue.queue, frameBufferCount);
    }

    for (auto &computeQueue: computeQueues) {
      vkGetDeviceQueue(logicalDevice, computeQueue.familyIndex, computeQueue.queueIndex, &computeQueue.queue);
      computeQueue.initialize(computeQueue.queue, frameBufferCount);
    }

    for (auto &transferQueue: transferQueues) {
      vkGetDeviceQueue(logicalDevice, transferQueue.familyIndex, transferQueue.queueIndex, &transferQueue.queue);
      transferQueue.initialize(transferQueue.queue, frameBufferCount);
    }
  }

  std::vector<const char*> VulkanDevice::getExtensions() const {
    static bool foundExtensions = false;
    static std::vector<const char*> extensions;

    if (foundExtensions)
      return extensions;

    std::uint32_t extensionCount = 0;
    vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &extensionCount, nullptr);

    std::vector<VkExtensionProperties> availableExtensions(extensionCount);
    vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &extensionCount, availableExtensions.data());
    for (size_t extensionIndex = 0; extensionIndex < extensionCount; extensionIndex++) {
      const std::string extensionName = availableExtensions[extensionIndex].extensionName;
      LOG_CORE_TRACE("Found device extension \"{}\"", extensionName);

      if (extensionName == VK_KHR_MAINTENANCE2_EXTENSION_NAME) {
        extensions.push_back(VK_KHR_MAINTENANCE2_EXTENSION_NAME);
        // bCanRestrictImageViewUsage = true;
      } else if (extensionName == VK_EXT_SHADER_SUBGROUP_VOTE_EXTENSION_NAME)
        extensions.push_back(VK_EXT_SHADER_SUBGROUP_VOTE_EXTENSION_NAME);
      else if (extensionName == VK_EXT_SHADER_VIEWPORT_INDEX_LAYER_EXTENSION_NAME) {
        extensions.push_back(VK_EXT_SHADER_VIEWPORT_INDEX_LAYER_EXTENSION_NAME);
        // mRealCapabilities->setCapability(RSC_VP_RT_INDEX_ANY_SHADER);
      }

#ifdef VK_EXT_mesh_shader
      else if (extensionName == VK_EXT_MESH_SHADER_EXTENSION_NAME) {
        extensions.push_back(VK_EXT_MESH_SHADER_EXTENSION_NAME);
        extensions.push_back(VK_KHR_SPIRV_1_4_EXTENSION_NAME);
        // mRealCapabilities->setCapability(RSC_MESH_PROGRAM);
        //
        // mDescriptorSetBindings[0].stageFlags |= VK_SHADER_STAGE_MESH_BIT_NV;
      }
#endif

      if (renderingDevice.validationLayersEnabled())
        extensions.push_back(VK_EXT_DEBUG_MARKER_EXTENSION_NAME);

      extensions.push_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);
    }

    foundExtensions = true;
    return extensions;
  }

  void VulkanDevice::findGraphicsQueue(std::vector<std::uint32_t> &usedQueuesCount) {
    const size_t familyCount = queueFamilyProperties.size();
    for (size_t familyIndex = 0; familyIndex < familyCount; familyIndex++) {
      if (queueFamilyProperties[familyIndex].queueFlags & VK_QUEUE_GRAPHICS_BIT &&
          usedQueuesCount[familyIndex] < queueFamilyProperties[familyIndex].queueCount) {
        LOG_CORE_TRACE(
          "Found graphics queue in family {} with {} queues.",
          familyIndex,
          queueFamilyProperties[familyIndex].queueCount
        );

        graphicsQueues.emplace_back(*this);
        VulkanQueue &queue = graphicsQueues.back();
        queue.type = VulkanQueue::Type::Transfer;
        queue.familyIndex = static_cast<std::uint32_t>(familyIndex);
        queue.queueIndex = usedQueuesCount[familyIndex];

        usedQueuesCount[familyIndex]++;
      }
    }

    if (graphicsQueues.empty())
      LOG_CORE_CRITICAL("GPU does not expose Graphics queue. Cannot be used for rendering");
  }

  void VulkanDevice::findComputeQueue(std::vector<std::uint32_t> &usedQueuesCount) {
    const size_t familyCount = queueFamilyProperties.size();
    for (size_t familyIndex = 0; familyIndex < familyCount; familyIndex++) {
      const bool isCompute = queueFamilyProperties[familyIndex].queueFlags & VK_QUEUE_COMPUTE_BIT;
      if (isCompute && usedQueuesCount[familyIndex] < queueFamilyProperties[familyIndex].queueCount) {
        LOG_CORE_TRACE(
          "Found compute queue in family {} with {} queues.",
          familyIndex,
          queueFamilyProperties[familyIndex].queueCount
        );

        computeQueues.emplace_back(*this);
        VulkanQueue &queue = computeQueues.back();
        queue.type = VulkanQueue::Type::Compute;
        queue.familyIndex = static_cast<std::uint32_t>(familyIndex);
        queue.queueIndex = usedQueuesCount[familyIndex];

        usedQueuesCount[familyIndex]++;
      }
    }
  }

  void VulkanDevice::findTransferQueues(std::vector<std::uint32_t> &usedQueueCount) {
    const size_t familyCount = queueFamilyProperties.size();
    for (size_t familyIndex = 0; familyIndex < familyCount; familyIndex++) {
      const bool isTransfer = queueFamilyProperties[familyIndex].queueFlags & VK_QUEUE_TRANSFER_BIT;
      const bool isGraphicsOrCompute = queueFamilyProperties[familyIndex].queueFlags & (VK_QUEUE_GRAPHICS_BIT | VK_QUEUE_COMPUTE_BIT);
      if (isTransfer && !isGraphicsOrCompute && usedQueueCount[familyIndex] < queueFamilyProperties[familyIndex].queueCount) {
        LOG_CORE_TRACE(
          "Found transfer queue in family {} with {} queues.",
          familyIndex,
          queueFamilyProperties[familyIndex].queueCount
        );

        transferQueues.emplace_back(*this);
        VulkanQueue &queue = transferQueues.back();
        queue.type = VulkanQueue::Type::Transfer;
        queue.familyIndex = static_cast<std::uint32_t>(familyIndex);
        queue.queueIndex = usedQueueCount[familyIndex];

        usedQueueCount[familyIndex]++;
      }
    }
  }

  VkBool32 VulkanDevice::debugCallback(
    VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
    VkDebugUtilsMessageTypeFlagsEXT messageType,
    const VkDebugUtilsMessengerCallbackDataEXT *callbackData,
    void *userData
  ) {
    switch (messageSeverity) {
      case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
        LOG_CORE_TRACE("Vulkan {}", callbackData->pMessage);
        break;

      case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
        LOG_CORE_INFO("Vulkan {}", callbackData->pMessage);
        break;

      case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
        LOG_CORE_WARNING("Vulkan {}", callbackData->pMessage);
        break;

      case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
        LOG_CORE_ERROR("Vulkan {}", callbackData->pMessage);
        break;

      default:
        break;
    }

    return VK_FALSE;
  }
}
