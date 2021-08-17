#ifndef B_ROOMINSTANCE_H_INCLUDED
#define B_ROOMINSTANCE_H_INCLUDED

#include "Room.h"
#include "../Transform.h"
#include "../../Collision/CollisionMeshCollection.h"

class RoomInstance {
	public:
		RoomInstance(const Room& room, CollisionMeshCollection& cmc);
		~RoomInstance();

		void render() const;

		void setPosition(const PGE::Vector3f& pos);

	private:
		const Room& room;
		Transform transform;

		CollisionMeshCollection& cmc;
		CollisionMeshCollection::Handle cHandle;
};

#endif // B_ROOMINSTANCE_H_INCLUDED
