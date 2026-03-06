#pragma once
#include <string>
#include "volk.h"

namespace Core::Graphics {
  std::string vulkanResultToString(VkResult result);
}