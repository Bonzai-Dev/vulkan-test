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
      explicit Event(EventType type, const char *name): name(name), type(type) {
      }

      virtual ~Event() = default;

      const EventType type;
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
        listeners[typeid(EventT)].push([listener](Event &event) {
          return listener(static_cast<EventT&>(event));
        });
      }

      template<typename EventT>
      requires(std::is_base_of_v<Event, EventT>)
      void queue(const EventT &event) const {
        eventTypeQueue.emplace(typeid(EventT));
        eventQueue.push(event);
      }

      void process() {
        while (!eventTypeQueue.empty()) {
          Event &event = eventQueue.front();
          std::queue<EventListener<Event>> &eventListeners = listeners[eventTypeQueue.front()];
          while (!eventListeners.empty()) {
            const EventListener<Event> &eventListener = eventListeners.front();
            eventListener(event);
          }

          event.handled = true;
          eventTypeQueue.pop();
          eventQueue.pop();
        }
      }

    private:
      static inline std::queue<Event> eventQueue;
      static inline std::queue<std::type_index> eventTypeQueue;
      static inline std::unordered_map<std::type_index, std::queue<EventListener<Event>>> listeners;
  };
}