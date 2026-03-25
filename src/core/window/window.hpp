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
        const WindowOptions &windowOptions,
        const SDL_DisplayMode *currentDisplay,
        std::uint64_t windowFlags
      );

      Window(const Window &other) = delete;

      Window &operator=(const Window &other) = delete;

      Window(Window &&other) noexcept;

      Window &operator=(Window &&other) noexcept;

      ~Window();

      void render() const;

      void show() { shown = true; }

      void hide() { shown = false; }

      void resize(std::uint32_t width, std::uint32_t height) {
        options.width = width;
        options.height = height;
      }

    private:
      WindowOptions options;

      std::uint32_t id = 0;

      bool shown = false;
      bool mouseFocused = false;
      bool keyboardFocused = false;

      SDL_Window *window = nullptr;
      std::uint64_t windowFlags = 0;
  };
}
