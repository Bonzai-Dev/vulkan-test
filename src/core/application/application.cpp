#include <glm/glm.hpp>
#include <core/application/logger.hpp>
#include <core/renderer/render_context.hpp>
#include "window.hpp"
#include "application.hpp"

namespace Core {
  Application::Application(const char *name) {
    graphicsBackend = Graphics::Backend::Vulkan;

    if (!SDL_Init(SDL_INIT_VIDEO)) {
      LOG_CORE_CRITICAL("Failed to initialize SDL Video: {}", SDL_GetError());
      return;
    }

    displays = SDL_GetDisplays(&displayCount);
    if (displayCount > 0)
      LOG_CORE_INFO("Found {} display(s).", displayCount);
    else
      LOG_CORE_WARNING("No displays found.");

    const Graphics::Backend backend = Graphics::Backend::Vulkan;
    switch (backend) {
      case  Graphics::Backend::Vulkan:
        break;

      default:
        LOG_CORE_CRITICAL("No suitable graphics backend found.");
        break;
    }
    // renderContext = std::make_shared<Vu>("Vulkan test");
    currentDisplay = SDL_GetCurrentDisplayMode(displays[0]);

    const WindowOptions options {
      .mouseLocked = true,
      .fullScreen = true,
      .vsync =  true,
      .width = static_cast<std::uint32_t>(currentDisplay->w),
      .height = static_cast<std::uint32_t>(currentDisplay->h)
    };
    mainWindow = std::make_shared<Window>(options);

    running = true;
    run();
  }

  Application::~Application() {
    quit();
  }

  void Application::run() const {
    while (running) {
      pollInputs();

      for (const auto &layer: layers)
        layer->render();

      // window->updateBuffer();

      const double currentTime = static_cast<double>(SDL_GetTicks()) / 1000;
      deltaTime = currentTime - lastFrameTime;
      lastFrameTime = static_cast<double>(currentTime);
    }

    quit();
  }

  void Application::pollInputs() const {
    SDL_Event windowEvent;
    mouseDelta = glm::zero<glm::vec2>();
    isMouseMoving = false;

    while (SDL_PollEvent(&windowEvent)) {
      const auto pressedKey = static_cast<Inputs::KeyboardKey>(windowEvent.key.scancode);
      switch (windowEvent.type) {
        case SDL_EVENT_QUIT:
          running = false;
          break;

        case SDL_EVENT_KEY_DOWN:
          if (!pressedKeys.contains(pressedKey))
            pressedKeys[pressedKey] = windowEvent.key;
          break;

        case SDL_EVENT_KEY_UP:
          if (pressedKeys.contains(pressedKey))
            pressedKeys.erase(pressedKey);
          break;

        case SDL_EVENT_MOUSE_MOTION:
          isMouseMoving = true;
          mouseDelta = glm::vec2(windowEvent.motion.xrel, windowEvent.motion.yrel);
          break;

        default:
          break;
      }
    }
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
