#include <SDL3/SDL_vulkan.h>
#include "vulkan_window.hpp"
#include "vulkan_rendering_device.hpp"

namespace Core::Graphics {
  VulkanWindow::VulkanWindow(const VkInstance &instance, const DisplayInfo &displayInfo, const WindowOptions &windowOptions) :
  options(windowOptions), instance(instance), displayInfo(displayInfo) {
    std::uint32_t width = options.width;
    std::uint32_t height = options.height;

    if (options.fullScreen)
      windowFlags |= SDL_WINDOW_FULLSCREEN;

    if (options.resizable)
      windowFlags |= SDL_WINDOW_RESIZABLE;

    windowFlags |= SDL_WINDOW_VULKAN;

    window = SDL_CreateWindow(
      options.windowName,
      static_cast<int>(width), static_cast<int>(height),
      windowFlags
    );

    if (!window) {
      LOG_CORE_ERROR("Failed to create window: {}", SDL_GetError());
      return;
    }

    SDL_SetWindowRelativeMouseMode(window, options.mouseLocked);
  }

  VulkanWindow::VulkanWindow(VulkanWindow &&other) noexcept :
    instance(other.instance),
    mouseFocused(other.mouseFocused),
    keyboardFocused(other.keyboardFocused),
    displayInfo(other.displayInfo),
    window(other.window),
    windowFlags(other.windowFlags) {
    other.window = nullptr;
  }

  VulkanWindow::~VulkanWindow() {
    SDL_DestroyWindow(window);
  }

  void VulkanWindow::render() {

  }
}
