#pragma once
#include <memory>
#include <SDL3/SDL_video.h>

// This class abstracts the rendering of windows for each graphics api.
//
// Unlike the Window class which contains the RenderWindow class, this class has low level control
// over the low level rendering of the window and is used by the Window class to render the window.

namespace Core {
  struct WindowOptions;
}

namespace Core::Graphics {
  enum class Backend {
    None,
    Vulkan
  };

  class RenderWindow {
    public:
      RenderWindow(const WindowOptions &options);

      ~RenderWindow();

    protected:
      const WindowOptions &options;
      SDL_Window *window = nullptr;
  };
}
