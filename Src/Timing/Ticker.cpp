#include "Ticker.h"

#include "../World/World.h"

using namespace PGE;

Ticker::Ticker(World& world) : world(world) { }

void Ticker::run() {
    prev = Clock::now();
    float delta;
	do {
        // During one second delta will have been about this much in sum.
        constexpr int TICKS_PER_SECOND = 60;
        constexpr float CLOCK_TIME_SECOND = (float)Clock::period::den / Clock::period::num;
        constexpr float CLOCK_TIME_PER_TICK = CLOCK_TIME_SECOND / TICKS_PER_SECOND;
        
        std::chrono::time_point<Clock> now = Clock::now();
        u64 diff = (now - prev).count();

        tickAccu += diff;
        while (tickAccu >= CLOCK_TIME_PER_TICK) {
            tickAccu -= (u64)CLOCK_TIME_PER_TICK;
            world.tick();
        }

        float tickProgress = tickAccu / CLOCK_TIME_PER_TICK;
        world.render(tickProgress);

        accumulator += diff;
        if (accumulator >= CLOCK_TIME_SECOND) {
            //text->setText(String::from(fps));
            accumulator = 0;
            fps = 0;
        }
        fps++;

        delta = diff / CLOCK_TIME_PER_TICK;
        prev = now;
	} while (world.update(delta));
}
