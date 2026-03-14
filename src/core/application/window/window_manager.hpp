#pragma once
#include <memory>
#include <vector>
#include "window.hpp"

namespace Core {
  class WindowManager {
    public:
      WindowManager();

      void addWindow(const WindowOptions& options);

      void update() const;

    private:
      std::vector<Window> windows;
  };
}
