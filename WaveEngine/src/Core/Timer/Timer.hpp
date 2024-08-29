#ifndef Timer_hpp
#define Timer_hpp

#include <chrono>

class Timer {
public:
	void start();
	void restart();
	void stop();
	// return the duration in milliseconds
	double duration();

private:
	std::chrono::steady_clock::time_point m_start;
	bool m_running{ false };
};

#endif