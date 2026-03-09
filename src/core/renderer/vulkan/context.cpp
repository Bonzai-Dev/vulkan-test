#include <SDL3/SDL_init.h>
#include <SDL3/SDL_vulkan.h>
#include <core/application/logger.hpp>
#include <core/application/application.hpp>
#include "volk.h"
#include "context.hpp"

namespace Core::Graphics {
  VulkanContext::VulkanContext(const char *appName) {
    if (volkInitialize() != VK_SUCCESS) {
      LOG_CORE_CRITICAL("Failed to load Vulkan. Vulkan drivers may be missing on your system.");
      return;
    }

    // Adding vulkan layers
    uint32_t instanceLayerPropertyCount;
    vkEnumerateInstanceLayerProperties(&instanceLayerPropertyCount, nullptr);
    std::vector<VkLayerProperties> instanceLayerProperties(instanceLayerPropertyCount);
    vkEnumerateInstanceLayerProperties(&instanceLayerPropertyCount, instanceLayerProperties.data());
    for (size_t layerIndex = 0; layerIndex < instanceLayerPropertyCount; layerIndex++) {
      const std::string layerName = instanceLayerProperties[layerIndex].layerName;
      if (Application::debugEnabled && layerName == "VK_LAYER_KHRONOS_validation") {
        validationLayersSupported = true;
        instanceLayers.push_back("VK_LAYER_KHRONOS_validation");
        LOG_CORE_INFO("Validation layers enabled.");
      }
      LOG_CORE_DEBUG("Found instance layer: \"{}\"", layerName);
    }

    createInstance(appName, getExtensions(), instanceLayers);
    currentDevice.initialize(0);
  }

  VulkanContext::~VulkanContext() {
    if (debugMessenger)
      vkDestroyDebugUtilsMessengerEXT(instance, debugMessenger, nullptr);
  }

  void VulkanContext::createInstance(
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
        .pfnUserCallback = &VulkanContext::debugCallback,
        .pUserData = nullptr
      };
      createInfo.pNext = &debugMessengerCreateInfo;
    }

    VULKAN_CHECK(vkCreateInstance(&createInfo, nullptr, &instance));
    volkLoadInstanceOnly(instance);

    if (validationLayersEnabled())
      VULKAN_CHECK(vkCreateDebugUtilsMessengerEXT(instance, &debugMessengerCreateInfo, nullptr, &debugMessenger));
  }

  std::vector<const char*> VulkanContext::getExtensions() {
    static bool foundExtensions = false;
    static uint32_t extensionCount = 0;
    static char const *const*extensions = SDL_Vulkan_GetInstanceExtensions(&extensionCount);
    static std::vector<const char*> extensionList(extensions, extensions + extensionCount);

    if (foundExtensions)
      return extensionList;

    if (validationLayersEnabled())
      extensionList.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);

    if (Application::debugEnabled) {
      for (auto &extension: extensionList)
        LOG_CORE_DEBUG("Found extension: \"{}\"", extension);
    }

    foundExtensions = true;
    return extensionList;
  }

  VkBool32 VulkanContext::debugCallback(
    VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
    VkDebugUtilsMessageTypeFlagsEXT messageType,
    const VkDebugUtilsMessengerCallbackDataEXT *callbackData,
    void *userData
  ) {
    switch (messageSeverity) {
      case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
        LOG_CORE_TRACE("Vulkan diagnostic: {}", callbackData->pMessage);
        break;

      case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
        LOG_CORE_INFO("Vulkan info: {}", callbackData->pMessage);
        break;

      case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
        LOG_CORE_WARNING("Vulkan warning: {}", callbackData->pMessage);
        break;

      case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
        LOG_CORE_ERROR("Vulkan error: {}", callbackData->pMessage);
        break;

      default:
        break;
    }

    return VK_FALSE;
  }
}
