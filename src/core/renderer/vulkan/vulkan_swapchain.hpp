#pragma once
#include <cstdint>
#include <memory>
#include "vulkan_rendering_device.hpp"

namespace Core::Graphics {
  class VulkanSwapChain {
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
      VulkanSwapChain(const VulkanRenderingDevice &renderingDevice, SDL_Window &window, const WindowOptions &options);

      VulkanSwapChain(const VulkanSwapChain &other) = delete;

      VulkanSwapChain &operator=(const VulkanSwapChain &other) = delete;

      VulkanSwapChain(VulkanSwapChain &&other) = delete;

      VulkanSwapChain &operator=(VulkanSwapChain &&other) = delete;

      ~VulkanSwapChain();

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
      const WindowOptions &windowOptions;

      VkSurfaceFormatKHR chooseSurfaceFormat() const;

      VkPresentModeKHR choosePresentMode() const;

      const VulkanRenderingDevice &renderingDevice;

      VkSurfaceKHR surface = VK_NULL_HANDLE;
      VkSwapchainKHR swapChain = VK_NULL_HANDLE;

      std::vector<VkImage> swapChainImages;
      std::vector<VkImageView> swapChainImageViews;
  };
}
