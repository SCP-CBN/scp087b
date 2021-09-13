#include "RoomSet.h"

using namespace PGE;

RoomSet::RoomSet(const std::vector<IRoomInfo*>& infos) {
	totalWeight = -1;
	rooms.reserve(infos.size());
	for (IRoomInfo* r : infos) {
		rooms.push_back(std::unique_ptr<IRoomInfo>(r));
		totalWeight += r->getWeight();
	}
}

void RoomSet::load(Resources& res) {
	for (auto& r : rooms) {
		r->load(res);
	}
}

void RoomSet::unload() {
	for (auto& r : rooms) {
		r->unload();
	}
}

const IRoomInfo& RoomSet::getRoom(int index) const {
	return *rooms[index];
}

const IRoomInfo& RoomSet::getRandomRoom(Random& random) const {
	int selected = random.nextInt(totalWeight);
	for (const auto& r : rooms) {
		selected -= r->getWeight();
		if (selected < 0) {
			return *r;
		}
	}
	throw PGE_CREATE_EX("Could not get random room!");
}

int RoomSet::getCount() const {
	return rooms.size();
}
