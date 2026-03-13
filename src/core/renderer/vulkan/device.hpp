#pragma once
#include "queue.hpp"

namespace Core::Graphics {
  class VulkanRenderContext;

  class VulkanWindow;

  class VulkanQueue;

  class VulkanDevice {
    public:
      VulkanDevice();

      // Class cannot be copied, only moved (since it owns a Vulkan device handle)
      VulkanDevice(const VulkanDevice &other) = delete;

      VulkanDevice &operator=(const VulkanDevice &other) = delete;

      VulkanDevice(VulkanDevice &&other) noexcept;

      VulkanDevice &operator=(VulkanDevice &&other) noexcept;

      ~VulkanDevice();

      void createPhysicalDevice(VkPhysicalDevice physicalDevice);

      void createLogicalDevice();

      const char *getName() const { return deviceProperties.deviceName; }

      const VkPhysicalDeviceProperties &getProperties() const { return deviceProperties; }

      const VkPhysicalDeviceFeatures &getFeatures() const { return deviceFeatures; }

      std::vector<const char *> getExtensions() const;

      VkPhysicalDevice getPhysicalDevice() const { return physicalDevice; }

      VkDevice getDevice() const { return logicalDevice; }

      const VulkanQueue &getGraphicsQueue() const { return graphicsQueue; }

      const std::vector<VulkanQueue> &getComputeQueues() const { return computeQueues; }

      const std::vector<VulkanQueue> &getTransferQueues() const { return transferQueues; }

      void initializeQueues();

    private:
      VulkanQueue findGraphicsQueue(std::vector<std::uint32_t> &usedQueuesCount) const;

      std::vector<VulkanQueue> findComputeQueue(std::vector<std::uint32_t> &usedQueuesCount) const;

      std::vector<VulkanQueue> findTransferQueues(std::vector<std::uint32_t> &usedQueueCount) const;

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
      VulkanQueue graphicsQueue{VulkanQueue::Type::Graphics, 0, 0};
      // Additional compute queues to run async compute (besides the main graphics one)
      std::vector<VulkanQueue> computeQueues;
      // Additional transfer queues to run async transfers (besides the main graphics one)
      std::vector<VulkanQueue> transferQueues;
  };
}