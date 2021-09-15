#ifndef B_ROOMINSTANCE_H_INCLUDED
#define B_ROOMINSTANCE_H_INCLUDED

#include "Room.h"
#include "../Transform.h"
#include "../../Collision/CollisionMeshCollection.h"

class RoomInstance {
	public:
		RoomInstance(const Room& room, CollisionMeshCollection& cmc, const PGE::Vector2f& uvOff, float uvRot);
		~RoomInstance();

		void render() const;

		void setCollision(bool coll);

		void setPosition(const PGE::Vector3f& pos);
		void setRotation(const PGE::Vector3f& rot);

		const CollisionMeshCollection::Handle& getCollisionHandle() const;

		const PGE::Vector3f& getPosition() const;

	private:
		void updateCollision();

		const Room& room;
		Transform transform;

		CollisionMeshCollection& cmc;
		CollisionMeshCollection::Handle cHandle;

		PGE::Vector2f uvOff;
		float uvRot;
};

#endif // B_ROOMINSTANCE_H_INCLUDED
