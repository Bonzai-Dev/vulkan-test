#pragma once
#include <cstdint>
#include <functional>
#include <queue>

namespace Core::Events {
  enum class EventType {
    None = 0,
    QuitApplication,
    KeyPressed, KeyReleased,
    MouseButtonPressed, MouseButtonReleased, MouseMoved, MouseScrolled
  };

  class Event {
    public:
      explicit Event(EventType type): type(type) {
      }

      const EventType type = EventType::None;
      bool handled = false;
      std::uint64_t time = 0;
  };

  template<typename EventT>
  requires(std::is_base_of_v<Event, EventT>)
  using EventListener = std::function<bool(EventT&)>;

  class EventDispatcher {
    public:
      template<typename EventT>
      requires(std::is_base_of_v<Event, EventT>)
      void listen(const EventListener<EventT> &listener) {
        listeners[EventT().type] = [listener](Event &event) {
          return listener(static_cast<EventT&>(event));
        };
      }

      void queue(const Event &event) {
        eventQueue.push(event);
      }

    private:
      static inline std::queue<Event> eventQueue;
      static inline std::unordered_map<EventType, EventListener<Event>> listeners;
  };
}
