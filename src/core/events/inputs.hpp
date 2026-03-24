#pragma once
#include "event.hpp"

namespace Core::Events {
  class MouseMotion: public Event {
    public:
      MouseMotion(float deltaX, float deltaY, float positionX, float positionY) :
      Event(EventType::MouseMotion, "MouseMotion"),
      x(positionX), y(positionY), deltaX(deltaX), deltaY(deltaY) {
      }

      const float x = 0;
      const float y = 0;

      const float deltaX = 0;
      const float deltaY = 0;
  };
}