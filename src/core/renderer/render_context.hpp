#pragma once

namespace Core::Graphics {
  enum class Backend {
    None,
    Vulkan
  };

  class RenderContext {
    public:
      RenderContext(Backend backend);

      ~RenderContext() = default;

      const Backend backend;

    private:

  };
}
