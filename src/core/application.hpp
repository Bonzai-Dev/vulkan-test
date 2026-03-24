#pragma once
#include <memory>
#include <core/logger.hpp>
#include <core/renderer/renderer.hpp>
#include <core/events/application.hpp>
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
      requires(std::is_base_of_v<Layer, LayerT>)
      void addLayer() const {
        layers.push_back(std::make_unique<LayerT>(*this));
      }

      void createWindow(const WindowOptions &options) const { windowManager.createWindow(options); }

      const double &getDeltaTime() const { return deltaTime; }

      const Graphics::Backend &getGraphicsBackend() const { return renderer.backend; }

      void quit() const;

      void run() const;

    private:
      Graphics::Backend selectGraphicsBackend() const;

      Logger logger;

      const char *name;
      mutable Events::EventDispatcher eventDispatcher;
      mutable WindowManager windowManager = WindowManager(*this, eventDispatcher);
      Graphics::Renderer renderer = Graphics::Renderer(selectGraphicsBackend(), name);

      mutable std::vector<std::unique_ptr<Layer>> layers;

      mutable double deltaTime = 0;
      mutable bool running = true;
  };
}
