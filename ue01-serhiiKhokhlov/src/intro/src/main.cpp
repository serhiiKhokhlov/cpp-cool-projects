#include <SFML/Config.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <iostream>

// Define the world coordinates
const sf::View world_view{{0, 0}, {1000, 1000}};
const int d1_velocity = 400;

static void center_view(sf::RenderWindow &window, sf::Vector2u new_size) {
  float ratio = (float)new_size.x / (float)new_size.y;
  if (ratio >= 1.0)
    window.setView(sf::View{world_view.getCenter(), {world_view.getSize().x * ratio, world_view.getSize().y}});
  else
    window.setView(sf::View{world_view.getCenter(), {world_view.getSize().x, world_view.getSize().y / ratio}});
}

int main() {
  // Create a render window
  sf::ContextSettings settings;
  settings.antiAliasingLevel = 8; // Request 8x antialiasing
  sf::RenderWindow window(sf::VideoMode({1000, 800}), "SFML Intro", sf::Style::Default, sf::State::Windowed, settings);
  window.setView(world_view);
  center_view(window, window.getSize());

  // Create a circle shape
  sf::CircleShape circle{50};
  circle.setFillColor(sf::Color::Green);
  circle.setPosition({0, 0});

  sf::RectangleShape rect{ {50, 50} };
  rect.setFillColor(sf::Color::Red);
  rect.setPosition({200, 200});

  // Clock to calculate time passed
  sf::Clock clock;

  while (window.isOpen()) {
    // Event processing
    while (const std::optional event = window.pollEvent())
    {
      if (event->is<sf::Event::Closed>()) {
        window.close();
      }
      if (const auto *redraw_event = event->getIf<sf::Event::Resized>()) {
        center_view(window, redraw_event->size);
      }
    }

    sf::Vector2f velocity({0, 0});
        
    // Check if arrow key is pressed
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scan::Up)) {
      velocity.y = -d1_velocity;
    }
    else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scan::Down)) {
      velocity.y = d1_velocity;
    }
    else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scan::Left)) {
      velocity.x = -d1_velocity;
    }
    else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scan::Right)) {
      velocity.x = d1_velocity;
    }

    // Update the circle's position
    sf::Time deltaTime = clock.restart();

    auto old_position = circle.getPosition();
    circle.move(velocity * deltaTime.asSeconds());

    if (circle.getGlobalBounds().findIntersection(rect.getGlobalBounds())) circle.setPosition(old_position);

    // Render the circle
    window.clear();
    window.draw(circle);
    window.draw(rect);
    window.display();
  }

  return 0;
}
