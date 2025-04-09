#pragma once
#include <iostream>
#include <string>
#include <memory>
#include <queue>
#include <vector>
#include <unordered_map>
#include "ResourcePool.h"
#include "Event.h"

class DESSimulator {
public:
    struct Metrics {
    protected:
        static std::string m_header; // Static member to hold the header string
	public:
		virtual ~Metrics() = default;
        virtual std::ostream& print(std::ostream& out) const = 0;
        static std::ostream& print_header(std::ostream& out) {
			return out << m_header << std::endl;
        }

		static void set_header(std::initializer_list <std::string> header) {
			std::string new_header = "";
			for (auto& h : header) {
				new_header += h + ";";
			}
			new_header.pop_back(); // Remove the last semicolon
			m_header = new_header;
		}
    };

protected:

    std::unique_ptr<ResourcePool> m_resource_pool;
    std::priority_queue<std::shared_ptr<Event>, std::vector<std::shared_ptr<Event>>, Event::EventComparator> m_events_queue;
    bool m_running{ true };
    double current_time{};
	std::shared_ptr<Event> current_event{};
    bool handleEvent(std::shared_ptr<Event> event);
	std::vector <std::shared_ptr<Metrics>> m_metrics;

public:

    DESSimulator(std::unique_ptr<ResourcePool> resource_pool);
    virtual ~DESSimulator() {}

    virtual void run();
    virtual void step();

    virtual void resetMetrics() = 0;
    virtual void computeMetrics() = 0;

    virtual std::vector <std::shared_ptr<Metrics>> getCurrentMetrics() const = 0;

    std::ostream& print_events(std::ostream& os) const;

    virtual void addEvent(std::shared_ptr<Event> event);

    virtual void addEvents(std::vector<std::shared_ptr<Event>> events);

    virtual bool executeNextEvent();

    void stop() { m_running = false; }

	std::unique_ptr<ResourcePool>& getResourcePool() { return m_resource_pool; }
};