#pragma once
#include <unordered_map>
#include "engines.h"

enum class KeyState {
	Idle,
	KeyDown,
	KeyUp
};

enum class ControllType {
	MoveUp,
	MoveDown,
	MoveLeft,
	MoveRight,
	Idle
};


class ControllObjectEvent : public Event {
public:
	ControllType ctrl_type;
	ControllObjectEvent(ControllType ctrl_type) : ctrl_type(ctrl_type) {}
};

class Controller : public Engine {
public:

protected:
	std::unordered_map<sf::Keyboard::Scan, ControllType> key_map;
	std::vector <sf::Keyboard::Scan> registered_keys;

public:
	Controller() {}
	Controller(std::list<std::shared_ptr<SimpleGameObject>> game_objects) : Engine(game_objects) {}

	void assignButton(sf::Keyboard::Scan key_btn, ControllType ctrl) {
		registered_keys.push_back(key_btn);
		key_map[key_btn] = ctrl;
	}

	virtual void update() override {
		int pressed_keys = 0;
		for (auto key : registered_keys) {
			if (sf::Keyboard::isKeyPressed(key)) {
				// Event processing
				//std::cout << "Key pressed: " << std::endl;

				for (auto& go : game_objects) {
				//	std::cout << "Sending event to " << go->getLabel() << std::endl;
					go->handleEvent(ControllObjectEvent(key_map.find(key)->second));
				}
				pressed_keys++;
			}
		}
		if (pressed_keys == 0) {
			for (auto& go : game_objects) {
				go->handleEvent(ControllObjectEvent(ControllType::Idle));
			}
		}
	}
};
