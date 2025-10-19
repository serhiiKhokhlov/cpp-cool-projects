#pragma once
#include <SFML/Config.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <memory>
#include <list>
#include <unordered_map>
#include <functional>
#include <typeindex>
#include <string>
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

class SimpleGameObject : public sf::Transformable, public IEventListener {
protected:

    struct RigidBody {   // in case if anynone will have a courage to program their own game physics :)
        
        friend class SimpleGameObject;
		friend class Engine;

        sf::Vector2f m_velocity{0, 0}; // in the breakout-game is the only used component 
        unsigned int m_weight{0};
        sf::Vector2f m_acceleration{0, 0};
        unsigned int m_exForce{0};

    };

    class Collider {
        friend class SimpleGameObject;
        friend class Engine;
    private:
        sf::FloatRect m_bounds{};

    public:
        bool m_is_updated{ false };
        bool m_lock_in_the_screen{ false };

        Collider() = default;

        explicit Collider(const Collider& other) = default;

        explicit Collider(Collider&& other) noexcept
            : m_is_updated{ other.m_is_updated }
            , m_bounds{ std::move(other.m_bounds) }
        {
        }
    
        Collider& operator=(const Collider& other) {
            if (this != &other) {
                m_is_updated = other.m_is_updated;
                m_bounds = other.m_bounds;
            }
            return *this;
        }

        Collider& operator=(Collider&& other) noexcept {
            if (this != &other) {
                m_is_updated = other.m_is_updated;
                m_bounds = std::move(other.m_bounds);
            }
            return *this;
        }

		sf::FloatRect getBounds() const {
			return m_bounds;
		}

        virtual bool collides_with(Collider const & other) const {
            return m_bounds.findIntersection(other.m_bounds) ? true : false;
        }

        virtual ~Collider() = default;
    };

	struct Transform {
		sf::Vector2f position{};
		sf::Angle rotation{};
		sf::Vector2f scale{1.0f, 1.0f};
	};

	Transform m_prev_transform_state{};
	Collider const * m_prev_collider_state{};

    std::string label{};
    std::unique_ptr<sf::Shape> m_shape{};
    std::unique_ptr<Collider> collider{};
    std::unique_ptr<RigidBody> rigid_body{};

    void updateCollider() {
        m_prev_collider_state = collider.get();
        if (collider) {
            collider->m_bounds = m_shape->getGlobalBounds();
            collider->m_is_updated = true;
        }
    }

public:
    bool m_to_be_rendered{ false };  //for usage in the game loop

    SimpleGameObject() = default;

    explicit SimpleGameObject(std::string label) : label{ std::move(label) } {}

    explicit SimpleGameObject(std::string label, std::unique_ptr<sf::Shape> shape)
        : label{ std::move(label) }
        , m_shape{ std::move(shape) } {
    }

    explicit SimpleGameObject(const SimpleGameObject& other) = delete; // No copy support for sf::Drawable

    explicit SimpleGameObject(SimpleGameObject&& other) noexcept = default;
    SimpleGameObject& operator=(SimpleGameObject&&) noexcept = default;

    virtual ~SimpleGameObject() = default;

    void setShape(std::unique_ptr<sf::Shape> n_shape) {
        m_shape = std::move(n_shape);
        updateCollider();
    }

	sf::Shape* getShape() const {
		return m_shape.get();
	}

    void addCollider() {
        if (m_shape) {
            collider = std::make_unique<Collider>();
            collider->m_bounds = m_shape->getGlobalBounds();
        };
    }

	void removeCollider() {
		collider.reset();
	}

    Collider * getCollider() const {
        return collider.get();
    }

	Collider const* getPrevCollider() const {
		return m_prev_collider_state;
	}

    void addRigidBody() {
        rigid_body = std::make_unique<RigidBody>();
    }

	void removeRigidBody() {
		rigid_body.reset();
	}

	RigidBody * getRigidBody() const {
		return rigid_body.get();
	}

    virtual void render(sf::RenderTarget& target) const {
        target.draw(*m_shape);
    }

    std::string getLabel() const {
        return label;
    }

    void setLabel(std::string new_label) {
        label = std::move(new_label);
    }

    virtual void setPosition(sf::Vector2f n_pos) {
		m_prev_transform_state.position = m_shape->getPosition();
        m_shape->setPosition(n_pos);
        updateCollider();
    }

    sf::Vector2f getPosition() const {
        return m_shape ? m_shape->getPosition() : sf::Vector2f{0.0f, 0.0f};
    }

    virtual void move(sf::Vector2f n_pos) {
        m_prev_transform_state.position = m_shape->getPosition();
        m_shape->move(n_pos);
        updateCollider();
    }

    virtual void setRotation(sf::Angle angle) {
        m_prev_transform_state.rotation = m_shape->getRotation();
        m_shape->setRotation(angle);
        updateCollider();
    }

    sf::Angle getRotation() const {
        return m_shape->getRotation();
    }

    virtual void rotate(sf::Angle angle) {
        m_prev_transform_state.rotation = m_shape->getRotation();
        m_shape->rotate(angle);
        updateCollider();
    }

    virtual void setScale(sf::Vector2f factors) {
        if (m_shape) {
            m_prev_transform_state.scale = m_shape->getScale();
            m_shape->setScale(factors);
            updateCollider();
        }
    }

    virtual void scale(sf::Vector2f factor) {
        m_prev_transform_state.scale = m_shape->getScale();
        m_shape->scale(factor);
        updateCollider();
    }

    sf::Vector2f getScale() const {
        return m_shape->getScale();
    }

    virtual void setOrigin(sf::Vector2f origin) {
        m_shape->setOrigin(origin);
        updateCollider();
    }

    sf::Vector2f getOrigin() const {
        return m_shape->getOrigin();
    }

    void resetTransform() {
        m_shape->setPosition(m_prev_transform_state.position);
        m_shape->setRotation(m_prev_transform_state.rotation);
        m_shape->setScale(m_prev_transform_state.scale);
    }

	Transform getPrevTransform() const {
		return m_prev_transform_state;
	}

};



