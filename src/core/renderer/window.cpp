#include <SDL3/SDL_video.h>
#include <SDL3/SDL_mouse.h>
#include <core/window/window_manager.hpp>
#include <core/events/window.hpp>
#include "window.hpp"

namespace Core::Graphics {
  Window::Window(
    const DisplayInfo &displayInfo,
    const WindowOptions &windowOptions,
    std::uint64_t windowFlags
  ) : options(windowOptions), displayInfo(displayInfo), windowFlags(windowFlags) {
    std::uint32_t width = windowOptions.width;
    std::uint32_t height = windowOptions.height;

    if (windowOptions.fullScreen) {
      width = displayInfo.width;
      height = displayInfo.height;
    }

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
