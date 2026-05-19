#include <map>
#include <SDL3/SDL_vulkan.h>
#include <core/application/application.hpp>
#include <core/application/logger.hpp>
#include "vulkan_rendering_device.hpp"
#include "vulkan_window.hpp"

using namespace Core::Events;

namespace Core::Graphics {
  VulkanRenderingDevice::VulkanRenderingDevice(const char *appName, const DisplayInfo &displayInfo) :
  RenderingDevice(appName, displayInfo) {
    if (volkInitialize() != VK_SUCCESS) {
      LOG_CORE_CRITICAL("Failed to load Vulkan. Vulkan drivers may be missing on your system");
      return;
    }

    // Creating vulkan instance
    VkDebugUtilsMessengerEXT debugMessenger = VK_NULL_HANDLE;
    const VkApplicationInfo appInfo{
      .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
      .pApplicationName = appName,
      .applicationVersion = VK_MAKE_VERSION(1, 0, 0),
      .pEngineName = "Vulkan renderer",
      .engineVersion = VK_MAKE_VERSION(1, 0, 0),
      .apiVersion = VK_MAKE_VERSION(1, 3, 0),
    };

    VkInstanceCreateInfo createInfo{
      .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
      .pApplicationInfo = &appInfo,
      .enabledLayerCount = static_cast<uint32_t>(getInstanceLayers().size()),
      .ppEnabledLayerNames = getInstanceLayers().data(),
      .enabledExtensionCount = static_cast<uint32_t>(getExtensions().size()),
      .ppEnabledExtensionNames = getExtensions().data()
    };

    VkDebugUtilsMessengerCreateInfoEXT debugMessengerCreateInfo{};
    if (validationLayersEnabled()) {
      debugMessengerCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT,
        .pNext = nullptr,
        .messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
                           VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
                           VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT,
        .messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
                       VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
                       VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT,
        .pfnUserCallback = &VulkanDevice::debugCallback,
        .pUserData = nullptr
      };
      createInfo.pNext = &debugMessengerCreateInfo;
    }

    VULKAN_CHECK(vkCreateInstance(&createInfo, nullptr, &instance));
    volkLoadInstanceOnly(instance);

    if (validationLayersEnabled())
      VULKAN_CHECK(vkCreateDebugUtilsMessengerEXT(
        instance, &debugMessengerCreateInfo, nullptr, &debugMessenger
      ));

