#pragma once
#include <memory>
#include <type_traits>
#include <vector>
#include <glm/gtc/constants.hpp>
#include <glm/glm.hpp>
#include <core/application/logger.hpp>
#include <core/renderer/renderer.hpp>
#include "window/render_layer.hpp"
#include "window/inputs.hpp"
#include "window/window.hpp"

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

      template<typename LayerT>
      requires(std::is_base_of_v<RenderLayer, LayerT>)
      void addLayer() const {
        layers.push_back(std::make_unique<LayerT>(*this));
      }

      const double &getDeltaTime() const { return deltaTime; }

      const glm::vec2 &getMouseDelta() const { return mouseDelta; }

      const std::shared_ptr<Window> &getMainWindow() const { return currentWindow; }

      const bool &mouseMoving() const { return isMouseMoving; }

      static const Graphics::Backend &getGraphicsBackend() { return renderer->backend; }

      void quit() const;

    private:
      Graphics::Backend chooseGraphicsBackend() const;

      void run() const;

      void pollInputs() const;

      Logger logger;

      static inline std::shared_ptr<Graphics::Renderer> renderer;

      mutable double deltaTime = 0;
      mutable glm::vec2 mouseDelta = glm::zero<glm::vec2>();
      mutable bool isMouseMoving = false;
      std::shared_ptr<Window> currentWindow;

      int displayCount = 0;
      mutable SDL_DisplayID *displays;
      const SDL_DisplayMode *currentDisplay;

      mutable std::vector<std::unique_ptr<RenderLayer>> layers;
      mutable std::unordered_map<Inputs::KeyboardKey, SDL_KeyboardEvent> pressedKeys;

      mutable double lastFrameTime = 0;
      mutable bool running = true;
  };
}
