#include "Ticker.h"

#include "../World/World.h"

using namespace PGE;

Ticker::Ticker(World& world) : world(world) { }

void Ticker::run() {
    prev = Clock::now();
	do {
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
            accumulator = 0;
            lastFps = fps;
            fps = 0;
        }
        fps++;

        delta = diff / CLOCK_TIME_PER_TICK;
        prev = now;
	} while (world.update(delta));
}

int Ticker::getFPS() const {
    return lastFps;
}

float Ticker::getInterpFPS() const {
    return TICKS_PER_SECOND / delta;
}
