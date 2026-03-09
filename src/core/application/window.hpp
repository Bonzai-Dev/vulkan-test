#pragma once
#include <SDL3/SDL_video.h>
#include <core/renderer/render_window.hpp>

namespace Core {
  struct WindowOptions {
    bool mouseLocked = false;
    bool fullScreen = false;
    bool vsync = false;

    std::uint32_t width = 800;
    std::uint32_t height = 600;

    const char *windowName;
  };

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
