#include "RoomInstance.h"

using namespace PGE;

RoomInstance::RoomInstance(const Room& room, CollisionMeshCollection& cmc, const Room::RenderInfo& info, const Vector3f& pos)
	: room(room), cmc(cmc), info(info) {
	transform.setRotation(Vector3f(0.f, info.rotation, 0.f));
	transform.setPosition(pos);
}

RoomInstance::~RoomInstance() {
#ifdef DEBUG
	PGE_ASSERT(!cHandle.valid(), "Tried destroying room without deactivating");
#endif
	if (cHandle.valid()) {
		cmc.removeInstance(cHandle);
	}
}

void RoomInstance::render(float interp) const {
	room.render(transform.getModelMatrix(), info);
}

void RoomInstance::activate() {
	PGE_ASSERT(!cHandle.valid(), "Room was already activated!");
	cHandle = cmc.addInstance(room.getCollisionMesh(), transform.getModelMatrix());
}

void RoomInstance::deactive() {
	PGE_ASSERT(cHandle.valid(), "Room was not active!");
	cmc.removeInstance(cHandle);
}
