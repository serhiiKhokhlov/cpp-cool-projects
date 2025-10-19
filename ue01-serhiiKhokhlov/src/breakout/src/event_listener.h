#pragma once
#include <unordered_map>
#include <functional>
#include <typeindex>
#include "event.h"


class IEventListener {
protected:
    std::unordered_multimap<std::type_index, std::function<void(const Event&)>> event_handlers;

public:
    template <typename EventType>
    void registerEventHandler(std::function<void(const EventType&)> handler) {
        event_handlers.emplace(typeid(EventType), [handler](const Event& event) {
                handler(static_cast<const EventType&>(event));
            });
    }

    void handleEvent(const Event& event) {
        auto it = event_handlers.find(typeid(event));
		for (; it != event_handlers.end() && it->first == typeid(event); ++it) {
			it->second(event);
		}
    }

    virtual ~IEventListener() = default;
};
