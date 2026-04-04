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

  class EventDispatch
  {
    virtual ~EventDispatch() {}
    virtual void FooEvent(int) =0;
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

  // template<typename EventT>
  // requires(std::is_base_of_v<Event, EventT>)
  // using EventCallback = std::function<void(EventT &)>;
  //
  // template<typename EventT>
  // requires(std::is_base_of_v<Event, EventT>)
  // class EventSubsystem {
  //   protected:
  //     EventSubsystem() = default;
  //
  //     static inline std::unordered_map<std::type_index, std::queue<EventCallback<Event>>> listeners;
  //     static inline std::queue<EventT> eventQueue;
  // };
  //
  // template<typename EventT>
  // requires(std::is_base_of_v<Event, EventT>)
  // class EventListener: public EventSubsystem<EventT> {
  //   public:
  //     EventListener(const EventCallback<EventT> &listener) {
  //       this->listeners[std::type_index(typeid(EventT))].push([listener](Event &event) {
  //          return listener(static_cast<EventT &>(event));
  //        });
  //     }
  // };
  //
  // template<typename EventT>
  // requires(std::is_base_of_v<Event, EventT>)
  // class EventDispatcher: public EventSubsystem<EventT> {
  //   public:
  //     explicit EventDispatcher(const EventT &event) {
  //       this->eventQueue.push(event);
  //     }
  //
  //     static void process() {
  //       while (!EventSubsystem<EventT>::eventQueue.empty()) {
  //         Event &event = EventSubsystem<EventT>::eventQueue.front();
  //         std::queue<EventCallback<Event>> &eventListeners = EventSubsystem<EventT>::listeners[std::type_index(typeid(event))];
  //         for (size_t listenerIndex = 0; listenerIndex < eventListeners.size(); listenerIndex++) {
  //           const EventCallback<Event> &eventListener = eventListeners.front();
  //           eventListener(event);
  //         }
  //
  //         event.handled = true;
  //         EventSubsystem<EventT>::eventQueue.pop();
  //       }
  //     }
  // };
}