#pragma once
#include <iostream>
#include "../simple_game_object.h"
#include "../controller.h"

float const PI = 3.14159265359f;

class Brick : public SimpleGameObject {
private:
	bool m_destroyed{ false };
public:
	Brick(float width, float height, sf::Color color) : SimpleGameObject("Brick") {
		setShape(std::make_unique<sf::RectangleShape>(sf::Vector2f{ width, height }));
		getShape()->setFillColor(color);
		addCollider();
		m_to_be_rendered = true;


		//respond on collision with ball
		registerEventHandler<CollisionEvent>([this](const CollisionEvent& event) {
		//	std::cout << getLabel() << " collided with " << event.other->getLabel() << std::endl;
			//destroy the brick
			destroy();
		});
	}

	void destroy() {
		m_destroyed = true;

		//remove the collider
		removeCollider();
		//remove the rigid body
		//remove the shape
		//setShape(nullptr);
		//set the object to be not rendered
		m_to_be_rendered = false;
	}

	bool isDestroyed() const {
		return m_destroyed;
	}

	void setDestroyed(bool destroyed) {
		m_destroyed = destroyed;
	}
};


class Paddle : public SimpleGameObject {
protected:
	float m_speed{ 0.0f };

public:
	Paddle(float speed, float width, float height) : SimpleGameObject("Paddle"), m_speed{speed} {
		setShape(std::make_unique<sf::RectangleShape>(sf::Vector2f{ width, height }));
		getShape()->setFillColor(sf::Color::White);
		addCollider();
		addRigidBody();
		m_to_be_rendered = true;
		getCollider()->m_lock_in_the_screen = true;

		registerEventHandler<ControllObjectEvent>([this, speed](const ControllObjectEvent& event) {

			//std::cout << "Paddle received event" << std::endl;

			switch (event.ctrl_type) {
			case ControllType::MoveLeft:
		//		std::cout << "Paddle moving left" << std::endl;
				getRigidBody()->m_velocity = { -speed, 0.0f };
				break;
			case ControllType::MoveRight:
				getRigidBody()->m_velocity = { speed, 0.0f };
				break;
			case ControllType::Idle:
				getRigidBody()->m_velocity = { 0.0f, 0.0f };
				break;
			}
		});

		registerEventHandler<WallCollisionEvent>([this](const WallCollisionEvent& event) {
			event;
			resetTransform();
		});
	}

	void setSpeed(float speed) {
		m_speed = speed;
	}

	float getSpeed() const {
		return m_speed;
	}
};

class Ball : public SimpleGameObject {
protected:
	float m_speed{ 0.0f };
	float m_angle{}; //angle in radians


public:
	Ball(float speed, float size) : SimpleGameObject("Ball"), m_speed{ speed } {
		setShape(std::make_unique<sf::CircleShape>(size));
		getShape()->setFillColor(sf::Color::White);
		addCollider();
		addRigidBody();
		m_to_be_rendered = true;
		getCollider()->m_lock_in_the_screen = true;

		//
		registerEventHandler<CollisionEvent>([this](const CollisionEvent& event) {
			if (!event.other)  //no collision
				return;

			//std::cout << getLabel() << " collided with " << event.other->getLabel() << std::endl;
						//specal behavior for the paddle
			if (dynamic_cast<Paddle*>(event.other) ) {
				//find the point of collision
				auto intersection = getCollider()->getBounds().findIntersection(event.other->getCollider()->getBounds());
				//determine the point of collision
				auto intersection_point_x = intersection.value().getCenter().x;
				auto paddle_left_x = event.other->getCollider()->getBounds().position.x;
				auto paddle_length = event.other->getCollider()->getBounds().size.x;

				//determine the angle of reflection based on the relative position of the intersection point to the center of the paddle
                //determine the angle of reflection based on the relative position of the intersection point to the center of the paddle

				auto collision_ratio = (intersection_point_x - paddle_left_x) / paddle_length;
				auto angle = 210.0f / 180.0f * PI + collision_ratio * 120.0f / 180.0f * PI;

				//set the new velocity
				getRigidBody()->m_velocity = sf::Vector2f{ m_speed * std::cos(angle), m_speed * std::sin(angle) };
			}
			//mirror the movement by default
			else {
				//reverse the movement
				//determine where to reflect the ball
				auto intersection = getCollider()->getBounds().findIntersection(event.other->getCollider()->getBounds());
				auto intersection_point = intersection.value().getCenter();
				auto ball_center = getCollider() -> getBounds().getCenter();

				sf::Vector2f centers_line = intersection_point - ball_center;
				//normalize the line (decide if its rather horizontal or vertical)
				//when line more vertical than horizontal then reflect the ball horizontally and vice versa
				if (std::abs(centers_line.x) > std::abs(centers_line.y)) {
					getRigidBody()->m_velocity = sf::Vector2f{ -getRigidBody()->m_velocity.x, getRigidBody()->m_velocity.y };
				}
				else {
					getRigidBody()->m_velocity = sf::Vector2f{ getRigidBody()->m_velocity.x, -getRigidBody()->m_velocity.y };
				}

				//std::cout << "Ball velocity is" << getRigidBody()->m_velocity.x << ", " << getRigidBody()->m_velocity.y << std::endl;
			}

			resetTransform();
		});

		registerEventHandler<WallCollisionEvent>([this](const WallCollisionEvent& event) {
			if (event.wall_type == WallCollisionEvent::WallType::horizontal) {
				getRigidBody()->m_velocity = sf::Vector2f{ getRigidBody()->m_velocity.x, -getRigidBody()->m_velocity.y };
			}
			else {
				getRigidBody()->m_velocity = sf::Vector2f{ -getRigidBody()->m_velocity.x, getRigidBody()->m_velocity.y };
			}

			resetTransform();
		});
	}

