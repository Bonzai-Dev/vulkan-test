#include <SDL3/SDL_vulkan.h>
#include "vulkan_window.hpp"
#include "vulkan_rendering_device.hpp"

namespace Core::Graphics {
  VulkanWindow::VulkanWindow(
    const VulkanDevice &device,
    const VkInstance &instance,
    const DisplayInfo &displayInfo,
    const WindowOptions &windowOptions
  ) : options(windowOptions), instance(instance), device(device), displayInfo(displayInfo) {
    std::uint32_t width = options.width;
    std::uint32_t height = options.height;

    if (options.fullScreen)
      windowFlags |= SDL_WINDOW_FULLSCREEN;

    if (options.resizable)
      windowFlags |= SDL_WINDOW_RESIZABLE;

    windowFlags |= SDL_WINDOW_VULKAN;

    window = SDL_CreateWindow(
      options.windowName,
      static_cast<int>(width), static_cast<int>(height),
      windowFlags
    );

    if (!window) {
      LOG_CORE_ERROR("Failed to create window: {}", SDL_GetError());
      return;
    }

    SDL_Vulkan_CreateSurface(window, instance, nullptr, &surface);
    vkb::SwapchainBuilder swapChainBuilder { device.getPhysicalDevice(), device.getDevice(), surface };
    swapChainFormat = VK_FORMAT_B8G8R8A8_UNORM;

    vkb::Swapchain vkbSwapChain = swapChainBuilder
      //.use_default_format_selection()
      .set_desired_format(VkSurfaceFormatKHR{ .format = swapChainFormat, .colorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR })
      //use vsync present mode
      .set_desired_present_mode(VK_PRESENT_MODE_FIFO_KHR)
      .set_desired_extent(options.width, options.height)
      .add_image_usage_flags(VK_IMAGE_USAGE_TRANSFER_DST_BIT)
      .build()
      .value();

    swapChain = vkbSwapChain.swapchain;
    swapChainExtent = vkbSwapChain.extent;
    swapChainImages = vkbSwapChain.get_images().value();
    swapChainImageViews = vkbSwapChain.get_image_views().value();

    SDL_SetWindowRelativeMouseMode(window, options.mouseLocked);
  }

  void VulkanWindow::destroy() const {
    vkDestroySwapchainKHR(device.getDevice(), swapChain, nullptr);
    for (int i = 0; i < swapChainImageViews.size(); i++)
      vkDestroyImageView(device.getDevice(), swapChainImageViews[i], nullptr);

    vkDestroySurfaceKHR(instance, surface, nullptr);

    SDL_DestroyWindow(window);
  }

  void VulkanWindow::render() {

  }
}
