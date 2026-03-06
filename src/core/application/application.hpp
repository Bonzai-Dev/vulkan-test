#pragma once
#include <memory>
#include <type_traits>
#include <vector>
#include <glm/gtc/constants.hpp>
#include <glm/glm.hpp>
#include <core/application/logger.hpp>
#include "window.hpp"
#include "render_layer.hpp"
#include "inputs.hpp"

namespace Core {
  class Application {
    public:
      explicit Application(const char *name);

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

      const std::shared_ptr<Window> &getMainWindow() const { return mainWindow; }

      const bool &mouseMoving() const { return isMouseMoving; }

      static const Graphics::Backend &getGraphicsBackend() { return graphicsBackend; }

      void quit() const;

    private:
      void run() const;

      void pollInputs() const;

      Logger logger;

      static inline Graphics::Backend graphicsBackend;

      mutable double deltaTime = 0;
      mutable glm::vec2 mouseDelta = glm::zero<glm::vec2>();
      mutable bool isMouseMoving = false;
      std::shared_ptr<Window> mainWindow;

      int displayCount;
      mutable SDL_DisplayID *displays;
      const SDL_DisplayMode *currentDisplay;

      mutable std::vector<std::unique_ptr<RenderLayer>> layers;
      mutable std::unordered_map<Inputs::KeyboardKey, SDL_KeyboardEvent> pressedKeys;

      mutable double lastFrameTime = 0;
      mutable bool running = false;
  };
}
