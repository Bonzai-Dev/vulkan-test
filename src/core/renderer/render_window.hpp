#pragma once
#include <memory>
#include <SDL3/SDL_video.h>

// This class abstracts the rendering of windows for each graphics api.
//
// Unlike the Window class which contains the RenderWindow class, this class has low level control
// over the low level rendering of the window and is used by the Window class to render the window.

namespace Core::Graphics {
  enum class Backend {
    None,
    Vulkan
  };

  class RenderWindow {
    public:
      RenderWindow(
        const char *windowName,
        bool mouseLocked,
        bool fullScreen,
        unsigned int width,
        unsigned int height
      );

      ~RenderWindow();

    protected:
      int width = 0;
      int height = 0;
      SDL_Window *window = nullptr;
  };
}
