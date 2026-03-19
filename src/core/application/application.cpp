#include <cstdint>
#include <core/application/logger.hpp>
#include "events/application.hpp"
#include "application.hpp"

namespace Core {
  Application::Application(const char *name) : name(name) {
    createWindow({
      .mouseLocked = false,
      .fullScreen = true,
      .vsync = true,
      .minimized = true,
      .windowName = "Game"
    });

    run();
  }

  Application::~Application() {
    quit();
  }

  bool onQuit(const Events::ApplicationQuit &event) {
    LOG_CORE_CRITICAL("QUit");
    return false;
  }

  void Application::run() const {
    static std::uint64_t lastFrameTime = 0;
    static std::uint64_t currentFrameTime = 0;
    while (running) {
      lastFrameTime = currentFrameTime;
      currentFrameTime = SDL_GetPerformanceCounter();
      deltaTime = static_cast<double>(currentFrameTime - lastFrameTime) * 1000 /
                  static_cast<double>(SDL_GetPerformanceFrequency());

      // eventDispatcher.processEvents();
      SDL_Event windowEvent;
      while (SDL_PollEvent(&windowEvent)) {
        switch (windowEvent.type) {
          case SDL_EVENT_QUIT: {
            running = false;
            const Events::ApplicationQuit event;
            Events::EventDispatcher<Events::ApplicationQuit>([&event]() {
              return onQuit(event);
            });
            break;
          }
          default:
            break;
        }
      }

      // isMouseMoving = false;

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
    SDL_Quit();
  }
}
