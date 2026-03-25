#include <SDL3/SDL_video.h>
#include <SDL3/SDL_mouse.h>
#include <core/events/window.hpp>
#include "window.hpp"
#include "../application.hpp"

namespace Core {
  Window::Window(
    const WindowOptions &windowOptions,
    const SDL_DisplayMode *currentDisplay,
    std::uint64_t windowFlags
  ) : options(windowOptions), windowFlags(windowFlags) {
    if (windowOptions.fullScreen) {
      windowFlags |= SDL_WINDOW_FULLSCREEN;
      options.width = currentDisplay->w;
      options.height = currentDisplay->h;
    }

    SDL_SetWindowRelativeMouseMode(window, options.mouseLocked);
  }

  Window &Window::operator=(Window &&other) noexcept {
    if (&other == this)
      return *this;

    id = other.id;
    mouseFocused = other.mouseFocused;
    keyboardFocused = other.keyboardFocused;
    window = other.window;
    windowFlags = other.windowFlags;

    other.window = nullptr;
    return *this;
  }

  Window::Window(Window &&other) noexcept :
  id(other.id),
  mouseFocused(other.mouseFocused),
  keyboardFocused(other.keyboardFocused),
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
