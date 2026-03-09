#pragma once
#include <cstdint>
#include <vector>
#include "volk.h"

namespace Core::Graphics {
  struct VulkanFrameData {
    VkCommandPool commandPool;
    VkCommandBuffer commandBuffer;
  };

  class VulkanQueue {
    public:
      enum class Type {
        Graphics,
        Compute,
        Transfer
      };

      VulkanQueue(
        Type type,
        std::uint32_t familyIndex,
        std::uint32_t queueIndex
      );

      ~VulkanQueue() = default;

      void initialize(const VkDevice &device, VkQueue queue, std::uint32_t bufferedFrameCount);

      Type type;
      std::uint32_t familyIndex;
      std::uint32_t queueIndex;
      VkQueue queue = VK_NULL_HANDLE;

    private:
      // One per buffered frame
      std::vector<VulkanFrameData> frameData;
  };
}
