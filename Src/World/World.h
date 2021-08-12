#ifndef B_WORLD_H_INCLUDED
#define B_WORLD_H_INCLUDED

#include <PGE/Graphics/Graphics.h>

#include "../Graphics/Resources.h"

class World {
    public:
        World();
        ~World();

        void run();
        bool shouldEnd() const;

    private:
        PGE::Graphics* graphics;
        Resources* resources;
};

#endif // B_WORLD_H_INCLUDED
