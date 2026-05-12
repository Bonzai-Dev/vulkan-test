#pragma once

#include "../window.hpp"
#include "vulkan_swapchain.hpp"

namespace Core::Graphics {
  class VulkanWindow: public Window {
    public:
      VulkanWindow(
        const DisplayInfo &displayInfo,
        const WindowOptions &windowOptions,
        std::uint64_t windowFlags
      );

    private:
      // VulkanSwapChain swapChain;
  };
}
