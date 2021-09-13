#ifndef B_WORLD_H_INCLUDED
#define B_WORLD_H_INCLUDED

#include <chrono>

#include <PGE/Graphics/Graphics.h>
#include <PGE/Input/InputManager.h>

#include "../Timing/TimeMaster.h"
#include "../Timing/Ticker.h"
#include "../Graphics/Rooms/RoomSet.h"

class World {
    public:
        World(TimeMaster& tm);
        ~World();

        void run();
        bool shouldEnd() const;

        void togglePaused();

        bool update(float delta);
        void tick();
        void render(float interp) const;

    private:
        TimeMaster& tm;
        // The real master behind the curtains
        Ticker ticker = Ticker(*this);

        RoomSet rooms;

        PGE::Graphics* graphics;
        PGE::InputManager* inputManager;
        class Camera* camera;
        class Resources* resources;

        bool paused = true;
};

#endif // B_WORLD_H_INCLUDED
