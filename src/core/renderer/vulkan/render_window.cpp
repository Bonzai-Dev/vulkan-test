#include <string>
#include <stdexcept>
#include <SDL3/SDL_vulkan.h>
#include "volk.h"
#include "render_window.hpp"

namespace Core::Graphics {
  VulkanRenderWindow::VulkanRenderWindow(
    const char *windowName,
    bool mouseLocked,
    bool fullScreen,
    unsigned int width,
    unsigned int height
  ) :
  RenderWindow(windowName, mouseLocked, fullScreen, width, height) {
    if (!SDL_Vulkan_CreateSurface(window, vulkan.getInstance(), nullptr, &surface))
      throw std::runtime_error("Failed to create window surface: " + std::string(SDL_GetError()));
  }

  VulkanRenderWindow::~VulkanRenderWindow() {
    vkDestroySurfaceKHR(vulkan.getInstance(), surface, nullptr);
  }
}
