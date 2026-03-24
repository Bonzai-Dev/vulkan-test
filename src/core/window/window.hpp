#pragma once
#include <cstdint>
#include <SDL3/SDL_video.h>
#include <core/events/window.hpp>
#include "layer.hpp"

namespace Core {
  struct WindowOptions {
    bool mouseLocked = false;
    bool fullScreen = false;
    bool vsync = false;
    bool minimized = false;

    std::uint32_t width = 800;
    std::uint32_t height = 600;

    const char *windowName = "Window";
  };

  class Window {
    public:
      explicit Window(
        const WindowOptions& windowOptions,
        const Application &application,
        const SDL_DisplayMode *currentDisplay,
        const Events::EventDispatcher &eventDispatcher
      );

      Window(const Window &other) = delete;

      Window &operator=(const Window &other) = delete;

      Window(Window &&other) noexcept;

      Window &operator=(Window &&other) noexcept;

      ~Window();

      const SDL_Window &getWindow() const { return *window; }

      void render() const;

    private:
     void onShow(const Events::WindowShown &event);

     void onHide(const Events::WindowHidden &event);

     void onResize(const Events::WindowResized &event);

     void onMouseEnter(const Events::WindowMouseEnter &event);

     void onMouseLeave(const Events::WindowMouseLeave &event);

     void onFocusGained(const Events::WindowFocusGained &event);

     void onFocusLost(const Events::WindowFocusLost &event);

     void onMinimized(const Events::WindowMinimized &event);

     void onMaximized(const Events::WindowMaximized &event);

     void onRestored(const Events::WindowRestored &event);

     void onClose(const Events::WindowClosed &event);

      void onExposed(const Events::WindowExposed &event);

      const Application &application;
      const Events::EventDispatcher &eventDispatcher;

      WindowOptions options;

      unsigned int id = 0;

      bool mouseFocused = false;
      bool keyboardFocused = false;

      SDL_Window *window = nullptr;
      SDL_WindowFlags windowFlags = 0;
  };
}
