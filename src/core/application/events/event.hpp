#pragma once
#include <cstdint>
#include <functional>
#include <map>
#include <queue>
#include <core/application/logger.hpp>

namespace Core::Events {
  enum class EventType {
    None = 0,
    QuitApplication,
    KeyPressed, KeyReleased,
    MouseButtonPressed, MouseButtonReleased, MouseMoved, MouseScrolled
  };

  class Event {
    public:
      explicit Event(EventType type) : type(type) {
      }

      const EventType type = EventType::None;
      bool handled = false;
      std::uint64_t time = 0;
  };

  template<typename EventT>
  requires(std::is_base_of_v<Event, EventT>)
  using EventListener = std::function<bool(EventT &)>;

  class EventDispatcher {
    public:
      template<typename EventT>
      requires(std::is_base_of_v<Event, EventT>)
      void listen(const EventListener<EventT> &listener) const {
        listeners[EventT().type].push([listener](Event &event) {
          return listener(static_cast<EventT &>(event));
        });
      }

      void queue(const Event &event) const {
        eventQueue.push(event);
      }

      void process() {
        while (!eventQueue.empty()) {
          Event &event = eventQueue.front();
          while (!listeners[event.type].empty()) {
            const EventListener<Event> &eventListener = listeners[event.type].front();
            eventListener(event);
            listeners[event.type].pop();
          }

          event.handled = true;
          eventQueue.pop();
        }
      }

    private:
      static inline std::queue<Event> eventQueue;
      static inline std::unordered_map<EventType, std::queue<EventListener<Event>>> listeners;
  };
}
