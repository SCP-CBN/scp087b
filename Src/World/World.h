#ifndef B_WORLD_H_INCLUDED
#define B_WORLD_H_INCLUDED

#include <PGE/Graphics/Graphics.h>

#include "../Graphics/GraphicsResources.h"

class World {
    public:
        World();
        ~World();

        void run();
        bool shouldEnd() const;

    private:
        PGE::Graphics* graphics;
        GraphicsResources* gfxRes;
};

#endif // B_WORLD_H_INCLUDED
