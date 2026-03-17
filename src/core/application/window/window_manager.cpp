#include <SDL3/SDL_init.h>
#include <core/application/logger.hpp>
#include "window_manager.hpp"

#include "core/application/application.hpp"

namespace Core {
  WindowManager::WindowManager(const Application &application): application(application) {
    if (!SDL_Init(SDL_INIT_VIDEO)) {
      LOG_CORE_CRITICAL("Failed to initialize SDL Video: {}", SDL_GetError());
      return;
    }

    displays = SDL_GetDisplays(&displayCount);
    if (displayCount > 0)
      LOG_CORE_INFO("Found {} display(s).", displayCount);
    else
      LOG_CORE_WARNING("No displays found.");

    currentDisplay = SDL_GetCurrentDisplayMode(displays[0]);
  }

  WindowManager::~WindowManager() {
    SDL_free(displays);
  }

  void WindowManager::createWindow(const WindowOptions& options) {
    windows.emplace_back(options, application, currentDisplay);
  }

  void WindowManager::update() const {
    for (const auto &window: windows) {
      // window.handleEvent(TODO);
      window.render();
    }
  }

  void WindowManager::pollInputs() const {
    SDL_Event sdlEvent;
    while (SDL_PollEvent(&sdlEvent)) {
      switch (sdlEvent.type) {
        case SDL_EVENT_MOUSE_MOTION:
          break;
        default:
          break;
      }
    }
  }
}
