#include <iostream>
#include <fstream>
#include <memory>
#include <sstream>
#include <queue>
#include <random>
#include <array>
#include <iomanip>
#include "DESSimulator.h"
#include "Event.h"
//#include "Time.h"

std::string doubleToCommaString(double value, int precision = 3) {
	std::ostringstream oss;
	oss << std::fixed << std::setprecision(precision) << value;

	std::string result = oss.str();

	// Replace '.' with ','
	std::size_t dotPos = result.find('.');
	if (dotPos != std::string::npos) {
		result[dotPos] = ',';
	}

	return result;
}

class CallCenter final : public DESSimulator {
public:
	struct CCMetrics final : public Metrics {
		double m_avg_waiting_time{};
		double m_avg_worker_usg{};
		double m_avg_queue_length{};
		double time{};

		std::ostream& print(std::ostream& out) const override {
			return out << *this;
		}

		//return in the format of the csv file
		friend std::ostream& operator << (std::ostream& out, CCMetrics rhs) {
			return out << TimeConverter::toCSVTimeString(rhs.time) << ";"
					   << TimeConverter::toCSVTimeString(rhs.m_avg_waiting_time) << ";" 
					   << doubleToCommaString(rhs.m_avg_worker_usg) << ";"
					   << doubleToCommaString(rhs.m_avg_queue_length) << "\n";
		}
	};

private: 
	std::queue<std::shared_ptr<Event>> m_waiting_queue{};  // New queue for waiting events
	std::queue<std::shared_ptr<Event>> m_abandoned_queue{}; // New vector for abandoned calls

	int m_updates_cnt{0};

	double m_waiting_time_sum{};
	double m_worker_usg_sum{};
	double m_queue_length{};

	class MetricsCheckEvent final : public Event {
	public:
		MetricsCheckEvent(double time) : Event(-2, "MetricsCheckEvent", time) {}

		bool execute(DESSimulator& sim) override {
			sim.computeMetrics();
			sim.resetMetrics();
			return true;
		}
	};

	void setMetricsCheckInterval(double time_interval) {
		for (double time = TimeConverter::toTime(0, 6, 0, 0) + time_interval; time < TimeConverter::toTime(0, 18, 0, 0); time += time_interval) {
			m_events_queue.push(std::make_shared<MetricsCheckEvent>(time));
		}
	}

	void updateMetrics();

public:
	CallCenter(std::unique_ptr<ResourcePool> resource_pool, double time_interval) : DESSimulator(std::move(resource_pool)) { 
		setMetricsCheckInterval(time_interval);
	}

	virtual ~CallCenter() {}

	void resetMetrics() override {
		m_waiting_time_sum = 0.0;
		m_worker_usg_sum = 0.0;
		m_queue_length = 0.0;
		m_updates_cnt = 0;
	}

	void computeMetrics() override {
		if (m_updates_cnt != 0) {
			std::unique_ptr<CCMetrics> n_metrics = std::make_unique<CCMetrics>();
			n_metrics->m_avg_waiting_time = m_waiting_time_sum / double(m_updates_cnt);
			n_metrics->m_avg_worker_usg = m_worker_usg_sum / double(m_updates_cnt);
			n_metrics->m_avg_queue_length = m_queue_length / double(m_updates_cnt);
			n_metrics->time = current_time;
			m_metrics.push_back(std::move(n_metrics));
		}
	}

	void printMetrics(std::ostream& os) const {
		CCMetrics::print_header(os);
		for (const auto& metric : m_metrics) {
			metric->print(os);
		}
	}

	void processWaitingQueue() {
		int queue_size = (int)m_waiting_queue.size();
		for (int i = 0; i < queue_size; ++i) {
			auto event = m_waiting_queue.front();
			m_waiting_queue.pop();
			event->reschedule(current_time);
			if (event->toBeDisposed()) {
	//			std::cout << "Abandoning event " << event->getId() << std::endl;
				m_abandoned_queue.push(event);
				return;
			}
			if (!handleEvent(event)) m_waiting_queue.push(event);
		}
	}

	std::vector <std::shared_ptr<Metrics>> getCurrentMetrics() const override {
		return m_metrics;
	}

	virtual void step() override {
		// First, check and process waiting queue before new events
	//	std::cout << "Available resources: " << m_resource_pool->getFreeCount() << std::endl;
		processWaitingQueue();
		if (!m_events_queue.empty()) {
			if (!executeNextEvent()) {
				// If execution fails (e.g., no available resources), move event to waiting queue
				m_waiting_queue.push(current_event);
			}
		}
		updateMetrics();
	}

