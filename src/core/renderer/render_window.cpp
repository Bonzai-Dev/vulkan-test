#include <SDL3/SDL_mouse.h>
#include <core/application/application.hpp>
#include <core/application/logger.hpp>
#include "vulkan/render_window.hpp"
#include "render_window.hpp"

namespace Core::Graphics {
  RenderWindow::RenderWindow(
    const char *windowName,
    bool mouseLocked,
    bool fullScreen,
    unsigned int width,
    unsigned int height
  ) : width(width), height(height) {
    SDL_WindowFlags flags = 0;
    if (fullScreen)
      flags |= SDL_WINDOW_FULLSCREEN;
    switch (Application::getGraphicsBackend()) {
      case Backend::Vulkan:
        flags |= SDL_WINDOW_VULKAN;
        break;

      default:
        LOG_CORE_ERROR("Unsupported graphics backend.");
    }

    window = SDL_CreateWindow(
      windowName,
      width, height,
      flags
    );

    if (!window) {
      LOG_CORE_ERROR("Failed to create window: {}", SDL_GetError());
      return;
    }

    SDL_SetWindowRelativeMouseMode(window, mouseLocked);
  }

  RenderWindow::~RenderWindow() {
    SDL_DestroyWindow(window);
  }
}
