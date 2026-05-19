#pragma once
#include <cstdint>
#include <vector>
#include "volk.h"

namespace Core::Graphics {
  class VulkanDevice;

  struct VulkanFrameData {
    VkCommandPool commandPool;
    VkCommandBuffer commandBuffer;

    VkFence renderFence;
    VkSemaphore swapChainSemaphore, renderSemaphore;
  };

  class VulkanQueue {
    public:
      enum class Type {
        Graphics,
        Compute,
        Transfer
      };

      VulkanQueue(const VulkanDevice &device): device(device) {}

      ~VulkanQueue();

      void destroy();

      void initialize(VkQueue queue, std::uint32_t bufferedFrameCount);

      void newCommandBuffer();

      Type type = Type::Graphics;
      std::uint32_t familyIndex = 0;
      std::uint32_t queueIndex = 0;
      VkQueue queue = VK_NULL_HANDLE;

    private:
      const VulkanDevice &device;

      // One per buffered frame
      std::vector<VulkanFrameData> frameData;

      size_t currentFrameIndex = 0;
  };
}