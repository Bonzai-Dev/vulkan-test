#include <cstdint>
#include <glm/glm.hpp>
#include <core/application/logger.hpp>
#include "application.hpp"

namespace Core {
  Application::Application(const char *name) {
    if (!SDL_Init(SDL_INIT_VIDEO)) {
      LOG_CORE_CRITICAL("Failed to initialize SDL Video: {}", SDL_GetError());
      return;
    }

    displays = SDL_GetDisplays(&displayCount);
    if (displayCount > 0)
      LOG_CORE_INFO("Found {} display(s).", displayCount);
    else
      LOG_CORE_WARNING("No displays found.");

    currentDisplay = SDL_GetCurrentDisplayMode(displays[0]);

    renderer = std::make_shared<Graphics::Renderer>(chooseGraphicsBackend(), name);

    run();
  }

  Application::~Application() {
    quit();
  }

  void Application::run() const {
    static std::uint64_t lastFrameTime = 0;
    static std::uint64_t currentFrameTime = 0;
    while (running) {
      lastFrameTime = currentFrameTime;
      currentFrameTime = SDL_GetPerformanceCounter();
      deltaTime = static_cast<double>(currentFrameTime - lastFrameTime) * 1000 /
        static_cast<double>(SDL_GetPerformanceFrequency());

      isMouseMoving = false;

      SDL_Event windowEvent;
      while (SDL_PollEvent(&windowEvent)) {
        switch (windowEvent.type) {
          case SDL_EVENT_QUIT:
            running = false;
            break;
        }
      }

      windowManager.update();
    }

    quit();
  }

  Graphics::Backend Application::chooseGraphicsBackend() const {
    // TODO: Implement graphics backend selection based on platform and availability
    const Graphics::Backend backend = Graphics::Backend::Vulkan;
    return backend;
  }

  bool Application::keyDown(Inputs::KeyboardKey key, Inputs::KeyDetectMode detectMode) const {
    if (!pressedKeys.contains(key)) return false;

    const SDL_KeyboardEvent keyEvent = pressedKeys[key];
    const SDL_Scancode scancode = keyEvent.scancode;
    const SDL_Keycode keycode = SDL_GetKeyFromScancode(keyEvent.scancode, keyEvent.mod, true);

    const bool isScancode = detectMode == Inputs::KeyDetectMode::Keycode && scancode == keyEvent.scancode;
    const bool isKeycode = detectMode == Inputs::KeyDetectMode::Scancode && keycode == keyEvent.key;
    if (isScancode || isKeycode)
      return true;

    return false;
  }

  void Application::quit() const {
    if (!running)
      return;

    running = false;
    SDL_free(displays);
    SDL_Quit();
  }
}
