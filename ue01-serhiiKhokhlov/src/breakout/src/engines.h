#pragma once
#include <list>
#include <memory>
#include <cmath>
//#include "C:\Users\barsm\Desktop\SWO4\ue01-serhiiKhokhlov-main\src\breakout\src\event_listener.h"
#include "simple_game_object.h"

struct Position {
	int x;
	int y;

	bool operator==(const Position& other) const {
		return x == other.x && y == other.y;
	}
};

template <>
struct std::hash<Position> {
	std::size_t operator()(const Position& p) const noexcept {
		return std::hash<unsigned int>()(p.x) ^ (std::hash<unsigned int>()(p.y) << 1);
	}
};

class Engine { // abstract class
protected:
	std::shared_ptr<sf::Time> deltaTime;
	std::list<std::shared_ptr<SimpleGameObject>> game_objects{};

public:
	Engine() {}
	explicit Engine(std::list<std::shared_ptr<SimpleGameObject>> game_objects) : game_objects(game_objects) {}

	void addGameObject(std::shared_ptr<SimpleGameObject> go) {
		game_objects.push_back(go);
	}

	void addGameObjects(std::list<std::shared_ptr<SimpleGameObject>> go_list) {
		game_objects.insert(game_objects.end(), go_list.begin(), go_list.end());
	}

	std::list<std::shared_ptr<SimpleGameObject>> getGameObjects() const {
		return game_objects;
	}

	virtual void start (){}

	virtual void update() = 0;

	void setDeltaTime(std::shared_ptr<sf::Time> dt) {
		deltaTime = dt;
	}

	virtual ~Engine() = default;
};

class SimplePhysicsEngine : public Engine {
protected:
public:

	SimplePhysicsEngine() {}
	SimplePhysicsEngine(std::list<std::shared_ptr<SimpleGameObject>> game_objects) : Engine(game_objects) {}

	virtual void update() override {
		for (auto& go : game_objects) {
			// update physics
			if (go->getRigidBody() && go->getRigidBody()->m_velocity != sf::Vector2f{ 0.0f, 0.0f }) {
				//std::cout << "Moving " << go->getLabel() << " with velocity (" << go->getRigidBody()->m_velocity.x << ", " << go->getRigidBody()->m_velocity.y << ")\n";
				//std::cout << "delta time: " << deltaTime->asSeconds() << std::endl;
				go->move(go->getRigidBody()->m_velocity * deltaTime->asSeconds());
			//	std::cout << "New position of " << go->getLabel() << " (" << go->getPosition().x << ", " << go->getPosition().y << ")\n";
			}
		}
	}
};

class SimpleCollisionEngine : public Engine {
private:
	float const gcell_size = 50.0f;
	using GridCell = std::list<SimpleGameObject*>;

	std::unordered_map<Position, GridCell> grid{};

	Position getCell(sf::Vector2f const& pos) {
		return Position{
			int(pos.x / gcell_size),
			int(pos.y / gcell_size)
		};
	}

	std::list <Position> findMatchingCells(sf::FloatRect const& bounds) {
		std::list<Position> cells;
		Position start = getCell(bounds.position);
		Position end = getCell(bounds.position + bounds.size);
		for (int x = start.x; x <= end.x; x++) {
			for (int y = start.y; y <= end.y; y++) {
				cells.push_back({ x, y });
			}
		}
		return cells;
	}

protected:
	sf::RenderTarget const *  m_target;

public:
	explicit SimpleCollisionEngine(std::list<std::shared_ptr<SimpleGameObject>> game_objects, sf::RenderTarget const* target) : Engine(game_objects), m_target(target) {
		// create grid
		sf::Vector2f target_size = m_target->getView().getSize();

//		unsigned int x_cells = unsigned int(ceil(target_size.x / gcell_size));
//		unsigned int y_cells = unsigned int(ceil(target_size.y / gcell_size));

		//grid.resize(x_cells);
		/*for (auto& row : grid) {
			row.resize(y_cells);
		}*/

		// fill grid
		for (auto & go : game_objects) {
			if (go->getCollider()) {
				Position pos = getCell(go->getPosition());
				//std::cout << "adding collider of " << go->getLabel() << " on position (" << pos.x << ", " << pos.y << ")\n";
				
				auto bounds = go->getCollider()->getBounds();

				//find the cells the object is in
				for (auto cell : findMatchingCells(bounds)) {
					grid[cell].push_back(go.get());
				}

				/*auto it = grid.find(pos);
				if (it != grid.end())
					it->second.push_back(go.get());	// vector prbly not appropriate cuz negative coordinates to map*/
			}
		}
	}

	virtual void update() override {	///TODO
		// check moved objects for collisions
		for (auto & go : game_objects) {
			//Position old_pos = getCell(go->getPrevTransform().position);
			//Position new_pos = getCell(go->getPosition());

			//std::cout << "current position of "<< go->getLabel()<<  " (" << go->getPosition().x << ", " << go->getPosition().y << ")\n";

			if (go->getCollider() && go->getCollider()->m_is_updated) {

				if (go->getCollider()->m_lock_in_the_screen) {
					auto bounds = go->getCollider()->getBounds();
					if (bounds.position.y <= -float(m_target->getView().getSize().y) / 2.0f || (bounds.position.y + bounds.size.y) >= float(m_target->getView().getSize().y) / 2.0f) {
						go->handleEvent(WallCollisionEvent(WallCollisionEvent::WallType::horizontal));
					}
					if (bounds.position.x <= -float(m_target->getView().getSize().x) / 2.0f || (bounds.position.x + bounds.size.x) >= float(m_target->getView().getSize().x) / 2.0f) {
						go->handleEvent(WallCollisionEvent(WallCollisionEvent::WallType::vertical));
					}
				}

				auto old_positions = findMatchingCells(go->getPrevCollider()->getBounds());
				auto new_positions = findMatchingCells(go->getCollider()->getBounds());

					for (auto cell : old_positions) {
						grid[cell].remove(go.get());
					}
					for (auto cell : new_positions) {
						std::cout << "adding collider of " << go->getLabel() << " on position (" << cell.x << ", " << cell.y << ")\n";
						grid[cell].push_back(go.get());
					}

				//find the cell where the object is (before updated)
				/*grid[old_pos].remove(go.get());
				std::cout << "removing collider of " << go->getLabel() << " on position (" << old_pos.x << ", " << old_pos.y << ")\n";
				//insert into the cell where the object is (after updated)
				grid[new_pos].push_back(go.get());		///TODO (overflow on push_back)
				std::cout << "adding collider of " << go->getLabel() << " on position (" << new_pos.x << ", " << new_pos.y << ")\n";*/


				go->getCollider()->m_is_updated = false;


				// check for collisions inside the cells
				for (auto cell : new_positions) {
					bool found_collision = false;
					for (auto other : grid[cell]) {
						if (go.get() != other && other->getCollider()) {
							if (go->getCollider()->collides_with(*other->getCollider())) {
								std::cout << go->getLabel() << " collided with " << other->getLabel() << std::endl;

								// handle collision
								go->handleEvent(CollisionEvent(other));
								other->handleEvent(CollisionEvent(go.get()));

								found_collision = true;
								break;
							}
						}
					}
					if (found_collision) break;
				}
			}
		}
	}

	virtual ~SimpleCollisionEngine() = default;
};