#pragma once
#include <cstdint>
#include <core/application/resource.hpp>

namespace Core::Graphics {
  class Texture: public Resource {
    public:
      Texture() = default;

      ~Texture() override = default;

      virtual std::uint32_t getWidth() const { return 0; }

      virtual std::uint32_t getHeight() const { return 0; }

    private:

  };
}