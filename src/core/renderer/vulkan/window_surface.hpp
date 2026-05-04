#pragma once
#include <cstdint>
#include <memory>
#include "rendering_device.hpp"

namespace Core::Graphics {
  class VulkanWindowSurface {
    enum SwapchainStatus {
      /// We already called VulkanWindow::acquireNextImage.
      ///
      /// Can only go into this state if we're coming from SwapchainReleased
      SwapchainAcquired,
      /// We already called VulkanWindow::getImageAcquiredSemaphore.
      /// Further calls to getImageAcquiredSemaphore will return null.
      /// It is rendering or intends to into this swapchain.
      ///
      /// Can only go into this state if we're coming from SwapchainAcquired
      SwapchainUsedInRendering,
      /// We've come from SwapchainUsedInRendering and are waiting for
      /// VulkanDevice::commitAndNextCommandBuffer to present us
      SwapchainPendingSwap,
      /// We don't own a swapchain. Cannot render to this window.
      ///
      /// This status should not last long unless we're not initialized yet.
      SwapchainReleased
    };

    public:
      VulkanWindowSurface(SDL_Window &window);

      VulkanWindowSurface(const VulkanWindowSurface &other) = delete;

      VulkanWindowSurface &operator=(const VulkanWindowSurface &other) = delete;

      VulkanWindowSurface(VulkanWindowSurface &&other) = delete;

      VulkanWindowSurface &operator=(VulkanWindowSurface &&other) = delete;

      ~VulkanWindowSurface();

    private:
      VkImageView createImageView(
        const VkImage &image,
        const VkFormat &format,
        VkImageAspectFlags aspectFlags,
        VkImageViewType viewType,
        std::uint32_t layerCount,
        std::uint32_t mipLevels
      ) const;

      void destroyImageView(const VkImageView &imageView) const;

      void createSurface();

      void createSwapChain();

      SDL_Window &window;

      VkSurfaceFormatKHR chooseSurfaceFormat() const;

      VkPresentModeKHR choosePresentMode() const;

      std::shared_ptr<VulkanRenderingDevice> vulkanContext;

      VkSurfaceKHR surface = VK_NULL_HANDLE;
      VkSwapchainKHR swapChain = VK_NULL_HANDLE;

      std::vector<VkImage> swapChainImages;
      std::vector<VkImageView> swapChainImageViews;
  };
}
