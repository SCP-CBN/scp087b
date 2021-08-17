#include "CollisionMesh.h"

using namespace PGE;

Collision CollisionMesh::checkCollision(const Matrix4x4f& matrix, const Line3f& line, float height, float radius) const {
    AABBox lineBox(line.pointA);
    lineBox.addPoint(line.pointB);
    lineBox.addPoint(lineBox.getMin() + Vector3f(-radius, -height * 0.5f, -radius));
    lineBox.addPoint(lineBox.getMax() + Vector3f(radius, height * 0.5f, radius));
    Collision retVal;
    AABBox triBox(Vectors::ZERO3F);
    for (size_t i = 0; i < indices.size() / 3; i++) {
        Vector3f vert0 = matrix.transform(vertices[indices[i * 3 + 0]]);
        Vector3f vert1 = matrix.transform(vertices[indices[i * 3 + 1]]);
        Vector3f vert2 = matrix.transform(vertices[indices[i * 3 + 2]]);
        triBox = AABBox(vert0);
        triBox.addPoint(vert1);
        triBox.addPoint(vert2);
        triBox.addPoint(triBox.getMin() + Vector3f(-0.1f, -0.1f, -0.1f));
        triBox.addPoint(triBox.getMax() + Vector3f(0.1f, 0.1f, 0.1f));
        if (!triBox.intersects(lineBox)) { continue; }
        Collision coll = Collision::triangleCollide(line, height, radius, vert0, vert1, vert2);
        if (coll.hit) {
            if (!retVal.hit || retVal.coveredAmount > coll.coveredAmount) {
                retVal = coll;
            }
        }
    }
    return retVal;
}

AABBox CollisionMesh::calculateBoundingBox(const Matrix4x4f& matrix) const {
    AABBox ret = AABBox(matrix.transform(vertices[0]));

    for (int i = 1; i < vertices.size(); i++) {
        ret.addPoint(matrix.transform(vertices[i]));
    }

    return ret;
}