	std::queue <std::shared_ptr<Event>> get_waiting_queue() const {
		return m_waiting_queue;
	}

	std::queue <std::shared_ptr<Event>> get_abandoned_queue() const {
		return m_abandoned_queue;
	}

	std::ostream& print_waiting_events(std::ostream& os) const {
		os << "Waiting calls: " << std::endl;
		auto temp_queue = m_waiting_queue;
		while (!temp_queue.empty()) {
			auto event = temp_queue.front();
			os << *event;
			temp_queue.pop();
		}
		return os;
	}

	std::ostream& print_abandoned_events(std::ostream& os) const {
		os << "Abandoned calls: " << std::endl;
		auto temp_queue = m_abandoned_queue;
		while (!temp_queue.empty()) {
			auto event = temp_queue.front();
			os << *event;
			temp_queue.pop();
		}
		return os;
	}


	std::vector<std::shared_ptr<Event>> generate_calls(const std::array<int, 24>& call_rates);
};

class StopEvent : public Event {
public:
    StopEvent(double time) : Event(-1, "StopEvent", time) {}

    bool execute(DESSimulator& sim) override {
 //     std::cout << "Stopping simulation at time: " << TimeConverter::toTimeString(m_time) << std::endl;
        sim.stop();
        return true;   
    }
};

class CallEndEvent : public Event {
	int to_be_released_cnt{};
public:
	CallEndEvent(int id, double time, int acquired_cnt) : Event(id, "CallEndEvent", time), to_be_released_cnt{ acquired_cnt } {}
	bool execute(DESSimulator& sim) override {
		sim.getResourcePool()->release(to_be_released_cnt);
//		std::cout << "Ending call " << m_id <<" at time : " << TimeConverter::toTimeString(m_time) << std::endl;
		return true;
	}
};

class CallStartEvent : public Event {
protected:
	int m_rsc_cnt{ 1 };
	int acquired_cnt{};
	double scheduled_start_time{};
	double m_time_to_abandon{1.0};

public:

	static std::default_random_engine generator;
	static std::normal_distribution<double> call_duration_distribution;
	static std::lognormal_distribution<double> abandon_time_distribution;

	CallStartEvent(int id, double time, int rsc_cnt)
		: Event(id, "CallStartEvent", time)
		, m_rsc_cnt{ rsc_cnt }
		, scheduled_start_time{ time }
		, m_time_to_abandon{TimeConverter::toTime(0, 0, std::max(0.0001, abandon_time_distribution(generator)), 0)} {
	}

	bool execute(DESSimulator& sim) override {
		if (sim.getResourcePool()->acquire(m_rsc_cnt)) {
			acquired_cnt = m_rsc_cnt;
	//		std::cout << "Starting call " << m_id << " at time: " << TimeConverter::toTimeString(m_time) << std::endl;
			return true;
		}
		else {
	//		std::cout << "No available resources for call " << m_id << " at time: " << TimeConverter::toTimeString(m_time) << std::endl;
			return false;
		}
	}

	double getWaitingTime() {
		return m_time - scheduled_start_time;
	}

	void reschedule(double new_time) override {
		if ((m_time = new_time) - scheduled_start_time > m_time_to_abandon) m_to_be_disposed = true;
	}

	double getTimeToAbandon() const {
		return m_time_to_abandon;
	}

	int getUsedResources() const {
		return acquired_cnt;
	}

	std::shared_ptr<Event> makeNextEvent() override {
		// Generate normally distributed call duration
		double duration = std::max(0.0, call_duration_distribution(generator)); // Ensure positive duration

		std::shared_ptr<Event> result = std::make_shared<CallEndEvent>(m_id, m_time + TimeConverter::toTime(0, 0, duration, 0), acquired_cnt);
		return result;
	}
};

void CallCenter::updateMetrics() {
	double waiting_time{ 0.0 };
	auto q_copy = m_waiting_queue;

	int cnt{ 0 };
	for (int i = 0; i < q_copy.size(); ++i) {
		auto event = std::dynamic_pointer_cast<CallStartEvent>(q_copy.front());
		q_copy.pop();
		waiting_time += event ? ++cnt, event->getWaitingTime() : 0.0;
	}

	m_waiting_time_sum += cnt == 0 ? 0.0 : waiting_time / double(cnt);
	m_worker_usg_sum += m_resource_pool->getBusyCount();
	m_queue_length += m_waiting_queue.size();

	++m_updates_cnt;
}

//=============================== Static members ===============================//
// Initialize the random number generator
std::default_random_engine CallStartEvent::generator(std::random_device{}());

// Initialize the normal distribution with mean = 3 min, std dev = 1 min
std::normal_distribution<double> CallStartEvent::call_duration_distribution(3.0, 1.0);

