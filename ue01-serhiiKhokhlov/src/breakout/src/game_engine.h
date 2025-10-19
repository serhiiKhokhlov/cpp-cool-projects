#pragma once
#include <list>
#include <memory>
#include <cmath>
#include <iostream>
#include "engines.h"

class GameEngine : public Engine {
protected:

	const sf::View world_view{ {0, 0}, {1000, 1000} };
	const int d1_velocity = 400;
	sf::ContextSettings settings;
	sf::RenderWindow window;
	sf::Clock clock;

	void center_view(sf::Vector2u new_size) {
		float ratio = (float)new_size.x / (float)new_size.y;
		if (ratio >= 1.0)
			window.setView(sf::View{ world_view.getCenter(), {world_view.getSize().x * ratio, world_view.getSize().y} });
		else
			window.setView(sf::View{ world_view.getCenter(), {world_view.getSize().x, world_view.getSize().y / ratio} });
	}

	std::list <std::unique_ptr<Engine>> engines;

public:
	GameEngine() {
		settings.antiAliasingLevel = 8; // Request 8x antialiasing
		window.create(sf::VideoMode({ 1000, 800 }), "GameEngine", sf::Style::Default, sf::State::Windowed, settings);
		window.setView(world_view);
		window.setSize({ static_cast<unsigned int>(world_view.getSize().x), static_cast<unsigned int>(world_view.getSize().y) });
		center_view(window.getSize());
		sf::Clock clock;
		deltaTime = std::make_shared<sf::Time>();
	}
    GameEngine(std::list<std::shared_ptr<SimpleGameObject>> game_objects) : Engine(game_objects) {
		settings.antiAliasingLevel = 8; // Request 8x antialiasing
		window.create(sf::VideoMode({ 1000, 800 }), "GameEngine", sf::Style::Default, sf::State::Windowed,  settings);
		window.setView(world_view);
		window.setSize({ static_cast<unsigned int>(world_view.getSize().x), static_cast<unsigned int>(world_view.getSize().y) });
		center_view(window.getSize());
		sf::Clock clock;
		deltaTime = std::make_shared<sf::Time>();
    }

	sf::RenderWindow const & getWindow() const {
		return window;
	}

	void addEngine(std::unique_ptr<Engine> engine) {
		if (engine->getGameObjects().empty())	//if the engine has no game objects, add them from the game engine
			engine->addGameObjects(game_objects);
		else {
			if (engine->getGameObjects() != game_objects) {
				std::cout << "Game objects are not the same" << std::endl;

				for (auto& go : engine->getGameObjects()) {
					if (std::find(game_objects.begin(), game_objects.end(), go) == game_objects.end()) {
						game_objects.push_back(go);
					}
				}
			}
		}

		engine->setDeltaTime(deltaTime);
		engines.push_back(std::move(engine));
	}

	virtual void start() {

		for (auto& engine : engines) {
			engine->start();
		}

		while (window.isOpen()) {
			// Event processing
			while (const std::optional event = window.pollEvent())
			{
				if (event->is<sf::Event::Closed>()) {
					window.close();
				}
				if (const auto* redraw_event = event->getIf<sf::Event::Resized>()) {
					center_view(redraw_event->size);
				}
			}

			window.clear();
			update();

			
			for (auto& go : game_objects) {
				if (go->m_to_be_rendered)
					go->render(window);
			}
			window.display();
		}

	}

	virtual void update() override {
		*deltaTime = clock.restart();
		for (auto & engine : engines) {
			engine->update();
		}
	}

	virtual ~GameEngine() = default;

};