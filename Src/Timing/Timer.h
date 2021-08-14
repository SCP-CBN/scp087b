#ifndef B_TIMER_H_INCLUDED
#define B_TIMER_H_INCLUDED

#include <chrono>
#include <unordered_map>

#include <PGE/String/Key.h>

class Timer {
	public:
		Timer(class TimeMaster& tm, const PGE::String& name);
		~Timer();

	private:
		using Clock = std::chrono::steady_clock;

		struct Time** current;
		const PGE::String name;
		const std::chrono::time_point<Clock> start;
};

#endif // B_TIMER_H_INCLUDED