    // Loading devices
    std::vector<VkPhysicalDevice> physicalDevices;
    uint32_t deviceCount = 0;
    vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);
    if (deviceCount == 0)
      LOG_CORE_CRITICAL("Cannot find any GPUs with Vulkan support");

    physicalDevices.resize(deviceCount);
    vkEnumeratePhysicalDevices(instance, &deviceCount, physicalDevices.data());

    for (size_t deviceIndex = 0; deviceIndex < deviceCount; deviceIndex++) {
      devices.emplace_back(std::make_unique<VulkanDevice>(*this));
      devices[deviceIndex]->createPhysicalDevice(physicalDevices[deviceIndex]);
      LOG_CORE_INFO("Found {}", devices[deviceIndex]->getName());
    }

    if (devices.empty()) {
      LOG_CORE_CRITICAL("No suitable Vulkan devices found");
      return;
    }

    for (auto &device: devices)
      deviceRankings.insert({rateDevice(device.get()), device.get()});

    currentDevice = deviceRankings.rbegin()->second;
    currentDevice->createLogicalDevice(debugMessenger);

    LOG_CORE_INFO("Rendering using {}", currentDevice->getName());

    const std::uint32_t version = currentDevice->getProperties().apiVersion;
    LOG_CORE_DEBUG(
      "{} supports Vulkan up to {}.{}.{}",
      currentDevice->getName(),
      VK_VERSION_MAJOR(version), VK_VERSION_MINOR(version),
      VK_VERSION_PATCH(version)
    );
  }

  void VulkanRenderingDevice::createWindow(const WindowOptions &options) {
    VulkanWindow window = VulkanWindow(*currentDevice, instance, displayInfo, options);
    windows.emplace(window.getId(), std::move(window));
  }

  void VulkanRenderingDevice::render() {
    for (auto &[windowId, window]: windows)
      window.render();
  }

  const std::vector<const char*> &VulkanRenderingDevice::getInstanceLayers() {
    static bool foundLayers = false;
    static std::vector<const char*> instanceLayers;

    if (foundLayers)
      return instanceLayers;

    const char *validationLayer = "VK_LAYER_KHRONOS_validation";
    uint32_t instanceLayerPropertyCount;
    vkEnumerateInstanceLayerProperties(&instanceLayerPropertyCount, nullptr);
    std::vector<VkLayerProperties> instanceLayerProperties(instanceLayerPropertyCount);
    vkEnumerateInstanceLayerProperties(&instanceLayerPropertyCount, instanceLayerProperties.data());
    for (size_t layerIndex = 0; layerIndex < instanceLayerPropertyCount; layerIndex++) {
      const std::string layerName = instanceLayerProperties[layerIndex].layerName;
      if (Application::debugEnabled && layerName == validationLayer) {
        validationLayersSupported = true;
        instanceLayers.push_back(validationLayer);
      }
      LOG_CORE_DEBUG("Found instance layer \"{}\"", layerName);
    }

    if constexpr (Application::debugEnabled) {
      if (std::ranges::find(instanceLayers, validationLayer) != instanceLayers.end())
        LOG_CORE_INFO("Validation layer \"{}\" has been found", validationLayer);
      else
        LOG_CORE_WARNING("Validation layers for Vulkan has been requested while in debug mode, but are not available");
    }

    return instanceLayers;
  }

   const std::vector<const char*> &VulkanRenderingDevice::getExtensions() {
    static bool foundExtensions = false;
    static uint32_t extensionCount = 0;
    static char const *const*extensions = SDL_Vulkan_GetInstanceExtensions(&extensionCount);
    static std::vector<const char*> extensionList(extensions, extensions + extensionCount);

    if (foundExtensions)
      return extensionList;

    if (validationLayersEnabled()) {
      extensionList.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
      extensionList.push_back(VK_EXT_DEBUG_REPORT_EXTENSION_NAME);
    }

    if (Application::debugEnabled) {
      for (auto &extension: extensionList)
        LOG_CORE_DEBUG("Found extension \"{}\"", extension);
    }

    foundExtensions = true;
    return extensionList;
  }

  int VulkanRenderingDevice::rateDevice(const VulkanDevice *device) {
    int score = 0;

    const VkPhysicalDeviceProperties deviceProperties = device->getProperties();
    const VkPhysicalDeviceFeatures deviceFeatures = device->getFeatures();

    // Discrete GPUs have a significant performance advantage
    if (deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
      score += 1000;

    // Maximum possible size of textures affects graphics quality
    score += static_cast<int>(deviceProperties.limits.maxImageDimension2D);

    // Application can't function without geometry shaders
    if (!deviceFeatures.geometryShader)
      return 0;

    return score;
  }

  VkFormat VulkanRenderingDevice::convertPixelFormat(PixelFormat format) {
    VkFormat converted = VK_FORMAT_UNDEFINED;
    switch (format) {
      case PixelFormat::R8G8B8A8:
        converted = VK_FORMAT_R8G8B8A8_UNORM;
        break;
      case PixelFormat::B8G8R8A8:
        converted = VK_FORMAT_B8G8R8A8_UNORM;
        break;
      default:
        break;
    }

    return converted;
  }

  std::string VulkanRenderingDevice::vulkanResultToString(VkResult result) {
    switch (result) {
      case VK_SUCCESS: return "VK_SUCCESS";
      case VK_NOT_READY: return "VK_NOT_READY";
      case VK_TIMEOUT: return "VK_TIMEOUT";
      case VK_EVENT_SET: return "VK_EVENT_SET";
      case VK_EVENT_RESET: return "VK_EVENT_RESET";
      case VK_INCOMPLETE: return "VK_INCOMPLETE";
      case VK_ERROR_OUT_OF_HOST_MEMORY: return "VK_ERROR_OUT_OF_HOST_MEMORY";
      case VK_ERROR_OUT_OF_DEVICE_MEMORY: return "VK_ERROR_OUT_OF_DEVICE_MEMORY";
      case VK_ERROR_INITIALIZATION_FAILED: return "VK_ERROR_INITIALIZATION_FAILED";
      case VK_ERROR_DEVICE_LOST: return "VK_ERROR_DEVICE_LOST";
      case VK_ERROR_MEMORY_MAP_FAILED: return "VK_ERROR_MEMORY_MAP_FAILED";
      case VK_ERROR_LAYER_NOT_PRESENT: return "VK_ERROR_LAYER_NOT_PRESENT";
      case VK_ERROR_EXTENSION_NOT_PRESENT: return "VK_ERROR_EXTENSION_NOT_PRESENT";
      case VK_ERROR_FEATURE_NOT_PRESENT: return "VK_ERROR_FEATURE_NOT_PRESENT";
      case VK_ERROR_INCOMPATIBLE_DRIVER: return "VK_ERROR_INCOMPATIBLE_DRIVER";
      case VK_ERROR_TOO_MANY_OBJECTS: return "VK_ERROR_TOO_MANY_OBJECTS";
      case VK_ERROR_FORMAT_NOT_SUPPORTED: return "VK_ERROR_FORMAT_NOT_SUPPORTED";
      case VK_ERROR_FRAGMENTED_POOL: return "VK_ERROR_FRAGMENTED_POOL";
      case VK_ERROR_OUT_OF_POOL_MEMORY: return "VK_ERROR_OUT_OF_POOL_MEMORY";
      case VK_ERROR_INVALID_EXTERNAL_HANDLE: return "VK_ERROR_INVALID_EXTERNAL_HANDLE";
      case VK_ERROR_SURFACE_LOST_KHR: return "VK_ERROR_SURFACE_LOST_KHR";
      case VK_ERROR_NATIVE_WINDOW_IN_USE_KHR: return "VK_ERROR_NATIVE_WINDOW_IN_USE_KHR";
      case VK_SUBOPTIMAL_KHR: return "VK_SUBOPTIMAL_KHR";
      case VK_ERROR_OUT_OF_DATE_KHR: return "VK_ERROR_OUT_OF_DATE_KHR";
      case VK_ERROR_INCOMPATIBLE_DISPLAY_KHR: return "VK_ERROR_INCOMPATIBLE_DISPLAY_KHR";
      case VK_ERROR_VALIDATION_FAILED_EXT: return "VK_ERROR_VALIDATION_FAILED_EXT";
      case VK_ERROR_INVALID_SHADER_NV: return "VK_ERROR_INVALID_SHADER_NV";
      case VK_ERROR_NOT_PERMITTED_EXT: return "VK_ERROR_NOT_PERMITTED_EXT";
      default:
        return "Unknown VkResult: " + std::to_string(result);
    }
  }
}
