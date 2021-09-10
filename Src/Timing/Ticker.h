#ifndef B_TICKER_H_INCLUDED
#define B_TICKER_H_INCLUDED

#include <chrono>

#include <PGE/Types/Types.h>

class Ticker {
	public:
		Ticker(class World& world);

		void run();

	private:
		World& world;

		using Clock = std::chrono::steady_clock;
		std::chrono::time_point<Clock> prev;
		
		PGE::u64 accumulator = 0;
		PGE::u64 tickAccu = 0;
		
		int fps = 0;
};

#endif // B_TICKER_H_INCLUDED
