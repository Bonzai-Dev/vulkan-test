#pragma once
#include <string>
#include "volk.h"

namespace Core::Graphics {
  std::string vulkanResultToString(VkResult result);
}

// clang-format off
#define VULKAN_CHECK(vkcall) { \
  VkResult result = vkcall; \
    if (result != VK_SUCCESS) { \
      std::string vkfunc = #vkcall; \
      vkfunc = vkfunc.substr(0, vkfunc.find('(')); \
      throw std::runtime_error("Vulkan error: " + vkfunc + " failed with " + vulkanResultToString(result)); \
  } \
}
