#pragma once
#include <vulkan/vulkan.hpp>
#include "util.hpp"
#include "device.hpp"

namespace Core::Graphics {
  class VulkanDevice;

  class VulkanContext {
    public:
      VulkanContext(const char *appName, std::uint32_t frameBufferCount);

      ~VulkanContext();

      static std::uint32_t getFrameBufferCount() { return frameBufferCount; }

      static std::vector<const char*> getExtensions();

      static bool validationLayersEnabled() { return validationLayersSupported; }

      VkInstance getInstance() const { return instance; }

      const VulkanDevice &getCurrentDevice() const { return currentDevice; }

      const std::vector<VulkanDevice> &getDevices() const;

      int rateDevice(const VulkanDevice &device);

    private:
      void createInstance(
        const char *appName,
        const std::vector<const char*> &extensions,
        const std::vector<const char*> &layers
      );

      const VulkanDevice &chooseDevice(const std::vector<VulkanDevice> &devices);

      static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
        VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
        VkDebugUtilsMessageTypeFlagsEXT messageType,
        const VkDebugUtilsMessengerCallbackDataEXT *callbackData,
        void *userData
      );

      static inline bool validationLayersSupported;

      static inline std::uint32_t frameBufferCount;

      std::vector<const char*> instanceLayers;
      VkInstance instance = VK_NULL_HANDLE;
      VkDebugUtilsMessengerEXT debugMessenger = VK_NULL_HANDLE;

      VulkanDevice currentDevice;
  };
}