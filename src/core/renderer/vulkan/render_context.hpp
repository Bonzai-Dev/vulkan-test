#pragma once
#include <cstdint>
#include <vector>
#include <core/renderer/render_context.hpp>
#include "device.hpp"
#include "volk.h"

namespace Core::Graphics {
  class VulkanRenderContext : public RenderContext {
    public:
      explicit VulkanRenderContext(const char *appName);

      ~VulkanRenderContext() override;

      VulkanRenderContext(const VulkanRenderContext &other) = delete;

      VulkanRenderContext &operator=(const VulkanRenderContext &other) = delete;

      VulkanRenderContext(VulkanRenderContext &&other) noexcept = delete;

      VulkanRenderContext &operator=(VulkanRenderContext &&other) noexcept = delete;

      static std::vector<const char*> getExtensions();

      static bool validationLayersEnabled() { return validationLayersSupported; }

      VkInstance getInstance() const { return instance; }

      const VulkanDevice *getCurrentDevice() const { return currentDevice.get(); }

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

      std::unique_ptr<VulkanDevice> currentDevice = nullptr;
  };
}