//Initialize the logarithmic distribution with mean = 3 min, std dev = 1 min
std::lognormal_distribution<double> CallStartEvent::abandon_time_distribution(3.0, 1.0);

//===============================================================================//


std::vector<std::shared_ptr<Event>> CallCenter::generate_calls(const std::array<int, 24>& call_rates) {
	std::vector<std::shared_ptr<Event>> events;
	std::default_random_engine generator;
	int call_id{ 0 };

	for (int hour = 6; hour < 18; ++hour) {
		std::exponential_distribution<double> exp_dist(call_rates[hour] / 60.0);
		double time_in_hour = 0.0;

		while (time_in_hour < 60.0) {
			time_in_hour += exp_dist(generator);
			if (time_in_hour < 60.0) {
				// Convert hour and minute to fractional day format
				double event_time = TimeConverter::toTime(0, hour, time_in_hour, 0);
				events.push_back(std::make_shared<CallStartEvent>(call_id++, event_time, 1));
				//			std::cout << "Generated call event at " << TimeConverter::toTimeString(event_time) << std::endl;
			}
		}
	}

	events.push_back(std::make_shared<StopEvent>(TimeConverter::toTime(0, 18, 0, 1)));

	return events;
}


std::vector < std::shared_ptr <DESSimulator::Metrics> > simulate_with(int const workers, std::array<int, 24> const& call_rates, double const m_check_time_interval = TimeConverter::toTime(0, 11, 59, 59) /*metrics for the whole working 12h - day*/) {
	auto sim = CallCenter(std::make_unique<ResourcePool>(workers), m_check_time_interval);
	sim.addEvents(sim.generate_calls(call_rates));
	sim.run();
	return sim.getCurrentMetrics();
}

int optimize_workers(std::array<int, 24> const& call_rates, int const max_workers, int min_workers = 1) {
	//First stage
	std::shared_ptr<CallCenter::CCMetrics> metrics;

	for (min_workers; min_workers <= max_workers; ++min_workers) {
		metrics = std::dynamic_pointer_cast<CallCenter::CCMetrics> (simulate_with(min_workers, call_rates)[0]);
		if (metrics->m_avg_waiting_time <= TimeConverter::toTime(0,0,0,30) && metrics->m_avg_queue_length < 10 ) {
			break;
		}
	}

	//Second stage
	double prev_avg_worker_usg_ratio{metrics->m_avg_worker_usg / min_workers};
	//std::cout << "Ratio: " << prev_avg_worker_usg_ratio << std::endl;

	metrics = std::dynamic_pointer_cast<CallCenter::CCMetrics>(simulate_with(min_workers + 1, call_rates)[0]);

	while (metrics->m_avg_worker_usg / (min_workers + 1) > prev_avg_worker_usg_ratio) {
		++min_workers;
	//	std::cout << "Ratio: " << metrics->m_avg_worker_usg / min_workers << std::endl;
		prev_avg_worker_usg_ratio = metrics->m_avg_worker_usg / min_workers;
		metrics = std::dynamic_pointer_cast<CallCenter::CCMetrics>(simulate_with(min_workers + 1, call_rates)[0]);
	}


	return min_workers;
}


int main() {	

	std::array<int, 24> const call_rates = {
		5,  5,  5,  5,  10,  20,  50, 100, 150, 200, 250, 300,  // 6 AM - 12 PM (Morning Peak)
		250, 200, 180, 150, 100, 80,  60,  40,  20,  10,  5,  5   // 12 PM - 12 AM (Afternoon & Evening Decline)
	};

	int const workers = 4;

	CallCenter::CCMetrics::set_header({ "Time", "AvgWaitingTime", "AvgWorkerUsage", "AvgQueueLength" });

	std::ofstream csv_out("metrics.csv");

	int optimal_workers = optimize_workers(call_rates, 20, workers);
	std::cout << "Optimal number of workers: " << optimal_workers << std::endl;
	
	auto resource_pool = std::make_unique<ResourcePool>(workers);
	auto sim = CallCenter(std::move(resource_pool), TimeConverter::toTime(0, 11, 59, 59));

	sim.addEvents(sim.generate_calls(call_rates));

	sim.print_events(std::cout);   // prints no nested events, only the main events

	//sim.run();

	sim.printMetrics(std::cout);

	//std::cout << "Total waiting:" << sim.get_waiting_queue().size()  << std::endl;

	//sim.print_waiting_events(std::cout);

	//std::cout << "Total abandoned:" << sim.get_abandoned_queue().size() << std::endl;

	//sim.print_abandoned_events(std::cout);

	return 0;
}
