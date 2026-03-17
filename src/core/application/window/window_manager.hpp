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
      void pollInputs() const;

      const Application &application;
      std::vector<Window> windows;

      int displayCount = 0;
      mutable SDL_DisplayID *displays;
      const SDL_DisplayMode *currentDisplay;
  };
}
