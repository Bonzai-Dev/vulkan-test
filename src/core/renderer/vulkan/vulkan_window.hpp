#pragma once
#include "../window.hpp"
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

      ~VulkanWindow();
    
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

      VkSurfaceFormatKHR chooseSurfaceFormat() const;

      VkPresentModeKHR choosePresentMode() const;

      const VkInstance &instance;
      const VulkanDevice &device;

      VkSurfaceKHR surface = VK_NULL_HANDLE;
      VkSwapchainKHR swapChain = VK_NULL_HANDLE;

      std::vector<VkImage> swapChainImages;
      std::vector<VkImageView> swapChainImageViews;
  };
}
