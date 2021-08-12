#include "RoomInstance.h"

RoomInstance::RoomInstance(const Room& room) : room(room) { }

void RoomInstance::render() const {
	room.render(transform.getModelMatrix());
}
