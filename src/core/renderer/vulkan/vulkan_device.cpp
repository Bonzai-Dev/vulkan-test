#include "vulkan_device.hpp"

namespace Core::Graphics {
  VulkanDevice::VulkanDevice(VkDevice device, VkPhysicalDevice physicalDevice) :
  device(device), physicalDevice(physicalDevice) {
  }
}
