#pragma once
#include <vulkan/vulkan.hpp>
#include "queue.hpp"

namespace Core::Graphics {
  class VulkanDevice {
    public:
      VulkanDevice(const VkInstance &vulkanInstance);

      ~VulkanDevice();

      void initialize(std::uint32_t deviceId);

      const char *getName() const { return deviceProperties.deviceName; }

      std::vector<const char *> getExtensions() const;

    private:
      VulkanQueue findGraphicsQueue(std::vector<std::uint32_t> &usedQueuesCount) const;

      std::vector<VulkanQueue> findComputeQueue(std::vector<std::uint32_t> &usedQueuesCount) const;

      std::vector<VulkanQueue> findTransferQueues(std::vector<std::uint32_t> &usedQueueCount) const;

      void createPhysicalDevice(std::uint32_t deviceId);

      void createLogicalDevice();

      const VkInstance &vulkanInstance;

      VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
      VkDevice logicalDevice = VK_NULL_HANDLE;
      VkPhysicalDeviceProperties deviceProperties{};
      VkPhysicalDeviceMemoryProperties deviceMemoryProperties{};
      VkPhysicalDeviceFeatures deviceFeatures{};

      std::uint32_t supportedStages = 0xFFFFFFFF;

      std::vector<VkQueueFamilyProperties> queueFamilyProperties;

      // The queue used to present to the surface
      VkQueue presentQueue = VK_NULL_HANDLE;
      // Graphics queue is *guaranteed by spec* to also be able to run compute and transfer
      // A GPU may not have a graphics queue though (renderer can't run there)
      // VulkanQueue graphicsQueue;
      // Additional compute queues to run async compute (besides the main graphics one)
      // std::vector<VulkanQueue> computeQueues;
      // // Additional transfer queues to run async transfers (besides the main graphics one)
      // std::vector<VulkanQueue> transferQueues;
  };
}