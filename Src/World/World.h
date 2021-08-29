#ifndef B_WORLD_H_INCLUDED
#define B_WORLD_H_INCLUDED

#include <chrono>

#include <PGE/Graphics/Graphics.h>
#include <PGE/Input/InputManager.h>

#include "../Timing/TimeMaster.h"

class World {
    public:
        World(TimeMaster& tm);
        ~World();

        void run();
        bool shouldEnd() const;

        void togglePaused();

    private:
        void tick();
        PGE::u64 tickAccu = 0;

        TimeMaster& tm;

        PGE::Graphics* graphics;
        PGE::InputManager* inputManager;
        class Camera* camera;
        class Resources* resources;

        bool paused = true;

        using Clock = std::chrono::steady_clock;
        std::chrono::time_point<Clock> prev;
        PGE::u64 accumulator = 0;
        int fps = 0;
};

#endif // B_WORLD_H_INCLUDED
