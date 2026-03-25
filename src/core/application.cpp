#include <SDL3/SDL.h>
#include <core/logger.hpp>
#include "events/application.hpp"
#include "application.hpp"

namespace Core {
  Application::Application(const char *name) : name(name) {
    eventDispatcher.listen<Events::ApplicationQuit>([&](const Events::ApplicationQuit &event) {
      quit();
      LOG_CORE_CRITICAL("QUIT");
    });
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

      eventDispatcher.process();
      windowManager.update();
    }
  }

  Graphics::Backend Application::selectGraphicsBackend() const {
    // TODO: Implement graphics backend selection based on platform and availability
    const Graphics::Backend backend = Graphics::Backend::Vulkan;
    return backend;
  }

  void Application::quit() const {
    if (!running)
      return;

    running = false;
    SDL_Quit();
  }
}
