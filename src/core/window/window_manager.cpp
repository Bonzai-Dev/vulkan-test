#include <SDL3/SDL_init.h>
#include <core/logger.hpp>
#include <core/events/inputs.hpp>
#include <core/events/application.hpp>
#include <core/events/window.hpp>
#include <core/application.hpp>
#include "window_manager.hpp"

namespace Core {
  WindowManager::WindowManager(const Application &application, const Events::EventDispatcher &eventDispatcher):
  application(application), eventDispatcher(eventDispatcher) {
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
    // TODO: do events here instead of window class
  }

  WindowManager::~WindowManager() {
    SDL_free(displays);
  }

  void WindowManager::createWindow(const WindowOptions& options) {
    windows.emplace_back(options, application, currentDisplay, eventDispatcher);
  }

  void WindowManager::update() const {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
      switch (event.type) {
        case SDL_EVENT_QUIT:
          eventDispatcher.queue(Events::ApplicationQuit());
          break;
        case SDL_EVENT_WINDOW_SHOWN:
          eventDispatcher.queue(Events::WindowShown(event.window.windowID));
          break;
        case SDL_EVENT_WINDOW_HIDDEN:
          eventDispatcher.queue(Events::WindowHidden(event.window.windowID));
          break;
        case SDL_EVENT_WINDOW_RESIZED:
          eventDispatcher.queue(Events::WindowResized(
            event.window.data1, event.window.data2,
            event.window.windowID
          ));
          break;
        case SDL_EVENT_WINDOW_EXPOSED:
          eventDispatcher.queue(Events::WindowExposed(event.window.windowID));
          break;
        case SDL_EVENT_WINDOW_MOUSE_ENTER:
          eventDispatcher.queue(Events::WindowMouseEnter(event.window.windowID));
          break;
        case SDL_EVENT_WINDOW_MOUSE_LEAVE:
          eventDispatcher.queue(Events::WindowMouseLeave(event.window.windowID));
          break;
        case SDL_EVENT_WINDOW_FOCUS_GAINED:
          eventDispatcher.queue(Events::WindowFocusGained(event.window.windowID));
          break;
        case SDL_EVENT_WINDOW_FOCUS_LOST:
          eventDispatcher.queue(Events::WindowFocusLost(event.window.windowID));
          break;
        case SDL_EVENT_WINDOW_MINIMIZED:
          eventDispatcher.queue(Events::WindowMinimized(event.window.windowID));
          break;
        case SDL_EVENT_WINDOW_MAXIMIZED:
          eventDispatcher.queue(Events::WindowMaximized(event.window.windowID));
          break;
        case SDL_EVENT_WINDOW_RESTORED:
          eventDispatcher.queue(Events::WindowRestored(event.window.windowID));
          break;
        case SDL_EVENT_WINDOW_CLOSE_REQUESTED:
          eventDispatcher.queue(Events::WindowClosed(event.window.windowID));
          break;
        case SDL_EVENT_MOUSE_MOTION:
          eventDispatcher.queue(Events::MouseMotion(
            event.motion.xrel, event.motion.yrel,
            event.motion.x, event.motion.y
          ));
        default:
          break;
      }
    }

    for (const Window &window: windows)
      window.render();
  }
}
