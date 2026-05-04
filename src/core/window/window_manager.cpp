#include <SDL3/SDL_init.h>
#include <core/logger.hpp>
#include <core/events/input.hpp>
#include <core/events/application.hpp>
#include <core/events/window.hpp>
#include <core/application.hpp>
#include "window_manager.hpp"

using namespace Core::Events;

namespace Core {
  WindowManager::WindowManager(const Application &application) : application(application) {
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

    EventDispatcher::listen<WindowShown>([&](const WindowShown &event) { onWindowShow(event); });
    EventDispatcher::listen<WindowHidden>([&](const WindowHidden &event) { onWindowHide(event); });
    EventDispatcher::listen<WindowResized>([&](const WindowResized &event) { onWindowResize(event); });

    EventDispatcher::listen<WindowMouseEnter>([&](const WindowMouseEnter &event) { onWindowMouseEnter(event); });
    EventDispatcher::listen<WindowMouseLeave>([&](const WindowMouseLeave &event) { onWindowMouseLeave(event); });
    EventDispatcher::listen<WindowMouseMotion>([&](const WindowMouseMotion &event) { onWindowMouseMotion(event); });

    EventDispatcher::listen<WindowFocusGained>([&](const WindowFocusGained &event) { onWindowFocusGained(event); });
    EventDispatcher::listen<WindowFocusLost>([&](const WindowFocusLost &event) { onWindowFocusLost(event); });
    EventDispatcher::listen<WindowMinimized>([&](const WindowMinimized &event) { onWindowMinimized(event); });
    EventDispatcher::listen<WindowMaximized>([&](const WindowMaximized &event) { onWindowMaximized(event); });
    EventDispatcher::listen<WindowRestored>([&](const WindowRestored &event) { onWindowRestored(event); });
    EventDispatcher::listen<WindowClosed>([&](const WindowClosed &event) { onWindowClose(event); });
    EventDispatcher::listen<WindowExposed>([&](const WindowExposed &event) { onWindowExposed(event); });

    EventDispatcher::listen<KeyPressedEvent>([&](const KeyPressedEvent &event) { onKeyPressed(event); });
    EventDispatcher::listen<KeyReleasedEvent>([&](const KeyReleasedEvent &event) { onKeyReleased(event); });
  }

  WindowManager::~WindowManager() {
    // TODO: remove listeners?
    SDL_free(displays);
  }

  void WindowManager::createWindow(const WindowOptions &options) {
    std::uint64_t windowFlags = 0;

    switch (application.graphicsBackend) {
      case Graphics::Backend::Vulkan:
        windowFlags |= SDL_WINDOW_VULKAN;
        break;
      default:
        break;
    }

    std::uint64_t width = options.width;
    std::uint64_t height = options.height;
    if (options.fullScreen) {
      windowFlags |= SDL_WINDOW_FULLSCREEN;
      width = currentDisplay->w;
      height = currentDisplay->h;
    }

    if (options.resizable)
      windowFlags |= SDL_WINDOW_RESIZABLE;

    SDL_Window *window = SDL_CreateWindow(
      options.windowName,
      static_cast<int>(width), static_cast<int>(height),
      windowFlags
    );

    if (!window) {
      LOG_CORE_ERROR("Failed to create window: {}", SDL_GetError());
      return;
    }

    windows.emplace(SDL_GetWindowID(window), Window(options, windowFlags));
  }

  void WindowManager::update() const {
    pollInputs();
    for (auto const &[windowId, window]: windows)
      window.render();
  }

  void WindowManager::pollInputs() const {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
      switch (event.type) {
        case SDL_EVENT_QUIT:
          EventDispatcher::queue<ApplicationQuit>(ApplicationQuit());
          break;
        case SDL_EVENT_WINDOW_SHOWN:
          EventDispatcher::queue(WindowShown(event.window.windowID));
          break;
        case SDL_EVENT_WINDOW_HIDDEN:
          EventDispatcher::queue(WindowHidden(event.window.windowID));
          break;
        case SDL_EVENT_WINDOW_RESIZED:
          EventDispatcher::queue(WindowResized(
            event.window.data1, event.window.data2,
            event.window.windowID
          ));
          break;
        case SDL_EVENT_WINDOW_EXPOSED:
          EventDispatcher::queue(WindowExposed(event.window.windowID));
          break;
        case SDL_EVENT_WINDOW_MOUSE_ENTER:
          EventDispatcher::queue(WindowMouseEnter(event.window.windowID));
          break;
        case SDL_EVENT_WINDOW_MOUSE_LEAVE:
          EventDispatcher::queue(WindowMouseLeave(event.window.windowID));
          break;
        case SDL_EVENT_WINDOW_FOCUS_GAINED:
          EventDispatcher::queue(WindowFocusGained(event.window.windowID));
          break;
        case SDL_EVENT_WINDOW_FOCUS_LOST:
          EventDispatcher::queue(WindowFocusLost(event.window.windowID));
          break;
        case SDL_EVENT_WINDOW_MINIMIZED:
          EventDispatcher::queue(WindowMinimized(event.window.windowID));
          break;
        case SDL_EVENT_WINDOW_MAXIMIZED:
          EventDispatcher::queue(WindowMaximized(event.window.windowID));
          break;
        case SDL_EVENT_WINDOW_RESTORED:
          EventDispatcher::queue(WindowRestored(event.window.windowID));
          break;
        case SDL_EVENT_WINDOW_CLOSE_REQUESTED:
          EventDispatcher::queue(WindowClosed(event.window.windowID));
          break;
        case SDL_EVENT_MOUSE_MOTION:
          EventDispatcher::queue(WindowMouseMotion(
            event.motion.xrel, event.motion.yrel,
            event.motion.x, event.motion.y,
            event.motion.windowID
          ));
          break;
        case SDL_EVENT_KEY_DOWN:
          // TODO: Take into account scancodes
          // https://wiki.libsdl.org/SDL3/BestKeyboardPractices
          EventDispatcher::queue(KeyPressedEvent(event.key.key, event.key.repeat, event.key.windowID));
          break;
        case SDL_EVENT_KEY_UP:
          EventDispatcher::queue(KeyReleasedEvent(event.key.key, event.key.windowID));
          break;
        default:
          break;
      }
    }
  }

  void WindowManager::onWindowShow(const WindowShown &event) {
    windows.at(event.windowId).show();
  }

  void WindowManager::onWindowHide(const WindowHidden &event) {
    windows.at(event.windowId).hide();
  }

  void WindowManager::onWindowResize(const WindowResized &event) {
    windows.at(event.windowId).resize(event.width, event.height);
  }

  void WindowManager::onWindowMouseMotion(const WindowMouseMotion &event) {
  }

  void WindowManager::onWindowMouseEnter(const WindowMouseEnter &event) {
  }

  void WindowManager::onWindowMouseLeave(const WindowMouseLeave &event) {
  }

  void WindowManager::onWindowFocusGained(const WindowFocusGained &event) {
  }

  void WindowManager::onWindowFocusLost(const WindowFocusLost &event) {
  }

  void WindowManager::onWindowMinimized(const WindowMinimized &event) {
  }

  void WindowManager::onWindowMaximized(const WindowMaximized &event) {
  }

  void WindowManager::onWindowRestored(const WindowRestored &event) {
  }

  void WindowManager::onWindowClose(const WindowClosed &event) {
  }

  void WindowManager::onWindowExposed(const WindowExposed &event) {
  }

  void WindowManager::onKeyPressed(const KeyPressedEvent &event) {

  }

  void WindowManager::onKeyReleased(const KeyReleasedEvent &event) {
  }
}
