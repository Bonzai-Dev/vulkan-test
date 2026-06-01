#pragma once
#include <cstdint>
#include <vector>
#include <vulkan/vulkan.h>
#include <SDL3/SDL_mouse.h>
#include <core/renderer/window.hpp>
#include "vulkan_device.hpp"

namespace Core::Graphics {
  struct VulkanFrameData {
    VkCommandPool commandPool;
    VkCommandBuffer commandBuffer;
    VkSemaphore swapChainSemaphore, renderSemaphore;
    VkFence renderFence;
  };

  class VulkanWindow {
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

      ~VulkanWindow() = default;

      WindowOptions options;

      void destroy() const;

      void render();

      void show() { shown = true; }

      void hide() { shown = false; }

      void focusMouse() { mouseFocused = true; }

      void unfocusMouse() { mouseFocused = false; }

      void focusKeyboard() { keyboardFocused = true; }

      void unfocusKeyboard() { keyboardFocused = false; }

      void minimize() { minimized = true; }

      void maximize() { minimized = false; }

      void resize(std::uint32_t width, std::uint32_t height) {
        options.width = width;
        options.height = height;
      }

      void restore() { minimized = false; }

      void close() const { SDL_HideWindow(window); }

      void lockMouse() const { SDL_SetWindowRelativeMouseMode(window, true); }

      void unlockMouse() const { SDL_SetWindowRelativeMouseMode(window, false); }

      std::uint32_t getId() const { return SDL_GetWindowID(window); }

    private:
      const VkInstance &instance;
      const VulkanDevice &device;
      VkSwapchainKHR swapChain;
      VkSurfaceKHR surface = VK_NULL_HANDLE;
      VkFormat swapChainFormat;

      std::vector<VkImage> swapChainImages;
      std::vector<VkImageView> swapChainImageViews;
      VkExtent2D swapChainExtent;

      bool minimized = false;
      bool shown = false;
      bool mouseFocused = false; // Whether if the mouse is in the window or not
      bool keyboardFocused = false;

      const DisplayInfo &displayInfo;

      SDL_Window *window = nullptr;
      std::uint64_t windowFlags = 0;
  };
}
