#include <core/application/logger.hpp>
#include "vulkan/render_context.hpp"
#include "renderer.hpp"

namespace Core::Graphics {
  Renderer::Renderer(Backend backend, const char *appName): backend(backend) {
    LOG_CORE_CRITICAL(appName);

    switch (backend) {
      case Backend::Vulkan:
        renderContext = std::make_unique<VulkanRenderContext>(appName);
        break;

      case Backend::None:
        LOG_CORE_CRITICAL("No graphics backend selected. Select a backend to render graphics.");
        break;

      default:
        LOG_CORE_CRITICAL("Unsupported graphics backend selected.");
        break;
    }
  }
}
