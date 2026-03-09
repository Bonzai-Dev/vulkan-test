#include <core/application/logger.hpp>
#include <core/renderer/vulkan/render_window.hpp>
#include "application.hpp"
#include "window.hpp"

namespace Core {
  Window::Window(
    const char *windowName,
    bool mouseLocked,
    bool fullScreen,
    unsigned int width,
    unsigned int height
  ) {
    switch (Application::getGraphicsBackend()) {
      case Graphics::Backend::Vulkan: {
        LOG_CORE_INFO("Rendering application with Vulkan.");
        WindowOptions options{
          .mouseLocked = mouseLocked,
          .fullScreen = fullScreen,
          .width = width,
          .height = height,
          .windowName = windowName
        };
        renderWindow = std::make_shared<Graphics::VulkanRenderWindow>(options);
        break;
      }

      default:
        LOG_CORE_ERROR("Unsupported graphics backend.");
    }
  }
}
