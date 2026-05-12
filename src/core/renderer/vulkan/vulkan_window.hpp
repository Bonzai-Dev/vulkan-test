#pragma once

#include "../window.hpp"
#include "vulkan_swapchain.hpp"

namespace Core::Graphics {
  class VulkanWindow: public Window {
    public:
      VulkanWindow(
       const VulkanDevice &device,
       const VkInstance &instance,
       const DisplayInfo &displayInfo,
       const WindowOptions &windowOptions
      );

    private:
      VulkanSwapChain swapChain;
  };
}
