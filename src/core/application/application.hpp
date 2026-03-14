#pragma once
#include <memory>
#include <core/application/logger.hpp>
#include <core/renderer/renderer.hpp>
#include "window/inputs.hpp"
#include "window/window.hpp"
#include "window/window_manager.hpp"

namespace Core {
  class Application {
    public:
      explicit Application(const char *name);

      Application(const Application &other) = delete;

      Application &operator=(const Application &other) = delete;

      Application(Application &&other) = delete;

      Application &operator=(Application &&other) = delete;

      ~Application();

#ifdef NDEBUG
      static constexpr bool debugEnabled = false;
#else
      static constexpr bool debugEnabled = true;
#endif

      bool keyDown(Inputs::KeyboardKey key, Inputs::KeyDetectMode detectMode) const;

      const double &getDeltaTime() const { return deltaTime; }

      const bool &mouseMoving() const { return isMouseMoving; }

      static const Graphics::Backend &getGraphicsBackend() { return renderer->backend; }

      void quit() const;

    private:
      Graphics::Backend chooseGraphicsBackend() const;

      void run() const;

      Logger logger;

      static inline std::shared_ptr<Graphics::Renderer> renderer;

      WindowManager windowManager;

      mutable bool isMouseMoving = false;

      int displayCount = 0;
      mutable SDL_DisplayID *displays;
      const SDL_DisplayMode *currentDisplay;

      mutable std::unordered_map<Inputs::KeyboardKey, SDL_KeyboardEvent> pressedKeys;

      mutable double deltaTime = 0;
      mutable bool running = true;
  };
}
