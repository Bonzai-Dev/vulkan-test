#include <cstdint>
#include "queue.hpp"

namespace Core::Graphics {
  VulkanQueue::VulkanQueue(Type type, std::uint32_t familyIndex, std::uint32_t queueIndex) :
  type(type),
  familyIndex(familyIndex),
  queueIndex(queueIndex) {

  }
}
