#include <SDL3/SDL_video.h>
#include <SDL3/SDL_mouse.h>
#include "window.hpp"
#include "../application.hpp"

namespace Core {
  Window::Window(
    const WindowOptions &windowOptions,
    const Application &application,
    const SDL_DisplayMode *currentDisplay
  ) : application(application), options(windowOptions) {
    if (windowOptions.fullScreen) {
      windowFlags |= SDL_WINDOW_FULLSCREEN;
      options.width = currentDisplay->w;
      options.height = currentDisplay->h;
    }

    switch (application.getGraphicsBackend()) {
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

    id = SDL_GetWindowID(window);

    SDL_SetWindowRelativeMouseMode(window, options.mouseLocked);
  }

  Window &Window::operator=(Window &&other) noexcept {
    if (&other == this)
      return *this;

    id = other.id;
    mouseFocused = other.mouseFocused;
    keyboardFocused = other.keyboardFocused;
    mouseDelta = other.mouseDelta;
    mousePosition = other.mousePosition;
    window = other.window;
    windowFlags = other.windowFlags;

    other.window = nullptr;
    return *this;
  }

  Window::Window(Window &&other) noexcept :
  application(other.application),
  id(other.id),
  mouseFocused(other.mouseFocused),
  keyboardFocused(other.keyboardFocused),
  mouseDelta(other.mouseDelta),
  mousePosition(other.mousePosition),
  window(other.window),
  windowFlags(other.windowFlags) {
  other.window = nullptr;
  }

  Window::~Window() {
    SDL_DestroyWindow(window);
  }

  void Window::handleEvent(const SDL_Event &event) const {
  }

  void Window::render() const {
  }
}
