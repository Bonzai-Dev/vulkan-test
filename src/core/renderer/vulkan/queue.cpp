#include <cstdint>
#include <core/application/logger.hpp>
#include "volk.h"
#include "util.hpp"
#include "queue.hpp"

namespace Core::Graphics {
  VulkanQueue::VulkanQueue(
    Type type,
    std::uint32_t familyIndex,
    std::uint32_t queueIndex
  ) :
  type(type),
  familyIndex(familyIndex),
  queueIndex(queueIndex) {
  }

  void VulkanQueue::initialize(const VkDevice &device, VkQueue queue, std::uint32_t bufferedFrameCount) {
    this->queue = queue;

    const VkCommandPoolCreateInfo commandPoolCreateInfo{
      .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
      .flags = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT,
      .queueFamilyIndex = familyIndex
    };

    constexpr VkCommandBufferAllocateInfo allocateInfo{
      .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
      .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
      .commandBufferCount = 1
    };
LOG_CORE_DEBUG(bufferedFrameCount);
    frameData.resize(bufferedFrameCount);
    for (auto &[commandPool, commandBuffer]: frameData) {
      VULKAN_CHECK(
        vkCreateCommandPool(device, &commandPoolCreateInfo, nullptr, &commandPool)
      );
      VULKAN_CHECK(vkAllocateCommandBuffers(device, &allocateInfo, &commandBuffer));
    }
  }
}
