#ifndef B_ROOMSET_H_INCLUDED
#define B_ROOMSET_H_INCLUDED

#include <PGE/Math/Random.h>

#include "RoomInfo.h"

class RoomSet {
	public:
		RoomSet(std::vector<IRoomInfo*>&& infos);
		~RoomSet();

		void load(Resources& res);
		void unload();

		const IRoomInfo& getRoom(int index) const;
		const IRoomInfo& getRandomRoom(PGE::Random& random) const;

		int getCount() const;

		const IRoomInfo* begin() const;
		const IRoomInfo* end() const;

	private:
		std::vector<IRoomInfo*> rooms;
		int totalWeight;
};

#endif // B_ROOMSET_H_INCLUDED
