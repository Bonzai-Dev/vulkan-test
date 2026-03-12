#pragma once
#include <cstdint>
#include <memory>
#include "render_context.hpp"
#include "volk.h"

namespace Core::Graphics {
  class VulkanWindow {
    public:
      VulkanWindow(
        std::shared_ptr<VulkanRenderContext> context,
        const WindowOptions &options,
        SDL_WindowFlags flags
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

      std::shared_ptr<VulkanRenderContext> vulkanContext;
      const WindowOptions &options;

      VkSurfaceKHR surface = VK_NULL_HANDLE;
      VkSwapchainKHR swapChain = VK_NULL_HANDLE;

      std::vector<VkImage> swapChainImages;
      std::vector<VkImageView> swapChainImageViews;
  };
}