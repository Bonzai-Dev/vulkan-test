#include "vulkan_window.hpp"

namespace Core::Graphics {
  VulkanWindow::VulkanWindow(const DisplayInfo &displayInfo, const WindowOptions &windowOptions, std::uint64_t windowFlags) :
  Window(displayInfo, windowOptions, windowFlags) {
  }
}
