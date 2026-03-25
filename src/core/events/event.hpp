#pragma once
#include <cstdint>
#include <functional>
#include <queue>
#include <typeindex>
#include <unordered_map>
#include <core/logger.hpp>

namespace Core::Events {
  enum class EventType {
    None = 0,
    QuitApplication,

    MouseMotion,

    WindowResized,
    WindowShown,
    WindowHidden,
    WindowMouseEnter,
    WindowMouseLeave,
    WindowFocusGained,
    WindowFocusLost,
    WindowMinimized,
    WindowMaximized,
    WindowRestored,
    WindowClosed,
    WindowExposed,
  };

  class Event {
    public:
      explicit Event(const char *name): name(name) {
      }

      virtual ~Event() = default;

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
        listeners[EventT::getType()].push([listener](Event &event) {
          return listener(static_cast<EventT&>(event));
        });
      }

      template<typename EventT>
      requires(std::is_base_of_v<Event, EventT>)
      void queue(const EventT &event) const {
        eventQueue.push(&event);

      }

      void process() {
        while (!eventQueue.empty()) {
          Event &event = eventQueue.front();
          std::queue<EventListener<Event>> &eventListeners = listeners[event.getType()];
          while (!eventListeners.empty()) {
            const EventListener<Event> &eventListener = eventListeners.front();
            eventListener(event);
            eventListeners.pop();
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