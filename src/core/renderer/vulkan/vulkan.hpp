#pragma once
#include <vulkan/vulkan.hpp>
#include "util.hpp"

namespace Core::Graphics {
  class Vulkan {
    public:
      Vulkan(const char *appName);

      ~Vulkan();

      static std::vector<const char*> getExtensions();

      static bool validationLayersEnabled() { return validationLayersSupported; }

      VkInstance getInstance() const { return instance; }

    private:
      void createInstance(
        const char *appName,
        const std::vector<const char*> &extensions,
        const std::vector<const char*> &layers
      );

      static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
        VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
        VkDebugUtilsMessageTypeFlagsEXT messageType,
        const VkDebugUtilsMessengerCallbackDataEXT *callbackData,
        void *userData
      );

      static inline bool validationLayersSupported;

      std::vector<const char*> instanceLayers;
      VkInstance instance = VK_NULL_HANDLE;
      VkDebugUtilsMessengerEXT debugMessenger = VK_NULL_HANDLE;
  };
}

#define VULKAN_CHECK(vkcall) { \
  VkResult result = vkcall; \
  if (result != VK_SUCCESS) { \
    std::string vkfunc = #vkcall; \
    vkfunc = vkfunc.substr(0, vkfunc.find('(')); \
    throw std::runtime_error("Vulkan error: " + vkfunc + " failed with " + vulkanResultToString(result)); \
    return; \
  } \
}
