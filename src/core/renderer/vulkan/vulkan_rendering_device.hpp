#pragma once
#include <cstdint>
#include <string>
#include <vector>
#include <unordered_map>
#include <core/events/window_events.hpp>
#include <core/events/input_events.hpp>
#include <core/renderer/window.hpp>
#include <core/renderer/pixel_format.hpp>
#include "volk.h"
#include "VkBootstrap.h"
#include "vulkan_window.hpp"
#include <vulkan/vulkan.h>

using namespace Core::Events;

namespace Core::Graphics {
  class VulkanRenderingDevice {
    public:
      explicit VulkanRenderingDevice(const char *appName, const DisplayInfo &displayInfo);

      ~VulkanRenderingDevice();

      VulkanRenderingDevice(const VulkanRenderingDevice &other) = delete;

      VulkanRenderingDevice &operator=(const VulkanRenderingDevice &other) = delete;

      VulkanRenderingDevice(VulkanRenderingDevice &&other) noexcept = delete;

      VulkanRenderingDevice &operator=(VulkanRenderingDevice &&other) noexcept = delete;

      // const std::vector<const char*> &getExtensions();

      bool validationLayersEnabled() const { return validationLayersSupported; }
      //
      // VkInstance getInstance() const { return instance; }
      //
      // const VulkanDevice *getCurrentDevice() const { return currentDevice; }
      //
      // const std::vector<const char*> &getInstanceLayers();
      //
      // int rateDevice(const VulkanDevice *device);

      void createWindow(const WindowOptions &options);

      void render();

      static VkFormat convertPixelFormat(PixelFormat format);

      static std::string vulkanResultToString(VkResult result);

    private:
      bool validationLayersSupported = false;
      const char *appName{};
      const DisplayInfo &displayInfo;
      std::unordered_map<std::uint32_t, VulkanWindow> windows;

      VkInstance instance;
      VkDebugUtilsMessengerEXT debugMessenger;
      VkPhysicalDevice currentGpu;
      VkDevice device;
      VkSurfaceKHR windowSurface;

      void onWindowShow(const WindowShown &event) {
        windows.at(event.windowId).show();
      }

      void onWindowHide(const WindowHidden &event) {
        windows.at(event.windowId).hide();
      }

      void onWindowResize(const WindowResized &event) {
        windows.at(event.windowId).resize(event.width, event.height);
      }

      void onWindowMouseMotion(const WindowMouseMotion &event) {
      }

      void onWindowMouseEnter(const WindowMouseEnter &event) {
      }

      void onWindowMouseLeave(const WindowMouseLeave &event) {
      }

      void onWindowFocusGained(const WindowFocusGained &event) {
      }

      void onWindowFocusLost(const WindowFocusLost &event) {
      }

      void onWindowMinimized(const WindowMinimized &event) {
      }

      void onWindowMaximized(const WindowMaximized &event) {
      }

      void onWindowRestored(const WindowRestored &event) {
      }

      void onWindowClose(const WindowClosed &event) {
      }

      void onWindowExposed(const WindowExposed &event) {
      }

      void onKeyPressed(const KeyPressedEvent &event) {

      }

      void onKeyReleased(const KeyReleasedEvent &event) {
      }
  };

  #define VULKAN_CHECK(vulkanCall) { \
    VkResult result = vulkanCall; \
    if (result != VK_SUCCESS) { \
      std::string vkfunc = #vulkanCall; \
      vkfunc = vkfunc.substr(0, vkfunc.find('(')); \
      throw std::runtime_error("Vulkan error: " + vkfunc + " failed with " + VulkanRenderingDevice::vulkanResultToString(result)); \
    } \
  }
}
