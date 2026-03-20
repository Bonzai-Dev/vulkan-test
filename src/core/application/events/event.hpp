#pragma once
#include <cstdint>
#include <functional>
#include <queue>
#include <typeindex>

#include "core/application/logger.hpp"
#include "glm/gtc/constants.hpp"

namespace Core::Events {
  enum class EventType {
    None = 0,
    QuitApplication,
    KeyPressed, KeyReleased,
    MouseButtonPressed, MouseButtonReleased, MouseMoved, MouseScrolled
  };

  class Event {
    public:
      explicit Event(EventType type):
      type(type) {}

      const EventType type = EventType::None;
      bool handled = false;
      std::uint64_t time = 0;
  };

  template<typename EventT>
  requires(std::is_base_of_v<Event, EventT>)
  using EventListener = std::function<bool(EventT&)>;
  //
  // template<typename EventT>
  // requires(std::is_base_of_v<Event, EventT>)
  // class EventDispatcher {
  //   public:
  //     EventDispatcher() {
  //
  //     }
  //
  //   private:
  //     static inline std::unordered_map<std::type_index, std::vector<EventListener<Event>>> listeners;
  // };

  class EventDispatcher {
    public:
      template<typename EventT>
      requires(std::is_base_of_v<Event, EventT>)
      void listen(const EventListener<EventT> &listener) {
        listeners[typeid(EventT)].push_back([listener](Event &event) -> bool {
          return listener(static_cast<EventT&>(event));
        });
      }

      // template<typename T>
        //  void queue(T&& msg) {
        // queuedDispatches.push([m = std::forward<T>(msg)](Dispatcher& dispatcher) {
        //     dispatcher.dispatch(m);
        // });
      // }

      void process() {
        while (!eventsQueue.empty()) {
          Event &event = eventsQueue.front();
          const std::vector<EventListener<Event>> &eventListeners = listeners[typeid(event)];
          for (const auto &eventListener : eventListeners) {
            eventListener(event);
            LOG_CORE_INFO("process");
          }

          event.handled = true;
          eventsQueue.pop();
        }
      }

    private:
      static inline std::queue<Event> eventsQueue;
      static inline std::unordered_map<std::type_index, std::vector<EventListener<Event>>> listeners;
  };
}
