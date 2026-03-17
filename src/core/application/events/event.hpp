#pragma once
#include <functional>
#include <queue>

namespace Core {
  enum class EventType {
    None = 0,
    KeyPressed,
    KeyReleased,
    MouseButtonPressed,
    MouseButtonReleased,
    MouseMoved,
    MouseScrolled
  };

  class Event {
    public:
      Event(EventType type, int time);

      const EventType type;

      const int time;

    protected:
      bool handled = false;
  };

  class EventDispatcher {
    public:
      static void queueEvent();

      

    private:
      static inline std::queue<Event> eventQueue;
  };
}