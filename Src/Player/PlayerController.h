#ifndef B_PLAYERCONTROLLER_H_INCLUDED
#define B_PLAYERCONTROLLER_H_INCLUDED

#include <PGE/Input/InputManager.h>

#include "../Collision/Collider.h"

using namespace PGE;

class Camera;

class PlayerController {
	public:
		PlayerController(PGE::InputManager& inputManager, Camera& camera, CollisionMeshCollection& cmc, float playerHeight);
		
		void update(float delta);
		void setPosition(const PGE::Vector3f& inPos);

	private:
		bool noClip = false;
		static constexpr float SPEED = 10.f;
		static constexpr float GRAVITY = 9.8f / 60.f;
	
		Vector3f playerPos;
		Vector3f camOffset; // Head height

		float vertVel;

		Camera& camera;

		Collider collider;

		std::unique_ptr<Input> forward;
		std::unique_ptr<Input> right;
		std::unique_ptr<Input> left;
		std::unique_ptr<Input> back;
		std::unique_ptr<Input> n;

		PGE::InputManager& inputManager;
};

#endif // B_PLAYERCONTROLLER_H_INCLUDED
