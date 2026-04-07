#include <core/application.hpp>
#include "app_layer.hpp"

int main() {
  const Core::Application application("Vulkan Template");
  application.createWindow({
    .mouseLocked = false,
    .fullScreen = false,
    .vsync = true,
    .resizable = true,
    .windowName = "Game"
  });

  application.addLayer<AppLayer>();
  application.run();

  return 0;
}
