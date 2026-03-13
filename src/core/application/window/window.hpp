#pragma once
#include <cstdint>
#include <SDL3/SDL_video.h>

namespace Core {
  struct WindowOptions {
    bool mouseLocked = false;
    bool fullScreen = false;
    bool vsync = false;

    std::uint32_t width = 800;
    std::uint32_t height = 600;

    const char *windowName = "Window";
  };

  class Window {
    public:
      explicit Window(const WindowOptions& options);

      ~Window();

      const SDL_Window &getWindow() const { return *window; }

    private:
      SDL_Window *window = nullptr;
      SDL_WindowFlags windowFlags = 0;
  };
}