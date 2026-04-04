#include <SDL3/SDL_init.h>
#include <core/logger.hpp>
#include <core/events/inputs.hpp>
#include <core/events/application.hpp>
#include <core/events/window.hpp>
#include <core/application.hpp>
#include "window_manager.hpp"

namespace Core {
  WindowManager::WindowManager(const Application &application):
  application(application) {
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

    Events::EventListener<Events::WindowShown>([&](const Events::WindowShown &event) { onWindowShow(event); });
    Events::EventListener<Events::WindowHidden>([&](const Events::WindowHidden &event) { onWindowHide(event); });
    Events::EventListener<Events::WindowResized>([&](const Events::WindowResized &event) { onWindowResize(event); });
    Events::EventListener<Events::WindowMouseEnter>([&](const Events::WindowMouseEnter &event) { onWindowMouseEnter(event); });
    Events::EventListener<Events::WindowMouseLeave>([&](const Events::WindowMouseLeave &event) { onWindowMouseLeave(event); });
    Events::EventListener<Events::WindowFocusGained>([&](const Events::WindowFocusGained &event) { onWindowFocusGained(event); });
    Events::EventListener<Events::WindowFocusLost>([&](const Events::WindowFocusLost &event) { onWindowFocusLost(event); });
    Events::EventListener<Events::WindowMinimized>([&](const Events::WindowMinimized &event) { onWindowMinimized(event); });
    Events::EventListener<Events::WindowMaximized>([&](const Events::WindowMaximized &event) { onWindowMaximized(event); });
    Events::EventListener<Events::WindowRestored>([&](const Events::WindowRestored &event) { onWindowRestored(event); });
    Events::EventListener<Events::WindowClosed>([&](const Events::WindowClosed &event) { onWindowClose(event); });
    Events::EventListener<Events::WindowExposed>([&](const Events::WindowExposed &event) { onWindowExposed(event); });
  }

  WindowManager::~WindowManager() {
    SDL_free(displays);
  }

  void WindowManager::createWindow(const WindowOptions& options) {
    std::uint64_t windowFlags = 0;

    switch (application.getGraphicsBackend()) {
      case Graphics::Backend::Vulkan:
        windowFlags |= SDL_WINDOW_VULKAN;
        break;
      default:
        break;
    }

    SDL_Window *window = SDL_CreateWindow(
      options.windowName,
      static_cast<int>(options.width),
      static_cast<int>(options.height),
      windowFlags
    );

    if (!window) {
      LOG_CORE_ERROR("Failed to create window: {}", SDL_GetError());
      return;
    }

    windows.emplace(SDL_GetWindowID(window), Window(options, currentDisplay, windowFlags));
  }

  void WindowManager::update() const {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
      switch (event.type) {
        case SDL_EVENT_QUIT:
          Events::EventDispatcher(Events::ApplicationQuit{});
          break;
        case SDL_EVENT_WINDOW_SHOWN:
          // Events::EventDispatcher(Events::WindowShown(event.window.windowID));
          break;
        case SDL_EVENT_WINDOW_HIDDEN:
          // eventDispatcher.queue(Events::WindowHidden(event.window.windowID));
          break;
        case SDL_EVENT_WINDOW_RESIZED:
          // eventDispatcher.queue(Events::WindowResized(
            // event.window.data1, event.window.data2,
            // event.window.windowID
          // ));
          break;
        case SDL_EVENT_WINDOW_EXPOSED:
          // eventDispatcher.queue(Events::WindowExposed(event.window.windowID));
          break;
        case SDL_EVENT_WINDOW_MOUSE_ENTER:
          // eventDispatcher.queue(Events::WindowMouseEnter(event.window.windowID));
          break;
        case SDL_EVENT_WINDOW_MOUSE_LEAVE:
          // eventDispatcher.queue(Events::WindowMouseLeave(event.window.windowID));
          break;
        case SDL_EVENT_WINDOW_FOCUS_GAINED:
          // eventDispatcher.queue(Events::WindowFocusGained(event.window.windowID));
          break;
        case SDL_EVENT_WINDOW_FOCUS_LOST:
          // eventDispatcher.queue(Events::WindowFocusLost(event.window.windowID));
          break;
        case SDL_EVENT_WINDOW_MINIMIZED:
          // eventDispatcher.queue(Events::WindowMinimized(event.window.windowID));
          break;
        case SDL_EVENT_WINDOW_MAXIMIZED:
          // eventDispatcher.queue(Events::WindowMaximized(event.window.windowID));
          break;
        case SDL_EVENT_WINDOW_RESTORED:
          // eventDispatcher.queue(Events::WindowRestored(event.window.windowID));
          break;
        case SDL_EVENT_WINDOW_CLOSE_REQUESTED:
          // eventDispatcher.queue(Events::WindowClosed(event.window.windowID));
          break;
        case SDL_EVENT_MOUSE_MOTION:
          // eventDispatcher.queue(Events::MouseMotion(
          //   event.motion.xrel, event.motion.yrel,
          //   event.motion.x, event.motion.y
          // ));
        default:
          break;
      }
    }

    for (auto const &[windowId, window]: windows)
      window.render();
  }

  void WindowManager::onWindowShow(const Events::WindowShown &event) {
    LOG_CORE_INFO(event.windowId);
    // windows.at(event.windowId).show();
  }

  void WindowManager::onWindowHide(const Events::WindowHidden &event) {
    windows.at(event.windowId).hide();
  }

  void WindowManager::onWindowResize(const Events::WindowResized &event) {
    windows.at(event.windowId).resize(event.width, event.height);
  }

  void WindowManager::onWindowMouseEnter(const Events::WindowMouseEnter &event) {

  }

  void WindowManager::onWindowMouseLeave(const Events::WindowMouseLeave &event) {

  }

  void WindowManager::onWindowFocusGained(const Events::WindowFocusGained &event) {

  }

  void WindowManager::onWindowFocusLost(const Events::WindowFocusLost &event) {

  }

  void WindowManager::onWindowMinimized(const Events::WindowMinimized &event) {

  }

  void WindowManager::onWindowMaximized(const Events::WindowMaximized &event) {

  }

  void WindowManager::onWindowRestored(const Events::WindowRestored &event) {

  }

  void WindowManager::onWindowClose(const Events::WindowClosed &event) {

  }

  void WindowManager::onWindowExposed(const Events::WindowExposed &event) {

  }
}
