#include "RoomInstance.h"

using namespace PGE;

RoomInstance::RoomInstance(const Room& room, CollisionMeshCollection& cmc)
	: room(room), cmc(cmc) { }

RoomInstance::~RoomInstance() {
	if (cHandle.valid()) {
		cmc.removeInstance(cHandle);
	}
}

void RoomInstance::render() const {
	room.render(transform.getModelMatrix());
}

void RoomInstance::updateCollision() {
	if (cHandle.valid()) {
		cHandle.update(transform.getModelMatrix());
	}
}

bool RoomInstance::pointInBB(const Vector3f& pos) const {
	return cHandle.pointInBB(pos);
}

void RoomInstance::setCollision(bool coll) {
	if (cHandle.valid()) {
		if (!coll) {
			cmc.removeInstance(cHandle);
		}
	} else {
		if (coll) {
			cHandle = cmc.addInstance(room.getCollisionMesh(), transform.getModelMatrix());
		}
	}
}

void RoomInstance::setPosition(const Vector3f& pos) {
	transform.setPosition(pos);
	updateCollision();
}

const Vector3f& RoomInstance::getPosition() const {
	return transform.getPosition();
}

void RoomInstance::setRotation(const Vector3f& rot) {
	transform.setRotation(rot);
	updateCollision();
}
