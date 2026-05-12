#pragma once

namespace Core::Graphics {
  enum class Backend {
    None,
    Vulkan
  };

  class RenderingDevice {
    public:
      RenderingDevice() = default;

      virtual ~RenderingDevice() = default;

      virtual void createWindow();
  };
}
