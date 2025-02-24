#ifndef Timer_hpp
#define Timer_hpp

#include <chrono>

class Timer {
public:
	void start() 
	{
		if (m_running)
			return;
		m_start = std::chrono::high_resolution_clock::now();
		m_running = true;
	}
	void restart()
	{
		stop();
		start();
	}
	void stop()
	{
		m_running = false;

	}
	// return the duration in milliseconds
	double duration()
	{
		if (!m_running)
			return -1;

		auto now = std::chrono::high_resolution_clock::now();
		const std::chrono::duration<double> high_resolution_duration = now - m_start;
		double milliseconds = high_resolution_duration.count() * 1000;
		return milliseconds;
	}

private:
	std::chrono::steady_clock::time_point m_start;
	bool m_running{ false };
};

#endif