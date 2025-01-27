#include "Timer.hpp"

void Timer::start()
{
	if (m_running)
		return;
	m_start = std::chrono::high_resolution_clock::now();
	m_running = true;
}

void Timer::restart()
{
	stop();
	start();
}

void Timer::stop()
{
	m_running = false;
}

double Timer::duration()
{
	if (!m_running)
		return -1;

	auto now = std::chrono::high_resolution_clock::now();
	const std::chrono::duration<double> high_resolution_duration = now - m_start;
	double milliseconds = high_resolution_duration.count() * 1000;
	return milliseconds;
}
