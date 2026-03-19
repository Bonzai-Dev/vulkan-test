#pragma once
#include <functional>
#include <queue>
#include <typeindex>

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
      type(type) {};

      const EventType type = EventType::None;
      bool handled = false;
      uint64_t time = 0;
  };

  template<typename EventT>
  requires(std::is_base_of_v<Event, EventT>)
  using EventListener = std::function<bool(EventT&)>;

  class EventDispatcher {
    public:
      template<typename EventT>
      requires(std::is_base_of_v<Event, EventT>)
      void listen(const EventListener<EventT> &listener) {
        listeners[typeid(EventT)] = [listener]() {
          listener();
        };
      }

      void process() {
        // while (!eventsQueue.empty()) {
        //   std::function<void(Dispatcher&)> queuedDispatch = std::move(queuedDispatches.front());
        //   queuedDispatches.pop();
        //   queuedDispatch(*this);
        // }
      }

    private:
      std::queue<EventListener<Event>> eventsQueue;
      std::unordered_map<std::type_index, EventListener<Event>> listeners;
  };
}
