#include "vulkan_window.hpp"

namespace Core::Graphics {
  VulkanWindow::VulkanWindow(const VulkanDevice &device, const VkInstance &instance, const DisplayInfo &displayInfo, const WindowOptions &windowOptions) :
  Window(displayInfo, windowOptions),
  swapChain(instance, device, *window, windowOptions) {
  }
}
