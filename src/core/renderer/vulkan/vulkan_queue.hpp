#pragma once
#include <cstdint>
#include <vulkan/vulkan.h>

namespace Core::Graphics {
  class VulkanQueue {
    public:
      VulkanQueue() = default;

      void initialize(VkDevice device, VkQueue queue, std::uint32_t queueFamilyIndex, std::size_t frameCount);

      void destroy() const;

      VkQueue getQueue() const { return queue; }

      std::uint32_t getQueueFamilyIndex() const { return familyIndex; }

      void render();

    private:
      VkDevice device;
      std::vector<VulkanFrameData> frameData;

      VkQueue queue = VK_NULL_HANDLE;
      std::uint32_t familyIndex = 0;
  };
}