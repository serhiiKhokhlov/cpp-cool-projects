#include <iostream>
#include <list>
#include <fstream>
#include <memory>
#include <filesystem>
#include "game_engine.h"
#include "custom-classes/breakout_game_objs.h"

//game configuration (can be set in the config.txt file) parameters
int rows{5};
int cols{7};
float margin{5.0f};
float side_margin{50.0f};
float upper_margin{100.0f};
float to_screen_ratio{0.33f};
float ball_size{ 10.0f };
float ball_speed{ 500.0f };
float paddle_width{ 180.0f };
float paddle_speed{ 500.0f };

//read the configuration from the file
void readConfig() {
	std::ifstream file("./resources/config.txt"); //CMake doesnt really copy the resources folder to the build directory so the file is not found
												  //required manual copying to the build directory
	if (!file.is_open()) {
		std::cerr << "Could not open the config file" << std::endl;
		return;
	}
	std::string line;
	while (std::getline(file, line)) {
		std::istringstream iss(line);
		std::string key;
		if (iss >> key) {
			if (key == "rows") {
				iss >> rows;
			}
			else if (key == "cols") {
				iss >> cols;
			}
			else if (key == "margin") {
				iss >> margin;
			}
			else if (key == "side-margin") {
				iss >> side_margin;
			}
			else if (key == "upper-margin") {
				iss >> upper_margin;
			}
			else if (key == "to-screen-ratio") {
				iss >> to_screen_ratio;
			}
			else if (key == "ball-size") {
				iss >> ball_size;
			}
			else if (key == "ball-speed") {
				iss >> ball_speed;
			}
			else if (key == "paddle-width") {
				iss >> paddle_width;
			}
			else if (key == "paddle-speed") {
				iss >> paddle_speed;
			}
		}
	}
}

std::list<std::shared_ptr<Brick>> createBricks(int rows, int cols, float margin, float side_margin, float upper_margin, sf::RenderTarget const* rt, float to_screen_ratio) {
	std::list<std::shared_ptr<Brick>> bricks;
	auto window_width = rt->getView().getSize().x;
	auto window_height = rt->getView().getSize().y;

	float brick_width = (window_width - (cols + 1) * margin - 2 * side_margin) / cols;
	float brick_height = (window_height * to_screen_ratio - (rows + 1) * margin - upper_margin) / rows;
	float start_x = side_margin - (window_width / 2.0f);
	float start_y = upper_margin - (window_height / 2.0f);
	for (int row = 0; row < rows; ++row) {
		for (int col = 0; col < cols; ++col) {
			auto brick = std::make_shared<Brick>(brick_width, brick_height, sf::Color::Red);    
			brick->setPosition({ start_x + col * (brick_width + margin), start_y + row * (brick_height + margin) });
			bricks.push_back(brick);
		}
	}
	return bricks;
}

void printWorkingDirectory() {
	std::cout << "Current working directory: " << std::filesystem::current_path() << std::endl;
}


int main() {
	printWorkingDirectory();
	readConfig();
	std::list<std::shared_ptr<SimpleGameObject>> game_objects;
	sf::Clock clock;

	GameEngine game_engine{};
	// Create GameManager
	std::shared_ptr<GameManager> game_manager = std::make_shared<GameManager>();
	game_objects.push_back(game_manager);

	// Create Bricks
	auto bricks = createBricks(rows, cols, margin, side_margin, upper_margin, &(game_engine.getWindow()), to_screen_ratio);
	for (auto& brick : bricks) {
		game_manager->addBrick(brick);
		game_objects.push_back(brick);
	}
	// Add BrickDestructionObserver
	auto observer = std::make_unique<BrickDestructionObserver>(bricks, game_manager.get());

	// Create Paddle
	std::shared_ptr<Paddle> paddle = std::make_shared<Paddle>(paddle_speed, paddle_width, 20.0f);
	paddle->setPosition({ -paddle->getShape()->getGlobalBounds().size.x / 2.0f, 300.0f });
	//paddle->removeCollider();
	game_objects.push_back(paddle);
	game_manager->setPaddle(paddle);

	// Create Ball
	std::shared_ptr<Ball> ball = std::make_shared<Ball>(ball_speed, ball_size);
	ball->setAngle(60.0f / 180.0f * PI);
	ball->removeCollider();

	game_objects.push_back(ball);
	game_manager->setBall(ball);

	// Configure GameEngine;
	game_engine.addGameObjects(game_objects);
	game_engine.addEngine(std::move(observer));

	std::unique_ptr <SimplePhysicsEngine> physics_engine = std::make_unique<SimplePhysicsEngine>(game_objects);

	game_engine.addEngine(std::move(physics_engine));
	game_engine.addEngine(std::make_unique<SimpleCollisionEngine>(game_objects, &(game_engine.getWindow())));

	std::unique_ptr<Controller> controller = std::make_unique<Controller>(game_objects);
	controller->assignButton(sf::Keyboard::Scan::Up, ControllType::MoveUp);
	controller->assignButton(sf::Keyboard::Scan::Down, ControllType::MoveDown);
	controller->assignButton(sf::Keyboard::Scan::Left, ControllType::MoveLeft);
	controller->assignButton(sf::Keyboard::Scan::Right, ControllType::MoveRight);
	controller->assignButton(sf::Keyboard::Scan::W, ControllType::MoveUp);
	controller->assignButton(sf::Keyboard::Scan::A, ControllType::MoveLeft);
	controller->assignButton(sf::Keyboard::Scan::S, ControllType::MoveDown);
	controller->assignButton(sf::Keyboard::Scan::D, ControllType::MoveRight);
	game_engine.addEngine(std::move(controller));

	std::unique_ptr<GameStartObserver> game_starter = std::make_unique<GameStartObserver>(game_objects);
	game_manager->setGameStartObserver(game_starter.get());
	game_engine.addEngine(std::move(game_starter));
	
	game_engine.addEngine(std::make_unique<BallFallObserver>(ball, game_manager.get()));

 	game_engine.start();

 	return 0;
}
