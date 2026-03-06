#pragma once
#include <core/renderer/render_window.hpp>
#include "volk.h"
#include "vulkan.hpp"

namespace Core::Graphics {
  class VulkanRenderWindow : public RenderWindow {
    public:
      VulkanRenderWindow(
        const char *windowName,
        bool mouseLocked,
        bool fullScreen,
        unsigned int width,
        unsigned int height
      );

    private:
      VkSurfaceKHR surface;
      Vulkan vulkan = Vulkan("Vulkan test application");
  };
}
