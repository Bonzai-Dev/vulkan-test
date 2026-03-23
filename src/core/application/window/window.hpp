#pragma once
#include <cstdint>
#include <memory>
#include <SDL3/SDL_video.h>
#include <glm/gtc/constants.hpp>
#include <glm/glm.hpp>
#include "layer.hpp"
#include "SDL3/SDL_events.h"

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

      const glm::vec2 &getMouseDelta() const { return mouseDelta; }

      void handleWindowEvent(const SDL_Event &event) const;

      void update(const SDL_Event &event) const;

    private:
      void render() const;

      const Application &application;
      const Events::EventDispatcher &eventDispatcher;

      WindowOptions options;

      unsigned int id = 0;

      bool mouseFocused = false;
      bool keyboardFocused = false;

      mutable glm::vec2 mouseDelta = glm::zero<glm::vec2>();
      mutable glm::vec2 mousePosition = glm::zero<glm::vec2>();

      SDL_Window *window = nullptr;
      SDL_WindowFlags windowFlags = 0;
  };
}
