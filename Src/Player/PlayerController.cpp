#include "PlayerController.h"

#include "../Graphics/Camera.h"

PlayerController::PlayerController(PGE::InputManager* inputManager, Camera* camera, CollisionMeshCollection* coMeCo, float playerHeight, Vector2f screenMiddle) 
	: camera(camera), inputManager(inputManager), collider(10.f, playerHeight), camOffset(0, playerHeight/2 - 5, 0), screenMiddle(screenMiddle) {
	forward = std::make_unique<KeyboardInput>(KeyboardInput::Keycode::W);
	right = std::make_unique<KeyboardInput>(KeyboardInput::Keycode::D);
	left = std::make_unique<KeyboardInput>(KeyboardInput::Keycode::A);
	back = std::make_unique<KeyboardInput>(KeyboardInput::Keycode::S);
	n = std::make_unique<KeyboardInput>(KeyboardInput::Keycode::N);

	inputManager->trackInput(forward.get());
	inputManager->trackInput(right.get());
	inputManager->trackInput(left.get());
	inputManager->trackInput(back.get());
	inputManager->trackInput(n.get());

	collider.setCollisionMeshCollection(coMeCo);
}

void PlayerController::update(float delta) {
	Vector3f addPos;
	
	// Using camera direction (possibly temporary)
	if (forward->isDown()) {
		addPos += camera->getForward();
	}
	if (back->isDown()) {
		addPos -= camera->getForward();
	}
	if (right->isDown()) {
		addPos -= camera->getForward().crossProduct(camera->getUpward());
	}
	if (left->isDown()) {
		addPos += camera->getForward().crossProduct(camera->getUpward());
	}
	if (n->isHit()) {
		noClip = !noClip;
	}

	// Stop two inputs making speed root 2.
	addPos = addPos.normalize() * speed;
	addPos.y = noClip ? addPos.y : 0;

	Vector3f toPos = playerPos + addPos * delta;

	toPos.y += noClip ? 0 : vertVel - gravity * delta;
	float oldY = playerPos.y;

	setPosition(noClip ? toPos : collider.tryMove(playerPos, toPos));
	vertVel = playerPos.y - oldY;

	camera->setPosition(playerPos + camOffset);

	// Mouse Control
	if (inputManager->getMousePosition() != screenMiddle) {
		Vector2f diff = inputManager->consumeMouseDelta() / 1000.f;
		if (abs(camera->getRotation().x) >= 0.5f * Math::PI) { diff.x = -diff.x; }
		Vector3f newRot = camera->getRotation() + Vector3f(diff.y, diff.x, 0.f);
		newRot.x = fmod(newRot.x, 2 * Math::PI);
		newRot.y = fmod(newRot.y, 2 * Math::PI);
		newRot.z = fmod(newRot.z, 2 * Math::PI);
		camera->setRotation(newRot);
	}
}

void PlayerController::setPosition(PGE::Vector3f inPos) {
	playerPos = inPos;
}
