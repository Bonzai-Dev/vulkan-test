#include <SDL3/SDL_mouse.h>
#include <core/application/application.hpp>
#include <core/application/logger.hpp>
#include "vulkan/render_window.hpp"
#include "render_window.hpp"

namespace Core::Graphics {
  RenderWindow::RenderWindow(const WindowOptions &options) : options(options) {
    SDL_WindowFlags flags = 0;
    if (options.fullScreen)
      flags |= SDL_WINDOW_FULLSCREEN;

    switch (Application::getGraphicsBackend()) {
      case Backend::Vulkan:
        flags |= SDL_WINDOW_VULKAN;
        break;

      default:
        LOG_CORE_ERROR("Unsupported graphics backend.");
    }

    window = SDL_CreateWindow(
      options.windowName,
      options.width, options.height,
      flags
    );

    if (!window) {
      LOG_CORE_ERROR("Failed to create window: {}", SDL_GetError());
      return;
    }

    SDL_SetWindowRelativeMouseMode(window, options.mouseLocked);
  }

  RenderWindow::~RenderWindow() {
    SDL_DestroyWindow(window);
  }
}
