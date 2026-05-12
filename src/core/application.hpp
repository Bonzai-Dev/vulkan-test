#pragma once
#include <memory>
#include <core/logger.hpp>
#include <core/events/application.hpp>
#include "renderer/rendering_device.hpp"
#include "renderer/window.hpp"
#include "window/window_manager.hpp"
#include "window/layer.hpp"

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

      void createWindow(const Graphics::WindowOptions &options);

      void update() const;

      const double &getDeltaTime() const { return deltaTime; }

      void quit() const;

      void run() const;

      const Graphics::Backend graphicsBackend = selectGraphicsBackend();

    private:
      void onWindowShow(const Events::WindowShown &event);

      void onWindowHide(const Events::WindowHidden &event);

      void onWindowResize(const Events::WindowResized &event);

      void onWindowMouseEnter(const Events::WindowMouseEnter &event);

      void onWindowMouseMotion(const Events::WindowMouseMotion &event);

      void onWindowMouseLeave(const Events::WindowMouseLeave &event);

      void onWindowFocusGained(const Events::WindowFocusGained &event);

      void onWindowFocusLost(const Events::WindowFocusLost &event);

      void onWindowMinimized(const Events::WindowMinimized &event);

      void onWindowMaximized(const Events::WindowMaximized &event);

      void onWindowRestored(const Events::WindowRestored &event);

      void onWindowClose(const Events::WindowClosed &event);

      void onWindowExposed(const Events::WindowExposed &event);

      void onKeyPressed(const Events::KeyPressedEvent &event);

      void onKeyReleased(const Events::KeyReleasedEvent &event);

      void pollInputs() const;

      Graphics::Backend selectGraphicsBackend() const;
      std::shared_ptr<Graphics::RenderingDevice> renderingDevice;

      Logger logger;

      const char *name;
      mutable Events::EventDispatcher eventDispatcher;
      mutable WindowManager windowManager = WindowManager(*this);

      mutable std::vector<std::unique_ptr<Layer>> layers;

      mutable double deltaTime = 0;
      mutable bool running = true;

      Graphics::DisplayInfo displayInfo;
      std::unordered_map<std::uint32_t, Graphics::Window> windows;

      int displayCount = 0;
      mutable SDL_DisplayID *displays;
      const SDL_DisplayMode *currentDisplay;
  };
}
