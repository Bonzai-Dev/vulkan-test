#pragma once
#include <core/renderer/render_window.hpp>
#include "volk.h"
#include "context.hpp"
#include "device.hpp"

namespace Core::Graphics {
  class VulkanContext;

  class VulkanDevice;

  class VulkanRenderWindow : public RenderWindow {
    public:
      VulkanRenderWindow(const WindowOptions &options);

      ~VulkanRenderWindow();

      std::uint32_t getBufferedFrameCount() const { return swapChainImages.size(); }

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

      VulkanContext vulkan = VulkanContext("Vulkan render window", 2);
      const VulkanDevice &vulkanDevice = vulkan.getCurrentDevice();

      VkSurfaceKHR surface = VK_NULL_HANDLE;
      VkSwapchainKHR swapChain = VK_NULL_HANDLE;

      std::vector<VkImage> swapChainImages;
      std::vector<VkImageView> swapChainImageViews;
  };
}
