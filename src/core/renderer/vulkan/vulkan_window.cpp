#include <SDL3/SDL_vulkan.h>
#include <core/application/logger.hpp>
#include "volk.h"
#include "vulkan_rendering_device.hpp"
#include "vulkan_window.hpp"
#include "vulkan_device.hpp"

namespace Core::Graphics {
  VulkanWindow::VulkanWindow(
    const VulkanDevice &device,
    const VkInstance &instance,
    const DisplayInfo &displayInfo,
    const WindowOptions &windowOptions
  ) : Window(displayInfo, windowOptions), instance(instance), device(device) {
    if (!SDL_Vulkan_CreateSurface(window, instance, nullptr, &surface))
      LOG_CORE_ERROR("Failed to create window surface: {}", std::string(SDL_GetError()));

    createSwapChain();
  }

  VulkanWindow::~VulkanWindow() {
    vkDestroySwapchainKHR(device.logicalDevice, swapChain, nullptr);
    for (const auto &imageView : swapChainImageViews)
      vkDestroyImageView(device.logicalDevice, imageView, nullptr);

    vkDestroySurfaceKHR(instance, surface, nullptr);
  }

  void VulkanWindow::render() {
    currentSemaphoreIndex = (currentSemaphoreIndex + 1) % imageReadySemaphores.size();
    VkSemaphore semaphore = imageReadySemaphores[currentSemaphoreIndex];

    std::uint32_t imageIndex = 0;
    VkResult result = vkAcquireNextImageKHR(device.logicalDevice, swapChain, UINT64_MAX, semaphore, VK_NULL_HANDLE, &imageIndex);
    if(imageFences[imageIndex])
      VULKAN_CHECK(vkWaitForFences(device.logicalDevice, 1, &imageFences[imageIndex], VK_TRUE, UINT64_MAX));
  }

  void VulkanWindow::createSwapChain() {
    VkSurfaceCapabilitiesKHR surfaceCapabilities;
    VULKAN_CHECK(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(
      device.physicalDevice, surface, &surfaceCapabilities
    ));

    VkBool32 supported = false;
    VULKAN_CHECK(vkGetPhysicalDeviceSurfaceSupportKHR(
      device.physicalDevice,
      device.getGraphicsQueues()[0].familyIndex,
      surface,
      &supported
    ));

    if (!supported) {
      LOG_CORE_CRITICAL("KHR Surface is unsupported");
      return;
    }

    // Tries to get triple buffering by default
    // https://github.com/KhronosGroup/Vulkan-Samples/tree/main/samples/performance/swapchain_images
    std::uint32_t minImageCount = surfaceCapabilities.minImageCount + 1;
    if (surfaceCapabilities.maxImageCount != 0)
      minImageCount = std::min(minImageCount, surfaceCapabilities.maxImageCount);

    const VkPresentModeKHR presentMode = choosePresentMode();
    const VkFormat surfaceFormat = VulkanRenderingDevice::convertPixelFormat(chooseSurfaceFormat());
    const VkSwapchainCreateInfoKHR swapChainCreateInfo{
      .sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
      .surface = surface,
      .minImageCount = minImageCount,
      .imageFormat = surfaceFormat,
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

    VULKAN_CHECK(vkCreateSwapchainKHR(device.logicalDevice, &swapChainCreateInfo, nullptr, &swapChain));

    VULKAN_CHECK(vkGetSwapchainImagesKHR(device.logicalDevice, swapChain, &swapChainImageCount, nullptr));
    swapChainImages.resize(swapChainImageCount);
    swapChainImageViews.resize(swapChainImageCount);
    renderFinishedSemaphores.resize(swapChainImageCount);
    imageReadySemaphores.resize(swapChainImageCount);
    imageFences.resize(swapChainImageCount);

    VULKAN_CHECK(vkGetSwapchainImagesKHR(device.logicalDevice, swapChain, &swapChainImageCount, swapChainImages.data()));

    VkSemaphoreCreateInfo semaphoreCreateInfo = { VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO };
    for (size_t imageIndex = 0; imageIndex < swapChainImageCount; imageIndex++) {
      VULKAN_CHECK(
        vkCreateSemaphore(device.logicalDevice, &semaphoreCreateInfo, nullptr, &imageReadySemaphores[imageIndex])
      );
      VULKAN_CHECK(
        vkCreateSemaphore(device.logicalDevice, &semaphoreCreateInfo, nullptr, &renderFinishedSemaphores[imageIndex])
      );
    }

    LOG_CORE_DEBUG("Swap chain contains {} image(s)", swapChainImageCount);
  }

  PixelFormat VulkanWindow::chooseSurfaceFormat() const {
    std::uint32_t formatsCount = 0;
    VULKAN_CHECK(vkGetPhysicalDeviceSurfaceFormatsKHR(
      device.physicalDevice, surface, &formatsCount, nullptr
    ));

    if (formatsCount == 0) {
      LOG_CORE_CRITICAL("No surface formats found");
      return {};
    }

    std::vector<VkSurfaceFormatKHR> formats(formatsCount);
    VULKAN_CHECK(vkGetPhysicalDeviceSurfaceFormatsKHR(
      device.physicalDevice, surface, &formatsCount, formats.data()
    ));

    PixelFormat chosenFormat = PixelFormat::Unknown;
    for(size_t formatIndex = 0; formatIndex < formatsCount && chosenFormat == PixelFormat::Unknown; formatIndex++) {
      switch(formats[formatIndex].format) {
        case VK_FORMAT_R8G8B8A8_UNORM:
          chosenFormat = PixelFormat::R8G8B8A8;
          break;
        case VK_FORMAT_B8G8R8A8_UNORM:
          chosenFormat = PixelFormat::B8G8R8A8;
          break;
        default:
          break;
      }
    }

    return chosenFormat;
  }

  VkPresentModeKHR VulkanWindow::choosePresentMode() const {
    std::uint32_t presentModesCount = 0;
    vkGetPhysicalDeviceSurfacePresentModesKHR(
      device.physicalDevice, surface, &presentModesCount, nullptr
    );

    std::vector<VkPresentModeKHR> presentModes(presentModesCount);
    vkGetPhysicalDeviceSurfacePresentModesKHR(
      device.physicalDevice, surface, &presentModesCount, presentModes.data()
    );

    // FIFO is guaranteed to be present
    VkPresentModeKHR presentMode = VK_PRESENT_MODE_FIFO_KHR;
    if (!options.vsync && std::ranges::find(presentModes, VK_PRESENT_MODE_IMMEDIATE_KHR) != presentModes.end())
      presentMode = VK_PRESENT_MODE_IMMEDIATE_KHR;

    return presentMode;
  }
}
