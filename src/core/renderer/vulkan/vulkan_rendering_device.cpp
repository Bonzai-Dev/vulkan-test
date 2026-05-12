#include <map>
#include <SDL3/SDL_vulkan.h>
#include <core/application.hpp>
#include <core/logger.hpp>
#include "vulkan_rendering_device.hpp"

namespace Core::Graphics {
  VulkanRenderingDevice::VulkanRenderingDevice(const char *appName) {
    if (volkInitialize() != VK_SUCCESS) {
      LOG_CORE_CRITICAL("Failed to load Vulkan. Vulkan drivers may be missing on your system");
      return;
    }

    createInstance(appName, getExtensions(), getInstanceLayers());

    currentDevice = chooseDevice();
    currentDevice->createLogicalDevice();

    LOG_CORE_INFO("Rendering using {}", currentDevice->getName());

    const std::uint32_t version = currentDevice->getProperties().apiVersion;
    LOG_CORE_DEBUG(
      "{} supports Vulkan up to {}.{}.{}",
      currentDevice->getName(),
      VK_VERSION_MAJOR(version), VK_VERSION_MINOR(version),
      VK_VERSION_PATCH(version)
    );
  }

  VulkanRenderingDevice::~VulkanRenderingDevice() {
    if (debugMessenger)
      vkDestroyDebugUtilsMessengerEXT(instance, debugMessenger, nullptr);
  }

  void VulkanRenderingDevice::createWindow() {

  }

  void VulkanRenderingDevice::createInstance(
    const char *appName,
    const std::vector<const char*> &extensions,
    const std::vector<const char*> &layers
  ) {
    // Creating vulkan instance
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
      .enabledLayerCount = static_cast<uint32_t>(layers.size()),
      .ppEnabledLayerNames = layers.data(),
      .enabledExtensionCount = static_cast<uint32_t>(extensions.size()),
      .ppEnabledExtensionNames = extensions.data()
    };

    // Setting up debug messenger
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
        .pfnUserCallback = &VulkanRenderingDevice::debugCallback,
        .pUserData = nullptr
      };
      createInfo.pNext = &debugMessengerCreateInfo;
    }

    VULKAN_CHECK(vkCreateInstance(&createInfo, nullptr, &instance));
    volkLoadInstanceOnly(instance);

    if (validationLayersEnabled())
      VULKAN_CHECK(vkCreateDebugUtilsMessengerEXT(instance, &debugMessengerCreateInfo, nullptr, &debugMessenger));
  }

  // void VulkanRenderingDevice::createWindow() {
  //
  // }

  std::vector<VulkanDevice> &VulkanRenderingDevice::getDevices() const {
    static bool deviceFetched = false;
    static std::vector<VulkanDevice> devices;

    if (deviceFetched)
      return devices;

    std::vector<VkPhysicalDevice> physicalDevices;
    uint32_t deviceCount = 0;
    vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);
    if (deviceCount == 0) {
      LOG_CORE_CRITICAL("Cannot find any GPUs with Vulkan support");
      return devices;
    }
    physicalDevices.resize(deviceCount);
    vkEnumeratePhysicalDevices(instance, &deviceCount, physicalDevices.data());

    for (size_t deviceIndex = 0; deviceIndex < deviceCount; deviceIndex++) {
      VulkanDevice device(instance);
      device.createPhysicalDevice(physicalDevices[deviceIndex]);
      devices.push_back(std::move(device));
      LOG_CORE_INFO("Found {}", devices[deviceIndex].getName());
    }

    if (devices.empty()) {
      LOG_CORE_CRITICAL("No suitable Vulkan devices found");
      return devices;
    }

    deviceFetched = true;

    return devices;
  }

  VulkanDevice *VulkanRenderingDevice::chooseDevice() {
    static bool deviceChosen = false;
    static std::map<int, VulkanDevice*> deviceRankings;
    if (deviceChosen)
      return deviceRankings.rbegin()->second;

    for (auto &device: getDevices())
      deviceRankings.insert({rateDevice(device), &device});

    deviceChosen = true;
    return deviceRankings.rbegin()->second;
  }

  VkBool32 VulkanRenderingDevice::debugCallback(
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

  std::vector<const char*> VulkanRenderingDevice::getInstanceLayers() {
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

  std::vector<const char*> VulkanRenderingDevice::getExtensions() {
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

  int VulkanRenderingDevice::rateDevice(const VulkanDevice &device) {
    int score = 0;

    const VkPhysicalDeviceProperties deviceProperties = device.getProperties();
    const VkPhysicalDeviceFeatures deviceFeatures = device.getFeatures();

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

  std::string vulkanResultToString(VkResult result) {
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
