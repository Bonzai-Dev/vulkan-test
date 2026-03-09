#include <string>
#include <stdexcept>
#include <SDL3/SDL_vulkan.h>
#include <core/application/logger.hpp>
#include <core/application/window.hpp>
#include "volk.h"
#include "render_window.hpp"

namespace Core::Graphics {
  VulkanRenderWindow::VulkanRenderWindow(const WindowOptions &options) : RenderWindow(options) {
    createSurface();
    createSwapChain();
  }

  VulkanRenderWindow::~VulkanRenderWindow() {
    for (auto &imageView : swapChainImageViews)
      destroyImageView(imageView);

    vkDestroySurfaceKHR(vulkan.getInstance(), surface, nullptr);
  }

  void VulkanRenderWindow::createSurface() {
    if (!SDL_Vulkan_CreateSurface(window, vulkan.getInstance(), nullptr, &surface))
      throw std::runtime_error("Failed to create window surface: " + std::string(SDL_GetError()));
  }

  void VulkanRenderWindow::createSwapChain() {
    VkBool32 supportPresent = false;
    vkGetPhysicalDeviceSurfaceSupportKHR(
      vulkanDevice.getPhysicalDevice(),
      vulkanDevice.getGraphicsQueue().familyIndex,
      surface,
      &supportPresent
    );
    if (!supportPresent) {
      LOG_CORE_CRITICAL("Surface is not supported by the graphics queue.");
      return;
    }

    VkSurfaceCapabilitiesKHR surfaceCapabilities;
    VULKAN_CHECK(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(
      vulkanDevice.getPhysicalDevice(), surface, &surfaceCapabilities
    ));

    // Tries to get triple buffering by default
    // https://github.com/KhronosGroup/Vulkan-Samples/tree/main/samples/performance/swapchain_images
    auto minImageCount = surfaceCapabilities.minImageCount + 1;
    if (surfaceCapabilities.maxImageCount != 0)
      minImageCount = std::min(minImageCount, surfaceCapabilities.maxImageCount);

    const VkPresentModeKHR presentMode = choosePresentMode();
    const VkSurfaceFormatKHR surfaceFormat = chooseSurfaceFormat();
    const VkSwapchainCreateInfoKHR swapChainCreateInfo{
      .sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
      .surface = surface,
      .minImageCount = minImageCount,
      .imageFormat = surfaceFormat.format,
      .imageColorSpace = VK_COLORSPACE_SRGB_NONLINEAR_KHR,
      .imageExtent = surfaceCapabilities.currentExtent,
      .imageArrayLayers = 1,
      .imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT,
      .imageSharingMode = VK_SHARING_MODE_EXCLUSIVE,
      .queueFamilyIndexCount = 0,
      .pQueueFamilyIndices = nullptr,
      .preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR,
      .compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
      .presentMode = presentMode,
      .clipped = VK_TRUE,
    };

    VULKAN_CHECK(vkCreateSwapchainKHR(vulkanDevice.getDevice(), &swapChainCreateInfo, nullptr, &swapChain));

    std::uint32_t swapChainImageCount = 0;
    VULKAN_CHECK(vkGetSwapchainImagesKHR(vulkanDevice.getDevice(), swapChain, &swapChainImageCount, nullptr));
    swapChainImages.resize(swapChainImageCount);
    swapChainImageViews.resize(swapChainImageCount);

    VULKAN_CHECK(vkGetSwapchainImagesKHR(vulkanDevice.getDevice(), swapChain, &swapChainImageCount, swapChainImages.data()));

    for (size_t imageIndex = 0; imageIndex < swapChainImageCount; imageIndex++) {
      createImageView(
        swapChainImages[imageIndex],
        surfaceFormat.format,
        VK_IMAGE_ASPECT_COLOR_BIT,
        VK_IMAGE_VIEW_TYPE_2D,
        1,
        1
      );
    }

    LOG_CORE_DEBUG("Swap chain contains {} image(s).", swapChainImageCount);
  }

  VkSurfaceFormatKHR VulkanRenderWindow::chooseSurfaceFormat() const {
    std::uint32_t formatsCount = 0;
    VULKAN_CHECK(vkGetPhysicalDeviceSurfaceFormatsKHR(
      vulkanDevice.getPhysicalDevice(), surface, &formatsCount, nullptr
    ));

    if (formatsCount == 0) {
      LOG_CORE_CRITICAL("No surface formats found.");
      return {};
    }

    std::vector<VkSurfaceFormatKHR> formats(formatsCount);
    VULKAN_CHECK(vkGetPhysicalDeviceSurfaceFormatsKHR(
      vulkanDevice.getPhysicalDevice(), surface, &formatsCount, formats.data()
    ));

    VkSurfaceFormatKHR chosenFormat = formats[0];
    for (size_t formatIndex = 0; formatIndex < formatsCount; formatIndex++) {
      const auto &[format, colorSpace] = formats[formatIndex];
      if (format == VK_FORMAT_B8G8R8A8_SRGB && colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
        return formats[formatIndex];
    }

    return chosenFormat;
  }

  VkPresentModeKHR VulkanRenderWindow::choosePresentMode() const {
    std::uint32_t presentModesCount = 0;
    vkGetPhysicalDeviceSurfacePresentModesKHR(
      vulkanDevice.getPhysicalDevice(), surface, &presentModesCount, nullptr
    );

    std::vector<VkPresentModeKHR> presentModes(presentModesCount);
    vkGetPhysicalDeviceSurfacePresentModesKHR(
      vulkanDevice.getPhysicalDevice(), surface, &presentModesCount, presentModes.data()
    );

    // FIFO is guaranteed to be present
    VkPresentModeKHR presentMode = VK_PRESENT_MODE_FIFO_KHR;
    if (!options.vsync && std::find(presentModes.begin(), presentModes.end(), VK_PRESENT_MODE_IMMEDIATE_KHR) != presentModes.end())
      presentMode = VK_PRESENT_MODE_IMMEDIATE_KHR;

    return presentMode;
  }

  VkImageView VulkanRenderWindow::createImageView(
    const VkImage &image,
    const VkFormat &format,
    VkImageAspectFlags aspectFlags,
    VkImageViewType viewType,
    std::uint32_t layerCount,
    std::uint32_t mipLevels
  ) const {
    VkImageViewCreateInfo createInfo{
      .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
      .image = image,
      .viewType = viewType,
      .format = format,
      .components = {
        .r = VK_COMPONENT_SWIZZLE_IDENTITY,
        .g = VK_COMPONENT_SWIZZLE_IDENTITY,
        .b = VK_COMPONENT_SWIZZLE_IDENTITY,
        .a = VK_COMPONENT_SWIZZLE_IDENTITY
      },
      .subresourceRange = {
        .aspectMask = aspectFlags,
        .baseMipLevel = 0,
        .levelCount = mipLevels,
        .baseArrayLayer = 0,
        .layerCount = layerCount
      }
    };

    VkImageView imageView;
    VULKAN_CHECK(vkCreateImageView(vulkanDevice.getDevice(), &createInfo, nullptr, &imageView));

    return imageView;
  }

  void VulkanRenderWindow::destroyImageView(const VkImageView &imageView) const {
    vkDestroyImageView(vulkanDevice.getDevice(), imageView, nullptr);
  }
}
