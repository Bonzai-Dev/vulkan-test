#pragma once
#include <functional>
#include <vector>

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

    private:

  };

  class EventDispatcher {
    public:
      EventDispatcher();

    private:
      std::vector<Event> eventQueue;
  };
}