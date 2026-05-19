#include <cstdint>
#include <core/application/logger.hpp>
#include "vulkan_rendering_device.hpp"
#include "volk.h"
#include "vulkan_queue.hpp"

namespace Core::Graphics {
  VulkanQueue::~VulkanQueue() {
    destroy();
  }

  void VulkanQueue::destroy() {
    if (!device.logicalDevice)
      return;

    vkDeviceWaitIdle(device.logicalDevice);
    for (const auto &data : frameData)
      vkDestroyCommandPool(device.logicalDevice, data.commandPool, nullptr);
  }

  void VulkanQueue::initialize(VkQueue queue, std::uint32_t bufferedFrameCount) {
    this->queue = queue;

    VkCommandPoolCreateInfo commandPoolCreateInfo {};
    commandPoolCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    commandPoolCreateInfo.flags = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT;
    commandPoolCreateInfo.queueFamilyIndex = familyIndex;

    VkCommandBufferAllocateInfo allocateInfo {};
    allocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocateInfo.commandBufferCount = 1;

    VkFenceCreateInfo fenceCreateInfo = { VK_STRUCTURE_TYPE_FENCE_CREATE_INFO };
    fenceCreateInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    frameData.resize(bufferedFrameCount);
    for (auto &data: frameData) {
      VULKAN_CHECK(vkCreateCommandPool(device.logicalDevice, &commandPoolCreateInfo, nullptr, &data.commandPool));
      allocateInfo.commandPool = data.commandPool;

      VULKAN_CHECK(vkAllocateCommandBuffers(device.logicalDevice, &allocateInfo, &data.commandBuffer));
      VULKAN_CHECK(vkCreateFence(device.logicalDevice, &fenceCreateInfo, nullptr, &data.renderFence));
    }
  }

  void VulkanQueue::newCommandBuffer() {
    vkResetCommandPool(device.logicalDevice, frameData[currentFrameIndex].commandPool, 0);

    // frameData[currentFrameIndex].renderFence;
    VkCommandBufferBeginInfo beginInfo = {VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO};
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
    vkBeginCommandBuffer(frameData[currentFrameIndex].commandBuffer, &beginInfo);
    VULKAN_CHECK(vkWaitForFences(device.logicalDevice, 1, &frameData[currentFrameIndex].renderFence, true, 1000000000));
    VULKAN_CHECK(vkResetFences(device.logicalDevice, 1, &frameData[currentFrameIndex].renderFence));
  }
}
