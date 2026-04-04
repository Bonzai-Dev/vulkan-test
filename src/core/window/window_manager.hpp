#pragma once
#include <memory>
#include <vector>
#include "window.hpp"

namespace Core {
  class WindowManager {
    public:
      explicit WindowManager(const Application &application);

      ~WindowManager();

      WindowManager(const WindowManager&) = delete;

      WindowManager& operator=(const WindowManager&) = delete;

      WindowManager(WindowManager&&) = delete;

      WindowManager& operator=(WindowManager&&) = delete;

      void createWindow(const WindowOptions& options);

      void update() const;

    private:
      void onWindowShow(const Events::WindowShown &event);

      void onWindowHide(const Events::WindowHidden &event);

      void onWindowResize(const Events::WindowResized &event);

      void onWindowMouseEnter(const Events::WindowMouseEnter &event);

      void onWindowMouseMotion(const Events::WindowMouseMotion &event);

      void onWindowMouseLeave(const Events::WindowMouseLeave &event);

      void onWindowFocusGained(const Events::WindowFocusGained &event);

      void onWindowFocusLost(const Events::WindowFocusLost &event);

      void onWindowMinimized(const Events::WindowMinimized &event);

      void onWindowMaximized(const Events::WindowMaximized &event);

      void onWindowRestored(const Events::WindowRestored &event);

      void onWindowClose(const Events::WindowClosed &event);

      void onWindowExposed(const Events::WindowExposed &event);

      const Application &application;
      std::unordered_map<std::uint32_t, Window> windows;

      int displayCount = 0;
      mutable SDL_DisplayID *displays;
      const SDL_DisplayMode *currentDisplay;
  };
}
