#pragma once
#include <string>
#include <iostream>
#include <memory>
#include "Time.h"
#include "DESSimulator.h"

class DESSimulator;

class Event {
protected:
    std::string m_name;
    double m_time;
    int m_id;
    bool m_to_be_disposed{false};

public:
    Event(int id, std::string name, double time) : m_id{id}, m_name(std::move(name)), m_time(time) {}
    virtual ~Event() = default;

    virtual std::shared_ptr<Event> makeNextEvent() { return nullptr; }
    virtual bool execute(DESSimulator& sim) = 0;

    double getTime() const { return m_time; }

	std::string getName() const { return m_name; }

	int getId() const { return m_id; }

	virtual void reschedule(double new_time) {
		m_time = new_time;
	}

    bool toBeDisposed() const {
        return m_to_be_disposed;
    }

    // Overload operator< so priority_queue works correctly
    bool operator<(const Event& other) const {
        return m_time > other.m_time;  // Reverse order for min-heap behavior
    }

    struct EventComparator {
        bool operator()(const std::shared_ptr<Event>& lhs, const std::shared_ptr<Event>& rhs) const {
            return lhs->getTime() > rhs->getTime();
        }
    };

    friend std::ostream& operator<< (std::ostream& os, const Event & event) {
        return os << "Event: " << TimeConverter::toTimeString( event.getTime() )<< " " << event.getName() << " " << event.getId() << std::endl;
    }
};

