#ifndef B_TICKER_H_INCLUDED
#define B_TICKER_H_INCLUDED

#include <chrono>

#include <PGE/Types/Types.h>

class Ticker {
	public:
		Ticker(class World& world);

		void run();

		int getFPS() const;
		float getInterpFPS() const;

	private:
		World& world;

		using Clock = std::chrono::steady_clock;

		// During one second delta will have been about this much in sum.
		static constexpr int TICKS_PER_SECOND = 60;
		static constexpr float CLOCK_TIME_SECOND = (float)Clock::period::den / Clock::period::num;
		static constexpr float CLOCK_TIME_PER_TICK = CLOCK_TIME_SECOND / TICKS_PER_SECOND;

		std::chrono::time_point<Clock> prev;
		
		PGE::u64 accumulator = 0;
		PGE::u64 tickAccu = 0;
		
		float delta;

		int fps = 0;
		int lastFps;
};

#endif // B_TICKER_H_INCLUDED
