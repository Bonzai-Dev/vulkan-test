#include "window_manager.hpp"

namespace Core {
  WindowManager::WindowManager() = default;

  void WindowManager::addWindow(const WindowOptions& options) {
    windows.emplace_back(options);
  }

  void WindowManager::update() const {
    for (const auto &window: windows) {
      window.handleEvent(TODO);
      window.render();
    }
  }
}
