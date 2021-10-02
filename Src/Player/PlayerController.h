#ifndef B_PLAYERCONTROLLER_H_INCLUDED
#define B_PLAYERCONTROLLER_H_INCLUDED

#include <PGE/Input/InputManager.h>

#include "../Collision/Collider.h"
#include "../World/World.h"

class Camera;

class PlayerController {
	public:
		PlayerController(PGE::InputManager& inputManager, Camera& camera, CollisionMeshCollection& cmc, float playerHeight, World& world);
		
		void update(float delta);
		void setPosition(const PGE::Vector3f& inPos);
		void nonMoveUpdates();

	private:
		bool noClip = true;
		static constexpr float SPEED = 10.f;
		static constexpr float GRAVITY = 9.8f / 60.f;
	
		PGE::Vector3f playerPos;
		PGE::Vector3f camOffset; // Head height

		float vertVel;

		Camera& camera;

		Collider collider;

		World& world;

		std::unique_ptr<PGE::Input> forward;
		std::unique_ptr<PGE::Input> right;
		std::unique_ptr<PGE::Input> left;
		std::unique_ptr<PGE::Input> back;
		std::unique_ptr<PGE::Input> n;
		std::unique_ptr<PGE::Input> flash;

		std::unique_ptr<PGE::Input> one;
		std::unique_ptr<PGE::Input> two;
		std::unique_ptr<PGE::Input> three;
		std::unique_ptr<PGE::Input> escape;
		std::unique_ptr<PGE::Input> checky;
		std::unique_ptr<PGE::Input> debug[12];

		PGE::InputManager& inputManager;
};

#endif // B_PLAYERCONTROLLER_H_INCLUDED
