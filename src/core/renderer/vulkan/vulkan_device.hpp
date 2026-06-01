#pragma once
#include <vulkan/vulkan.h>

namespace Core::Graphics {
  class VulkanDevice {
    public:
      VulkanDevice(VkDevice device, VkPhysicalDevice physicalDevice);

      VkDevice getDevice() const { return device; }

      VkPhysicalDevice getPhysicalDevice() const { return physicalDevice; }

    private:
      VkDevice device;
      VkPhysicalDevice physicalDevice;

      VkPhysicalDeviceProperties deviceProperties{};
      VkPhysicalDeviceMemoryProperties deviceMemoryProperties{};
      VkPhysicalDeviceFeatures deviceFeatures{};
  };
}