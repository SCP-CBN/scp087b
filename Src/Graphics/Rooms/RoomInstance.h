#ifndef B_ROOMINSTANCE_H_INCLUDED
#define B_ROOMINSTANCE_H_INCLUDED

#include "Room.h"
#include "../Transform.h"

class RoomInstance {
	public:
		RoomInstance(const Room& room);

		void render() const;

		void setPosition(const PGE::Vector3f& pos);

	private:
		const Room& room;
		Transform transform;
};

#endif // B_ROOMINSTANCE_H_INCLUDED
