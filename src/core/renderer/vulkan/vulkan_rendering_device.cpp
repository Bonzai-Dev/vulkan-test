#include <map>
#include <SDL3/SDL_vulkan.h>
#include <core/application/application.hpp>
#include <core/application/logger.hpp>
#include "vulkan_rendering_device.hpp"
#include "vulkan_window.hpp"

using namespace Core::Events;

namespace Core::Graphics {
  VulkanRenderingDevice::VulkanRenderingDevice(const char *appName, const DisplayInfo &displayInfo) : appName(appName),
  displayInfo(displayInfo) {
    EventDispatcher::listen<WindowShown>([&](const WindowShown &event) { onWindowShow(event); });
    EventDispatcher::listen<WindowHidden>([&](const WindowHidden &event) { onWindowHide(event); });
    EventDispatcher::listen<WindowResized>([&](const WindowResized &event) { onWindowResize(event); });

    EventDispatcher::listen<WindowMouseEnter>([&](const WindowMouseEnter &event) { onWindowMouseEnter(event); });
    EventDispatcher::listen<WindowMouseLeave>([&](const WindowMouseLeave &event) { onWindowMouseLeave(event); });
    EventDispatcher::listen<WindowMouseMotion>([&](const WindowMouseMotion &event) { onWindowMouseMotion(event); });

    EventDispatcher::listen<WindowFocusGained>([&](const WindowFocusGained &event) { onWindowFocusGained(event); });
    EventDispatcher::listen<WindowFocusLost>([&](const WindowFocusLost &event) { onWindowFocusLost(event); });
    EventDispatcher::listen<WindowMinimized>([&](const WindowMinimized &event) { onWindowMinimized(event); });
    EventDispatcher::listen<WindowMaximized>([&](const WindowMaximized &event) { onWindowMaximized(event); });
    EventDispatcher::listen<WindowRestored>([&](const WindowRestored &event) { onWindowRestored(event); });
    EventDispatcher::listen<WindowClosed>([&](const WindowClosed &event) { onWindowClose(event); });
    EventDispatcher::listen<WindowExposed>([&](const WindowExposed &event) { onWindowExposed(event); });

    EventDispatcher::listen<KeyPressedEvent>([&](const KeyPressedEvent &event) { onKeyPressed(event); });
    EventDispatcher::listen<KeyReleasedEvent>([&](const KeyReleasedEvent &event) { onKeyReleased(event); });

    vkb::InstanceBuilder instanceBuilder;
    auto instanceResult = instanceBuilder.set_app_name(appName)
    .request_validation_layers(true)
    .use_default_debug_messenger()
    .require_api_version(1, 3, 0)
    .build();

    instance = instanceResult.value().instance;
    debugMessenger = instanceResult.value().debug_messenger;
  }

  VulkanRenderingDevice::~VulkanRenderingDevice() {

  }

  void VulkanRenderingDevice::render() {

  }

  void VulkanRenderingDevice::createWindow(const WindowOptions &options) {

  }

  VkFormat VulkanRenderingDevice::convertPixelFormat(PixelFormat format) {
    VkFormat converted = VK_FORMAT_UNDEFINED;
    switch (format) {
      case PixelFormat::R8G8B8A8:
        converted = VK_FORMAT_R8G8B8A8_UNORM;
        break;
      case PixelFormat::B8G8R8A8:
        converted = VK_FORMAT_B8G8R8A8_UNORM;
        break;
      default:
        break;
    }

    return converted;
  }

  std::string VulkanRenderingDevice::vulkanResultToString(VkResult result) {
    switch (result) {
      case VK_SUCCESS: return "VK_SUCCESS";
      case VK_NOT_READY: return "VK_NOT_READY";
      case VK_TIMEOUT: return "VK_TIMEOUT";
      case VK_EVENT_SET: return "VK_EVENT_SET";
      case VK_EVENT_RESET: return "VK_EVENT_RESET";
      case VK_INCOMPLETE: return "VK_INCOMPLETE";
      case VK_ERROR_OUT_OF_HOST_MEMORY: return "VK_ERROR_OUT_OF_HOST_MEMORY";
      case VK_ERROR_OUT_OF_DEVICE_MEMORY: return "VK_ERROR_OUT_OF_DEVICE_MEMORY";
      case VK_ERROR_INITIALIZATION_FAILED: return "VK_ERROR_INITIALIZATION_FAILED";
      case VK_ERROR_DEVICE_LOST: return "VK_ERROR_DEVICE_LOST";
      case VK_ERROR_MEMORY_MAP_FAILED: return "VK_ERROR_MEMORY_MAP_FAILED";
      case VK_ERROR_LAYER_NOT_PRESENT: return "VK_ERROR_LAYER_NOT_PRESENT";
      case VK_ERROR_EXTENSION_NOT_PRESENT: return "VK_ERROR_EXTENSION_NOT_PRESENT";
      case VK_ERROR_FEATURE_NOT_PRESENT: return "VK_ERROR_FEATURE_NOT_PRESENT";
      case VK_ERROR_INCOMPATIBLE_DRIVER: return "VK_ERROR_INCOMPATIBLE_DRIVER";
      case VK_ERROR_TOO_MANY_OBJECTS: return "VK_ERROR_TOO_MANY_OBJECTS";
      case VK_ERROR_FORMAT_NOT_SUPPORTED: return "VK_ERROR_FORMAT_NOT_SUPPORTED";
      case VK_ERROR_FRAGMENTED_POOL: return "VK_ERROR_FRAGMENTED_POOL";
      case VK_ERROR_OUT_OF_POOL_MEMORY: return "VK_ERROR_OUT_OF_POOL_MEMORY";
      case VK_ERROR_INVALID_EXTERNAL_HANDLE: return "VK_ERROR_INVALID_EXTERNAL_HANDLE";
      case VK_ERROR_SURFACE_LOST_KHR: return "VK_ERROR_SURFACE_LOST_KHR";
      case VK_ERROR_NATIVE_WINDOW_IN_USE_KHR: return "VK_ERROR_NATIVE_WINDOW_IN_USE_KHR";
      case VK_SUBOPTIMAL_KHR: return "VK_SUBOPTIMAL_KHR";
      case VK_ERROR_OUT_OF_DATE_KHR: return "VK_ERROR_OUT_OF_DATE_KHR";
      case VK_ERROR_INCOMPATIBLE_DISPLAY_KHR: return "VK_ERROR_INCOMPATIBLE_DISPLAY_KHR";
      case VK_ERROR_VALIDATION_FAILED_EXT: return "VK_ERROR_VALIDATION_FAILED_EXT";
      case VK_ERROR_INVALID_SHADER_NV: return "VK_ERROR_INVALID_SHADER_NV";
      case VK_ERROR_NOT_PERMITTED_EXT: return "VK_ERROR_NOT_PERMITTED_EXT";
      default:
        return "Unknown VkResult: " + std::to_string(result);
    }
  }
}
