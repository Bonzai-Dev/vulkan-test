#include <SDL3/SDL_video.h>
#include <SDL3/SDL_mouse.h>
#include <core/window/window_manager.hpp>
#include <core/events/window.hpp>
#include "window.hpp"

namespace Core::Graphics {
  Window::Window(
    const DisplayInfo &displayInfo,
    const WindowOptions &windowOptions
  ) : options(windowOptions), displayInfo(displayInfo) {
    std::uint32_t width = windowOptions.width;
    std::uint32_t height = windowOptions.height;

    if (options.fullScreen) {
      windowFlags |= SDL_WINDOW_FULLSCREEN;
      width = displayInfo.width;
      height = displayInfo.height;
    }

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

  Window::Window(Window &&other) noexcept : mouseFocused(other.mouseFocused),
    keyboardFocused(other.keyboardFocused),
    displayInfo(other.displayInfo),
    window(other.window),
    windowFlags(other.windowFlags) {
    other.window = nullptr;
  }

  Window::~Window() {
    SDL_DestroyWindow(window);
  }

  void Window::render() const {
    // TODO: rendering logic here
  }
}
