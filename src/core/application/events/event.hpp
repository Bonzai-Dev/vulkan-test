#pragma once
#include <functional>
#include <queue>

namespace Core::Events {
  template<typename EventT>
  using EventListener = std::function<bool(EventT&)>;

  enum class EventType {
    None = 0,
    QuitApplication,
    KeyPressed, KeyReleased,
    MouseButtonPressed, MouseButtonReleased, MouseMoved, MouseScrolled
  };

  class Event {
    public:
      explicit Event(EventType type): type(type) {};

      const EventType type = EventType::None;
      bool handled = false;
      uint64_t time = 0;
  };

  class EventDispatcher {
    public:
      template<typename EventT>
      requires(std::is_base_of_v<Event, EventT>)
      void post(const EventListener<EventT> listener, EventT& event) {
        eventQueue.push(event);
      }

      void processEvents() {
        while (!eventQueue.empty()) {
          Event &event = eventQueue.front();
          event.handled = true;
          eventQueue.pop();
        }
      }

    private:
      static inline std::queue<Event> eventQueue;
  };
}