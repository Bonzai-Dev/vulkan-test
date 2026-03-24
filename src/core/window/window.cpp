#include <SDL3/SDL_video.h>
#include <SDL3/SDL_mouse.h>
#include <core/events/window.hpp>
#include "window.hpp"
#include "../application.hpp"

namespace Core {
  Window::Window(
    const WindowOptions &windowOptions,
    const Application &application,
    const SDL_DisplayMode *currentDisplay,
    const Events::EventDispatcher &eventDispatcher
  ) : application(application), eventDispatcher(eventDispatcher), options(windowOptions) {
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
  window(other.window),
  windowFlags(other.windowFlags),
  eventDispatcher(other.eventDispatcher) {
  other.window = nullptr;
  }

  Window::~Window() {
    SDL_DestroyWindow(window);
  }

  void Window::render() const {
    // TODO: rendering logic here
  }

  void Window::onClose(const Events::WindowClosed &event) {
    if (event.windowId == id)
      SDL_HideWindow(window);
  }

  void Window::onExposed(const Events::WindowExposed &event) {

  }

  void Window::onShow(const Events::WindowShown &event) {

  }

  void Window::onHide(const Events::WindowHidden &event) {

  }

  void Window::onResize(const Events::WindowResized &event) {

  }

  void Window::onMouseEnter(const Events::WindowMouseEnter &event) {

  }

  void Window::onMouseLeave(const Events::WindowMouseLeave &event) {

  }

  void Window::onFocusGained(const Events::WindowFocusGained &event) {

  }

  void Window::onFocusLost(const Events::WindowFocusLost &event) {

  }

  void Window::onMinimized(const Events::WindowMinimized &event) {

  }

  void Window::onMaximized(const Events::WindowMaximized &event) {

  }

  void Window::onRestored(const Events::WindowRestored &event) {

  }
}
