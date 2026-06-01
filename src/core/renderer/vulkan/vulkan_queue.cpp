#include <cstdint>
#include "vulkan_rendering_device.hpp"
#include "vulkan_queue.hpp"

namespace Core::Graphics {
  void VulkanQueue::initialize(VkDevice device, VkQueue queue, std::uint32_t queueFamilyIndex, std::size_t frameCount) {
    this->device = device;
    this->queue = queue;
    this->familyIndex = queueFamilyIndex;

    VkCommandPoolCreateInfo commandPoolCreateInfo {};
    commandPoolCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    commandPoolCreateInfo.flags = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT;
    commandPoolCreateInfo.queueFamilyIndex = familyIndex;

    VkFenceCreateInfo fenceCreateInfo = { VK_STRUCTURE_TYPE_FENCE_CREATE_INFO };
    fenceCreateInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    frameData.resize(frameCount);
    for (auto &data: frameData) {
      VULKAN_CHECK(vkCreateCommandPool(device, &commandPoolCreateInfo, nullptr, &data.commandPool));

      VkCommandBufferAllocateInfo allocateInfo {};
      allocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
      allocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
      allocateInfo.commandBufferCount = 1;
      allocateInfo.commandPool = data.commandPool;

      VULKAN_CHECK(vkAllocateCommandBuffers(device, &allocateInfo, &data.commandBuffer));
      // VULKAN_CHECK(vkCreateFence(device, &fenceCreateInfo, nullptr, &data.renderFence));
    }
  }

  void VulkanQueue::destroy() const {
    vkDeviceWaitIdle(device);

    for (size_t i = 0; i < frameData.size(); i++)
      vkDestroyCommandPool(device, frameData[i].commandPool, nullptr);
  }

  void VulkanQueue::render() {
    VULKAN_CHECK(vkWaitForFences(device, 1, &frameData[].renderFence, true, 1000000000));
    VULKAN_CHECK(vkResetFences(device, 1, &frameData[].renderFence));
    //request image from the swapchain
    uint32_t swapchainImageIndex;
    VULKAN_CHECK(vkAcquireNextImageKHR(device, swapChain, 1000000000, get_current_frame()._swapchainSemaphore, nullptr, &swapchainImageIndex));
  }
}
