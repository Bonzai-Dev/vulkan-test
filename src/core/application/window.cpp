#include <SDL3/SDL_video.h>
#include <SDL3/SDL_mouse.h>
#include "window.hpp"
#include "application.hpp"

namespace Core {
  Window::Window(const WindowOptions& options) {
    if (options.fullScreen)
      windowFlags |= SDL_WINDOW_FULLSCREEN;

    switch (Application::getGraphicsBackend()) {
      case Graphics::Backend::Vulkan:
        windowFlags |= SDL_WINDOW_VULKAN;
        break;

      default:
        break;
    }

    window = SDL_CreateWindow(
      options.windowName,
      static_cast<int>(options.width),
      static_cast<int>(options.height),
      windowFlags
    );

    if (!window) {
      LOG_CORE_ERROR("Failed to create window: {}", SDL_GetError());
      return;
    }

    SDL_SetWindowRelativeMouseMode(window, options.mouseLocked);
  }

  Window::~Window() {
    SDL_DestroyWindow(window);
  }
}
