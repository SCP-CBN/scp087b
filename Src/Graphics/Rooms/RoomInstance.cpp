#include "RoomInstance.h"

using namespace PGE;

RoomInstance::RoomInstance(const Room& room) : room(room) { }

void RoomInstance::render() const {
	room.render(transform.getModelMatrix());
}

void RoomInstance::setPosition(const Vector3f& pos) {
	transform.setPosition(pos);
}
