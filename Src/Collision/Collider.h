#ifndef B_COLLIDER_H_INCLUDED
#define B_COLLIDER_H_INCLUDED

#include "CollisionMeshCollection.h"

// TODO: Change this shit.
class Collider {
	public:
        Collider(float radius, float height);

        void setCollisionMeshCollection(CollisionMeshCollection* c);

        PGE::Vector3f tryMove(const PGE::Vector3f& position, const PGE::Vector3f& goal) const;

    private:
        CollisionMeshCollection* cmc = nullptr;
                
        float height;
        float radius;
};

#endif // B_COLLIDER_H_INCLUDED
