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

      template<typename LayerT>
      requires(std::is_base_of_v<RenderLayer, LayerT>)
      void addLayer() const {
        layers.push_back(std::make_unique<LayerT>(*this));
      }

      bool keyDown(Inputs::KeyboardKey key, Inputs::KeyDetectMode detectMode) const;

      void createWindow(const WindowOptions &options) const { windowManager.createWindow(options); }

      const double &getDeltaTime() const { return deltaTime; }

      const bool &mouseMoving() const { return isMouseMoving; }

      const Graphics::Backend &getGraphicsBackend() const { return renderer.backend; }

      void quit() const;

    private:
      Graphics::Backend chooseGraphicsBackend() const;

      void run() const;

      Logger logger;

      const char *name;
      mutable WindowManager windowManager = WindowManager(*this);
      Graphics::Renderer renderer = Graphics::Renderer(chooseGraphicsBackend(), name);

      mutable std::vector<std::unique_ptr<RenderLayer>> layers;

      mutable std::unordered_map<Inputs::KeyboardKey, SDL_KeyboardEvent> pressedKeys;
      mutable bool isMouseMoving = false;

      mutable double deltaTime = 0;
      mutable bool running = true;
  };
}
