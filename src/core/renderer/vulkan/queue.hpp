#pragma once
#include <cstdint>

namespace Core::Graphics {
  class VulkanQueue {
    public:
      enum class Type {
        Graphics,
        Compute,
        Transfer
      };

      VulkanQueue(Type type, std::uint32_t familyIndex, std::uint32_t queueIndex);

      ~VulkanQueue() = default;

    private:
      Type type;
      std::uint32_t familyIndex;
      std::uint32_t queueIndex;
  };
}