	void setAngle( float angle ) {
		m_angle = angle;
	}

	void setSpeed(float speed) {
		m_speed = speed;
	}

	float getSpeed() const {
		return m_speed;
	}

	sf::Vector2f computeVelocity() {
		return sf::Vector2f{ m_speed * std::cos(m_angle), m_speed * std::sin(m_angle) };
	}
};

class GameStartObserver : public Engine {
	sf::Keyboard::Scan const startKey{ sf::Keyboard::Scan::Space };


public:
	bool is_started{ false };
	GameStartObserver() {}
	GameStartObserver(std::list<std::shared_ptr<SimpleGameObject>> game_objects) : Engine(game_objects) {}

	virtual void update() override {
		if (is_started) return;
		for (auto& go : game_objects) {
			if (is_started = sf::Keyboard::isKeyPressed(startKey))
				go->handleEvent(StartEvent{});
			else go->handleEvent(GameStartPendingEvent{});
		}
	}
};

class GameManager : public SimpleGameObject {
private:
	int score;
	int level{0};

	float start_ball_speed{500.0f};
	float start_paddle_speed{300.0f};

	sf::Color const lvl_colors[5] = { sf::Color::Red, sf::Color::Green, sf::Color::Blue, sf::Color::Yellow, sf::Color::Magenta };
	int const lvl_scores[5] = { 10, 20, 30, 40, 50 };
	float const ball_speed_lvl_modifier = 1.1f;
	float const paddle_speed_lvl_modifier = 1.1f;

	std::list<std::shared_ptr<Brick>> bricks;
	std::shared_ptr<Ball> ball;
	std::shared_ptr<Paddle> paddle;
	sf::Font font;
	sf::Text scoreText{font, "Score: " + std::to_string(score)};
	int m_bricks_alive_cnt{};
	GameStartObserver* game_start_observer{};

public:
	

	GameManager() : SimpleGameObject{ "GameManager" }
		, score{ 0 } 
	{
		/*if (!font.loadFromFile("path/to/font.ttf")) {
			// Handle error
		}*/
		m_to_be_rendered = true;
		font.openFromFile("C:/Windows/Fonts/cascadiamono.ttf");
		scoreText.setFont(font);
		scoreText.setCharacterSize(50);
		scoreText.setFillColor(sf::Color::White);
		scoreText.setPosition({ -480.0f, 420.0f });
		updateScoreText();
		m_bricks_alive_cnt = bricks.size();

		registerEventHandler<StartEvent>([this](const StartEvent& event) {
			ball->setSpeed(ball->getSpeed() * ball_speed_lvl_modifier);
			paddle->setSpeed(paddle->getSpeed() * paddle_speed_lvl_modifier);
			ball->getRigidBody()->m_velocity = ball->computeVelocity();
			ball->move({ 0.0f, -10.0f });
			ball->addCollider();
			ball->getCollider()->m_lock_in_the_screen = true;
		});

		registerEventHandler<GameStartPendingEvent>([this](const GameStartPendingEvent& event) {
			ball->removeCollider();
			ball->setPosition({ paddle->getPosition().x + paddle->getShape()->getGlobalBounds().size.x / 2.0f, paddle->getPosition().y - ball->getShape()->getGlobalBounds().size.y });
		});
	}

