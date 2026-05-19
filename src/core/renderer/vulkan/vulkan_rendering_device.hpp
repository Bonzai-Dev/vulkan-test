#pragma once
#include <cstdint>
#include <vector>
#include <core/renderer/rendering_device.hpp>
#include "vulkan_device.hpp"
#include "volk.h"

namespace Core::Graphics {
  class VulkanRenderingDevice : public RenderingDevice {
    public:
      explicit VulkanRenderingDevice(const char *appName, const DisplayInfo &displayInfo);

      ~VulkanRenderingDevice() override = default;

      VulkanRenderingDevice(const VulkanRenderingDevice &other) = delete;

      VulkanRenderingDevice &operator=(const VulkanRenderingDevice &other) = delete;

      VulkanRenderingDevice(VulkanRenderingDevice &&other) noexcept = delete;

      VulkanRenderingDevice &operator=(VulkanRenderingDevice &&other) noexcept = delete;

      const std::vector<const char*> &getExtensions();

      bool validationLayersEnabled() const { return validationLayersSupported; }

      VkInstance getInstance() const { return instance; }

      const VulkanDevice *getCurrentDevice() const { return currentDevice; }

      const std::vector<const char*> &getInstanceLayers();

      int rateDevice(const VulkanDevice *device);

      void createWindow(const WindowOptions &options) override;

      void render() override;

      static VkFormat convertPixelFormat(PixelFormat format);

      static std::string vulkanResultToString(VkResult result);

    private:
      bool validationLayersSupported = false;

      std::vector<std::unique_ptr<VulkanDevice>> devices;
      std::map<int, VulkanDevice*> deviceRankings;
      VulkanDevice *currentDevice;
      VkInstance instance = VK_NULL_HANDLE;
  };

  #define VULKAN_CHECK(vulkanCall) { \
    VkResult result = vulkanCall; \
    if (result != VK_SUCCESS) { \
      std::string vkfunc = #vulkanCall; \
      vkfunc = vkfunc.substr(0, vkfunc.find('(')); \
      throw std::runtime_error("Vulkan error: " + vkfunc + " failed with " + VulkanRenderingDevice::vulkanResultToString(result)); \
    } \
  }
}
