#pragma once
#include "../window.hpp"
#include <core/renderer/pixel_format.hpp>
#include "vulkan_device.hpp"

namespace Core::Graphics {
  class VulkanWindow: public Window {
    enum SwapChainStatus {
      /// We already called VulkanWindow::acquireNextImage.
      ///
      /// Can only go into this state if we're coming from SwapchainReleased
      SwapChainAcquired,
      /// We already called VulkanWindow::getImageAcquiredSemaphore.
      /// Further calls to getImageAcquiredSemaphore will return null.
      /// It is rendering or intends to into this swapchain.
      ///
      /// Can only go into this state if we're coming from SwapchainAcquired
      SwapChainUsedInRendering,
      /// We've come from SwapchainUsedInRendering and are waiting for
      /// VulkanDevice::commitAndNextCommandBuffer to present us
      SwapChainPendingSwap,
      /// We don't own a swapchain. Cannot render to this window.
      ///
      /// This status should not last long unless we're not initialized yet.
      SwapChainReleased
    };
    
    public:
      VulkanWindow(
       const VulkanDevice &device,
       const VkInstance &instance,
       const DisplayInfo &displayInfo,
       const WindowOptions &windowOptions
      );

      VulkanWindow(const VulkanWindow &other) = delete;

      VulkanWindow &operator=(const VulkanWindow &other) = delete;

      VulkanWindow(VulkanWindow &&other) = delete;

      VulkanWindow &operator=(VulkanWindow &&other) = delete;

      ~VulkanWindow() override;

      void render() override;

    private:
      void createSwapChain();

      PixelFormat chooseSurfaceFormat() const;

      VkPresentModeKHR choosePresentMode() const;

      const VkInstance &instance;
      const VulkanDevice &device;

      // Makes Queue execution wait until the acquired image is done presenting
      std::vector<VkSemaphore> imageReadySemaphores;
      std::vector<VkSemaphore> renderFinishedSemaphores;
      std::vector<VkFence> imageFences;

      VkSurfaceKHR surface = VK_NULL_HANDLE;
      VkSwapchainKHR swapChain = VK_NULL_HANDLE;

      std::uint32_t swapChainImageCount = 0;

      std::vector<VkImage> swapChainImages;
      std::vector<VkImageView> swapChainImageViews;

      size_t currentSemaphoreIndex = 0;
  };
}
