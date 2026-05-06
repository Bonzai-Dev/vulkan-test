#include "vulkan_window.hpp"

namespace Core::Graphics {
  VulkanWindow::VulkanWindow(const WindowOptions &windowOptions, std::uint64_t windowFlags) :
  Window(windowOptions, windowFlags), swapChain(*window) {
  }
}
