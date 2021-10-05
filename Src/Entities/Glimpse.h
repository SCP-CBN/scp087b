#ifndef B_GLIMPSE_H_INCLUDED
#define B_GLIMPSE_H_INCLUDED

#include <PGE/Graphics/Mesh.h>

#include "../Graphics/Resources.h"

class Glimpse {
    public:
        Glimpse(Resources& res);
        ~Glimpse();

        void setPosition(const PGE::Vector3f& inPos);
        void render();
        void lerpFacing(const PGE::Vector3f& direction);
        void setFacing(const PGE::Vector3f& direction);

    private:
        Resources& resources;
        PGE::Vector3f glimpsePos;
        PGE::Mesh* glimpseMesh;
        PGE::Material* material;
        PGE::Vector3f facing;
        Resources::Handle<PGE::Texture> glimpseTex;
};

#endif // B_GLIMPSE_H_INCLUDED
