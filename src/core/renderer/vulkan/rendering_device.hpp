#pragma once
#include <cstdint>
#include <vector>
#include <core/renderer/rendering_device.hpp>
#include "device.hpp"
#include "volk.h"

namespace Core::Graphics {
  class VulkanRenderingDevice : public RenderingDevice {
    public:
      explicit VulkanRenderingDevice(const char *appName);

      ~VulkanRenderingDevice() override;

      VulkanRenderingDevice(const VulkanRenderingDevice &other) = delete;

      VulkanRenderingDevice &operator=(const VulkanRenderingDevice &other) = delete;

      VulkanRenderingDevice(VulkanRenderingDevice &&other) noexcept = delete;

      VulkanRenderingDevice &operator=(VulkanRenderingDevice &&other) noexcept = delete;

      static std::vector<const char*> getExtensions();

      static bool validationLayersEnabled() { return validationLayersSupported; }

      VkInstance getInstance() const { return instance; }

      const VulkanDevice *getCurrentDevice() const { return currentDevice; }

      std::vector<VulkanDevice> &getDevices() const;

      std::vector<const char*> getInstanceLayers();

      int rateDevice(const VulkanDevice &device);

    private:
      void createInstance(
        const char *appName,
        const std::vector<const char*> &extensions,
        const std::vector<const char*> &layers
      );

      VulkanDevice *chooseDevice();

      static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
        VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
        VkDebugUtilsMessageTypeFlagsEXT messageType,
        const VkDebugUtilsMessengerCallbackDataEXT *callbackData,
        void *userData
      );

      static inline bool validationLayersSupported;

      VkInstance instance = VK_NULL_HANDLE;
      VkDebugUtilsMessengerEXT debugMessenger = VK_NULL_HANDLE;

      VulkanDevice *currentDevice = nullptr;
  };

  std::string vulkanResultToString(VkResult result);

  #define VULKAN_CHECK(vulkanCall) { \
    VkResult result = vulkanCall; \
    if (result != VK_SUCCESS) { \
      std::string vkfunc = #vulkanCall; \
      vkfunc = vkfunc.substr(0, vkfunc.find('(')); \
      throw std::runtime_error("Vulkan error: " + vkfunc + " failed with " + vulkanResultToString(result)); \
    } \
  }
}
