#include "DESSimulator.h"


DESSimulator::DESSimulator(std::unique_ptr<ResourcePool> resource_pool) : m_resource_pool(std::move(resource_pool)) {}

// Define the static member
std::string DESSimulator::Metrics::m_header = "";

void DESSimulator::run() {
    while (m_running && !m_events_queue.empty()) {
        step();
    }
}

void DESSimulator::step() {
    // First, check and process waiting queue before new events
	std::cout << "Available resources: " << m_resource_pool->getFreeCount() << std::endl;

    if (!m_events_queue.empty()) {
        executeNextEvent();
    }
}

void DESSimulator::addEvent(std::shared_ptr<Event> event) {
    m_events_queue.push(event);
}

void DESSimulator::addEvents(std::vector<std::shared_ptr<Event>> events) {
    for (auto& event : events) {
        m_events_queue.push(event);
    }
}

std::ostream& DESSimulator::print_events(std::ostream& os) const {
	os << "Events in the queue: " << std::endl;
	auto temp_queue = m_events_queue;
	while (!temp_queue.empty()) {
		auto event = temp_queue.top();
		os << *event;
		temp_queue.pop();
	}
	return os;
}

bool DESSimulator::handleEvent(std::shared_ptr<Event> event) {
    if (event->execute(*this)) {
        auto next_event = event->makeNextEvent();
        if (next_event) {
            addEvent(next_event);
        }
        return true;
    }
    else return false;
}

bool DESSimulator::executeNextEvent() {
    if (!m_events_queue.empty()) {
        current_event = m_events_queue.top();
        m_events_queue.pop();
        current_time = current_event->getTime();

        return handleEvent(current_event);
    }
    else return false;
}
