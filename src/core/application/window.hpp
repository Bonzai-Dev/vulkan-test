#pragma once
#include <SDL3/SDL_video.h>
#include <core/renderer/render_window.hpp>

namespace Core {
  class Window {
    public:
      Window(
        const char *windowName,
        bool mouseLocked,
        bool fullScreen = false,
        unsigned int width = 800,
        unsigned int height = 600
      );

    private:
      SDL_Window *window = nullptr;

      std::shared_ptr<Graphics::RenderWindow> renderWindow;
  };
}
