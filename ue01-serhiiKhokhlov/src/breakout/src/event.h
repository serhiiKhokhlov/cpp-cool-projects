#pragma once
#include <memory>
#include "simple_game_object.h"

class SimpleGameObject;

class Event {
public:
    Event() {}
    virtual ~Event() = default;
};

class CollisionEvent : public Event {
public:
    SimpleGameObject* other;
    CollisionEvent(SimpleGameObject * other) : other(other) {}
};

class ButtonPressEvent : public Event {
public:
    sf::Keyboard::Scan button_id;
    ButtonPressEvent(sf::Keyboard::Scan id) : button_id(id) {}
};

class WallCollisionEvent : public Event {
public:

	enum class WallType {
		vertical,
		horizontal
	};

	WallType wall_type;
	WallCollisionEvent(WallType wall_type) : wall_type{ wall_type } {}
};

class StartEvent : public Event {
public:
	StartEvent() {}
};

class GameStartPendingEvent : public Event {
public:
	GameStartPendingEvent() {}
};