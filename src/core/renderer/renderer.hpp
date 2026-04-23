#pragma once
#include <memory>
#include "render_context.hpp"

namespace Core::Graphics {
  enum class Backend {
    None,
    Vulkan
  };

  class Renderer {
    public:
      Renderer(Backend backend, const char *appName);

      const Backend backend;

    private:
      std::shared_ptr<RenderContext> renderContext;
  };
}
