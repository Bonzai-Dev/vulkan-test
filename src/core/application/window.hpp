#pragma once
#include <SDL3/SDL_video.h>

namespace Core {
  namespace Graphics {
    class VulkanRenderContext;
  }

  struct WindowOptions {
    bool mouseLocked = false;
    bool fullScreen = false;
    bool vsync = false;

    std::uint32_t width = 800;
    std::uint32_t height = 600;

    const char *windowName = "";
  };

  class Window {
    public:
      Window(const WindowOptions &options);

      const WindowOptions &options;

    private:
      SDL_Window *window = nullptr;

      std::shared_ptr<Graphics::VulkanRenderContext> renderWindow;
  };
}
