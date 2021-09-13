#ifndef B_ROOMSET_H_INCLUDED
#define B_ROOMSET_H_INCLUDED

#include <PGE/Math/Random.h>

#include "RoomInfo.h"

class RoomSet {
	public:
		RoomSet(const std::vector<IRoomInfo*>& infos);

		void load(Resources& res);
		void unload();

		const IRoomInfo& getRoom(int index) const;
		const IRoomInfo& getRandomRoom(PGE::Random& random) const;

		int getCount() const;

	private:
		std::vector<std::unique_ptr<IRoomInfo>> rooms;
		int totalWeight;
};

#endif // B_ROOMSET_H_INCLUDED
