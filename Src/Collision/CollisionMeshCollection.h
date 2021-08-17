#ifndef B_COLLISIONMESHCOLLECTION_H_INCLUDED
#define B_COLLISIONMESHCOLLECTION_H_INCLUDED

#include <stack>

#include <PGE/Math/Matrix.h>

#include "CollisionMesh.h"

class CollisionMeshCollection {
    private:
        struct Instance;

	public:
        struct Handle : PGE::NoHeap {
            friend CollisionMeshCollection;

            Handle() = default;

            void update(const PGE::Matrix4x4f& mat) const;
            bool valid() const;

            private:
                Handle(int index, Instance& inst);

                int index;
                Instance* instance = nullptr;
        };

        Handle addInstance(const CollisionMesh& mesh, const PGE::Matrix4x4f& matrix);
        void removeInstance(Handle& hdl);

        Collision checkCollision(const PGE::Line3f& line, float height, float radius) const;

    private:
        struct Instance : PGE::NoHeap {
            Instance(const CollisionMesh& msh, const PGE::Matrix4x4f& mtrx);

            Collision checkCollision(const PGE::Line3f& line, float height, float radius) const;

            PGE::AABBox getBoundingBox() const;
            void updateMatrix(const PGE::Matrix4x4f& newMat);

            private:
                const CollisionMesh& mesh;
                PGE::Matrix4x4f mat;
                PGE::AABBox bbox;

                void recalculateBoundingBox();
        };

        // TODO: Benchmark this!
        std::unordered_map<int, Instance> instances;
        std::stack<int, std::vector<int>> freeInts;
};

#endif // B_COLLISIONMESHCOLLECTION_H_INCLUDED