	void setGameStartObserver(GameStartObserver* observer) {
		game_start_observer = observer;
	}

	void addBrick(std::shared_ptr<Brick> brick) {
		bricks.push_back(brick);
		++m_bricks_alive_cnt;
	}

	void addBricks(std::list<std::shared_ptr<Brick>> bricks_l) {
		bricks.append_range(bricks_l);
		m_bricks_alive_cnt += bricks_l.size();
	}

	void setBall(std::shared_ptr<Ball> nball) {
		ball = nball;
		if (ball) { if (ball->getSpeed() == 0)ball->setSpeed(start_ball_speed); else start_ball_speed = ball->getSpeed(); }
	}

	void setPaddle(std::shared_ptr<Paddle> npaddle) {
		paddle = npaddle;
		if (paddle) { if (paddle->getSpeed() == 0)paddle->setSpeed(start_paddle_speed); else start_paddle_speed = paddle->getSpeed(); };
	}

	void resetBricks() {
		level = std::min((level + 1), 4);
		//std::cout << "RESETING BRICKS" << std::endl;
		game_start_observer->is_started = false;

		for (auto& brick : bricks) {
			brick->setDestroyed(false); // Assuming Brick has a method to set its active state
			brick->addCollider();
			brick->getShape()->setFillColor(lvl_colors[level]);
			brick->m_to_be_rendered = true;
			m_bricks_alive_cnt = bricks.size();
		}

		ball->setPosition({ 0.0f, 0.0f });
		paddle->setPosition({ 0.0f, 400.0f });
	}

	void restartGame() {
		level = -1;
		resetBricks();
		updateScore(-score);
		ball->setSpeed(start_ball_speed);
		paddle->setSpeed(start_paddle_speed);
	}

	void updateScore(int points) {
		score += points;
		updateScoreText();
	}

	void updateScoreText() {
		scoreText.setString("Score: " + std::to_string(score));
	}

	void checkBricks() {
		//std::cout << "CHECKING BRICKS" << std::endl;

		if (m_bricks_alive_cnt == 0) {
			resetBricks();
		}
	}

	void handleBrickDestruction(std::shared_ptr<Brick> brick) {
		updateScore(lvl_scores[level]); // Example score increment
		updateScoreText();
		brick->setDestroyed(false);
		--m_bricks_alive_cnt;
		checkBricks();
	}

	virtual void render(sf::RenderTarget& target) const override {
		//std::cout << scoreText.getString().toAnsiString() << std::endl;
		target.draw(scoreText);
	}
};

class BrickDestructionObserver : public Engine {
private:
	std::list<std::shared_ptr<Brick>> bricks;
	GameManager* game_manager;

public:
	BrickDestructionObserver(std::list<std::shared_ptr<Brick>> bricks, GameManager* game_manager)
		: bricks(bricks), game_manager(game_manager) {
	}

	virtual void update() override {
		for (auto& brick : bricks) {
			if (brick->isDestroyed()) {
				game_manager->handleBrickDestruction(brick);
			}
		}
	}
};

class BallFallObserver : public Engine {
private:
	std::shared_ptr<Ball> ball;
	GameManager* game_manager;
public:
	BallFallObserver(std::shared_ptr<Ball> ball, GameManager* game_manager)
		: ball(ball), game_manager(game_manager) {
	}
	virtual void update() override {
		if (!ball->getCollider()) return;
		if (ball->getCollider()->getBounds().position.y + ball->getCollider()->getBounds().size.y > 450.0f) {
			game_manager->restartGame();
		}
	}
};


