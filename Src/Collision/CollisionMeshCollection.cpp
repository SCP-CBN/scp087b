#include "CollisionMeshCollection.h"

#include <PGE/Exception/Exception.h>

using namespace PGE;

CollisionMeshCollection::Instance::Instance(const CollisionMesh& msh, const Matrix4x4f& mtrx)
    : mesh(msh) {
    mat = mtrx;
    recalculateBoundingBox();
}

Collision CollisionMeshCollection::Instance::checkCollision(const Line3f& line, float height, float radius) const {
    return mesh.checkCollision(mat, line, height, radius);
}

AABBox CollisionMeshCollection::Instance::getBoundingBox() const {
    return bbox;
}

void CollisionMeshCollection::Instance::updateMatrix(const Matrix4x4f& newMat) {
    mat = newMat;
    recalculateBoundingBox();
}

void CollisionMeshCollection::Instance::recalculateBoundingBox() {
    bbox = mesh.calculateBoundingBox(mat);
}

CollisionMeshCollection::Handle::Handle(int idx, Instance& inst)
	: index(idx), instance(&inst) { }

void CollisionMeshCollection::Handle::update(const Matrix4x4f& mat) const {
	instance->updateMatrix(mat);
}

bool CollisionMeshCollection::Handle::valid() const {
	return instance != nullptr;
}

CollisionMeshCollection::Handle CollisionMeshCollection::addInstance(const CollisionMesh& mesh, const Matrix4x4f& matrix) {
    int freeInt;
    if (freeInts.empty()) {
        freeInt = instances.size();
    } else {
        freeInt = freeInts.top();
        freeInts.pop();
    }

    return Handle(freeInt, instances.emplace(freeInt, Instance(mesh, matrix)).first->second);
}

void CollisionMeshCollection::removeInstance(Handle& hdl) {
    instances.erase(instances.find(hdl.index));
    freeInts.push(hdl.index);
    hdl = Handle(); // Reset the handle.
}

Collision CollisionMeshCollection::checkCollision(const Line3f& line, float height, float radius) const {
    Collision retVal; retVal.hit = false;

    AABBox lineBox(line.pointA);
    lineBox.addPoint(line.pointB);
    lineBox.addPoint(lineBox.getMin() + Vector3f(-radius - 0.5f, -height * 0.5f - 0.5f, -radius - 0.5f));
    lineBox.addPoint(lineBox.getMax() + Vector3f(radius + 0.5f, height * 0.5f + 0.5f, radius + 0.5f));

    for (const auto& it : instances) {
        AABBox bbox = it.second.getBoundingBox();
        if (!bbox.intersects(lineBox)) { continue; }
        Collision coll = it.second.checkCollision(line, height, radius);
        if (coll.hit) {
            if (!retVal.hit || retVal.coveredAmount > coll.coveredAmount) {
                retVal = coll;
            }
        }
    }
    return retVal;
}
