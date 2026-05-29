#pragma once
#include <cstdint>

namespace Core::Graphics {
  struct WindowOptions {
    bool mouseLocked = false;
    bool fullScreen = false;
    bool vsync = false;
    bool resizable = true;

    std::uint32_t width = 800;
    std::uint32_t height = 600;

    const char *windowName = "Window";
  };

  struct DisplayInfo {
    std::uint32_t displayId;

    std::uint32_t width;
    std::uint32_t height;
  };

  enum class Backend {
    None,
    Vulkan
  };
}