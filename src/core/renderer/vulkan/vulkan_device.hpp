#pragma once
#include <core/pointers.hpp>
#include "vk_mem_alloc.h"
#include "vulkan_queue.hpp"

namespace Core::Graphics {
  class VulkanRenderingDevice;

  class VulkanSwapChain;

  class VulkanQueue;

  class VulkanDevice: public RefCounted {
    public:
      VulkanDevice(const VulkanRenderingDevice &renderingDevice);

      // Class cannot be copied, only moved (since it owns a Vulkan device handle)
      VulkanDevice(const VulkanDevice &other) = delete;

      VulkanDevice &operator=(const VulkanDevice &other) = delete;

      VulkanDevice(VulkanDevice &&other) = delete;

      VulkanDevice &operator=(VulkanDevice &&other) = delete;

      ~VulkanDevice() override;

      void createPhysicalDevice(VkPhysicalDevice physicalDevice);

      void createLogicalDevice(VkDebugUtilsMessengerEXT debugMessenger);

      const char *getName() const { return deviceProperties.deviceName; }

      const VkPhysicalDeviceProperties &getProperties() const { return deviceProperties; }

      const VkPhysicalDeviceFeatures &getFeatures() const { return deviceFeatures; }

      std::vector<const char *> getExtensions() const;

      const std::vector<VulkanQueue> &getGraphicsQueues() const { return graphicsQueues; }

      const std::vector<VulkanQueue> &getComputeQueues() const { return computeQueues; }

      const std::vector<VulkanQueue> &getTransferQueues() const { return transferQueues; }

      void initializeQueues();

      static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
        VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
        VkDebugUtilsMessageTypeFlagsEXT messageType,
        const VkDebugUtilsMessengerCallbackDataEXT *callbackData,
        void *userData
      );

      VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
      VkDevice logicalDevice = VK_NULL_HANDLE;

    private:
      VkDebugUtilsMessengerEXT debugMessenger = VK_NULL_HANDLE;

      void findGraphicsQueue(std::vector<std::uint32_t> &usedQueuesCount);

      void findComputeQueue(std::vector<std::uint32_t> &usedQueuesCount);

      void findTransferQueues(std::vector<std::uint32_t> &usedQueueCount);

      const VulkanRenderingDevice &renderingDevice;

      VkPhysicalDeviceProperties deviceProperties{};
      VkPhysicalDeviceMemoryProperties deviceMemoryProperties{};
      VkPhysicalDeviceFeatures deviceFeatures{};

      std::uint32_t supportedStages = 0xFFFFFFFF;

      std::vector<VkQueueFamilyProperties> queueFamilyProperties;

      // The queue used to present to the surface
      VkQueue presentQueue = VK_NULL_HANDLE;
      // Graphics queue is *guaranteed by spec* to also be able to run compute and transfer
      // A GPU may not have a graphics queue though (renderer can't run there)
      std::vector<VulkanQueue> graphicsQueues;
      // Additional compute queues to run async compute (besides the main graphics one)
      std::vector<VulkanQueue> computeQueues;
      // Additional transfer queues to run async transfers (besides the main graphics one)
      std::vector<VulkanQueue> transferQueues;

      VmaAllocator memoryAllocator{};
  };
}
