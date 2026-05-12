#pragma once
#include <core/events/input.hpp>
#include <core/events/window.hpp>
#include <core/renderer/window.hpp>

namespace Core {
  class Application;

  class WindowManager {
    public:
      explicit WindowManager(const Application &application);

      ~WindowManager();

      WindowManager(const WindowManager&) = delete;

      WindowManager& operator=(const WindowManager&) = delete;

      WindowManager(WindowManager&&) = delete;

      WindowManager& operator=(WindowManager&&) = delete;

      void createWindow(const Graphics::WindowOptions& options);

      void update() const;

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

      const Application &application;
      // const Graphics::RenderingDevice &renderingDevice;

      Graphics::DisplayInfo displayInfo;
      std::unordered_map<std::uint32_t, Graphics::Window> windows;

      int displayCount = 0;
      mutable SDL_DisplayID *displays;
      const SDL_DisplayMode *currentDisplay;
  };
}
