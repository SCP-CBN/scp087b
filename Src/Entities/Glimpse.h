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
        void lerpFacing(float interp);
        void setPriorTick(const PGE::Vector3f& direction);
        void setCurrTick(const PGE::Vector3f& direction) { currTick = direction; };
        void setFacing(const PGE::Vector3f& direction);
        PGE::Vector3f getPriorTick() { return priorTickFacing; };
        PGE::Vector3f getCurrTick() { return currTick; };

    private:
        Resources& resources;
        PGE::Vector3f glimpsePos;
        PGE::Mesh* glimpseMesh;
        PGE::Material* material;
        PGE::Vector3f priorTickFacing;
        PGE::Vector3f facing;
        PGE::Vector3f currTick;
        Resources::Handle<PGE::Texture> glimpseTex;
};

#endif // B_GLIMPSE_H_INCLUDED
