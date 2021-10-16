#include "RoomSet.h"

using namespace PGE;

RoomSet::RoomSet(std::vector<IRoomInfo*>&& infos) {
	totalWeight = -1;
	for (IRoomInfo* r : infos) {
		totalWeight += r->getWeight();
	}
	rooms = std::move(infos);
}

RoomSet::~RoomSet() {
	for (IRoomInfo* r : rooms) {
		delete r;
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
	return (int)rooms.size();
}

const IRoomInfo* const* RoomSet::begin() const {
	return &rooms.front();
}

const IRoomInfo* const* RoomSet::end() const {
	return &rooms.back() + 1;
}
