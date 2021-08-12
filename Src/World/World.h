#ifndef B_WORLD_H_INCLUDED
#define B_WORLD_H_INCLUDED

#include <PGE/Graphics/Graphics.h>
#include <PGE/Input/InputManager.h>

class World {
    public:
        World();
        ~World();

        void run();
        bool shouldEnd() const;

    private:
        PGE::Graphics* graphics;
        PGE::InputManager* inputManager;
        class Camera* camera;
        class Resources* resources;

        bool paused = false;
};

#endif // B_WORLD_H_INCLUDED
