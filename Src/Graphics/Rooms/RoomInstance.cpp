#include "RoomInstance.h"

using namespace PGE;

RoomInstance::RoomInstance(const Room& room, CollisionMeshCollection& cmc)
	: room(room), cmc(cmc) {
	cHandle = cmc.addInstance(room.getCollisionMesh(), transform.getModelMatrix());
}

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

void RoomInstance::setPosition(const Vector3f& pos) {
	transform.setPosition(pos);
	updateCollision();
}

void RoomInstance::setRotation(const Vector3f& rot) {
	transform.setRotation(rot);
	updateCollision();
}
