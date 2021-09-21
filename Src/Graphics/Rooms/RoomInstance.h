#ifndef B_ROOMINSTANCE_H_INCLUDED
#define B_ROOMINSTANCE_H_INCLUDED

#include "Room.h"
#include "../Transform.h"
#include "../../Collision/CollisionMeshCollection.h"

class RoomInstance {
	public:
		RoomInstance(const Room& room, CollisionMeshCollection& cmc, const Room::RenderInfo& info, const PGE::Vector3f& pos);
		virtual ~RoomInstance();

		virtual void update(float delta) { }
		virtual void tick() { }
		virtual void render(float interp) const;

		// Start updating, rendering & colliding.
		virtual void activate();
		// Stop updating, rendering & colliding.
		virtual void deactive();

		// Reach floor.
		virtual void enter() { }
		// Leave floor.
		virtual void leave() { }

	private:
		const Room& room;
		Transform transform;

		CollisionMeshCollection& cmc;
		CollisionMeshCollection::Handle cHandle;

		Room::RenderInfo info;
};

#endif // B_ROOMINSTANCE_H_INCLUDED
