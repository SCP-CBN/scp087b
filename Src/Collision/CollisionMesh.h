#ifndef B_COLLISIONMESH_H_INCLUDED
#define B_COLLISIONMESH_H_INCLUDED

#include <PGE/Math/Matrix.h>
#include <PGE/Math/Line.h>

#include "Collision.h"

class CollisionMesh {
	public:
		CollisionMesh(std::vector<PGE::Vector3f>&& verts, std::vector<PGE::u32>&& inds)
			: vertices(std::move(verts)), indices(std::move(inds)) { }

		Collision checkCollision(const PGE::Matrix4x4f& matrix, const PGE::Line3f& line, float height, float radius) const;

	private:
		std::vector<PGE::Vector3f> vertices;
		std::vector<PGE::u32> indices;
};

#endif // B_COLLISIONMESH_H_INCLUDED
