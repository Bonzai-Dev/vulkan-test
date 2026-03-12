#pragma once

// Class that each graphics API inherits from. 
// Used to abstract away the differences between graphics APIs and provide a common interface for the renderer to use.

namespace Core::Graphics {
  class RenderContext {
    public:
      RenderContext() = default;

      virtual ~RenderContext() = default;
  };
}
