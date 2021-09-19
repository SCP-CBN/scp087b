#ifndef B_PLAYERCONTROLLER_H_INCLUDED
#define B_PLAYERCONTROLLER_H_INCLUDED

#include <PGE/Input/InputManager.h>

#include "../Collision/Collider.h"

using namespace PGE;

class PlayerController {
	public:
		PlayerController(PGE::InputManager* inputManager, class Camera* camera, CollisionMeshCollection* coMeCo, float playerHeight, bool noclip, Vector2f screenMiddle);
		
		void update(float delta);
		void setPosition(PGE::Vector3f inPos);

	private:
		bool noclip;
		static constexpr float speed = 10.f;
		static constexpr float gravity = 9.8f / 60.f;
	
		Vector3f playerPos;
		Vector3f camOffset; // Head height

		float vertVel;

		Vector2f screenMiddle;

		class Camera* camera;

		Collider collider;

		std::unique_ptr<Input> forward;
		std::unique_ptr<Input> right;
		std::unique_ptr<Input> left;
		std::unique_ptr<Input> back;

		PGE::InputManager* inputManager;
};

#endif // B_PLAYERCONTROLLER_H_INCLUDED
