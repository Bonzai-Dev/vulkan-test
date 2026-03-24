#pragma once
#include <cstdint>
#include <functional>
#include <map>
#include <queue>
#include <core/logger.hpp>

namespace Core::Events {
  enum class EventType {
    None = 0,
    QuitApplication,
    MouseMotion,
    WindowResized, WindowShown, WindowHidden, WindowMouseEnter, WindowMouseLeave, WindowFocusGained, WindowFocusLost,
    WindowMinimized, WindowMaximized, WindowRestored, WindowClosed, WindowExposed
  };

  class Event {
    public:
      explicit Event(EventType type, const char *name) : type(type), name(name) {
      }

      const EventType type = EventType::None;
      bool handled = false;
      std::uint64_t time = 0;
      const char *name = "Event";
  };

  template<typename EventT>
  requires(std::is_base_of_v<Event, EventT>)
  using EventListener = std::function<void(EventT &)>;

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
