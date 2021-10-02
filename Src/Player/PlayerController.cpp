#include "PlayerController.h"

#include "../Graphics/Camera.h"

using namespace PGE;

PlayerController::PlayerController(InputManager& inputManager, Camera& camera, CollisionMeshCollection& cmc, float playerHeight, World& world) 
	: camera(camera), inputManager(inputManager), collider(10.f, playerHeight), camOffset(0, playerHeight /2 - 5.f, 0), world(world) {
	forward = std::make_unique<KeyboardInput>(KeyboardInput::Keycode::W);
	right = std::make_unique<KeyboardInput>(KeyboardInput::Keycode::D);
	left = std::make_unique<KeyboardInput>(KeyboardInput::Keycode::A);
	back = std::make_unique<KeyboardInput>(KeyboardInput::Keycode::S);
	n = std::make_unique<KeyboardInput>(KeyboardInput::Keycode::N);

	one = std::make_unique<KeyboardInput>(KeyboardInput::Keycode::NUM1);
	two = std::make_unique<KeyboardInput>(KeyboardInput::Keycode::NUM2);
	three = std::make_unique<KeyboardInput>(KeyboardInput::Keycode::NUM3);
	escape = std::make_unique<KeyboardInput>(KeyboardInput::Keycode::ESCAPE);
	flash = std::make_unique<KeyboardInput>(KeyboardInput::Keycode::F);
	checky = std::make_unique<KeyboardInput>(KeyboardInput::Keycode::RCTRL);

	inputManager.trackInput(forward.get());
	inputManager.trackInput(right.get());
	inputManager.trackInput(left.get());
	inputManager.trackInput(back.get());
	inputManager.trackInput(n.get());
	inputManager.trackInput(flash.get());

	//UserInterface block
	inputManager.trackInput(one.get());
	inputManager.trackInput(two.get());
	inputManager.trackInput(three.get());
	inputManager.trackInput(escape.get());
	for (int i = 0; i < 12; i++) {
		debug[i] = std::make_unique<KeyboardInput>((KeyboardInput::Keycode)((int)KeyboardInput::Keycode::F1 + i));
		inputManager.trackInput(debug[i].get());
	}

	collider.setCollisionMeshCollection(&cmc);
}

void PlayerController::update(float delta) {
	Vector3f addPos;
	
	// Using camera direction (possibly temporary)
	if (forward->isDown()) {
		addPos += camera.getForward();
	}
	if (back->isDown()) {
		addPos -= camera.getForward();
	}
	if (right->isDown()) {
		addPos -= camera.getForward().crossProduct(camera.getUpward());
	}
	if (left->isDown()) {
		addPos += camera.getForward().crossProduct(camera.getUpward());
	}
	if (n->isHit()) {
		noClip = !noClip;
	}
	if (flash->isHit()) {
		world.swapLight();
	}

	// Stop two inputs making speed root 2.
	addPos = addPos.normalize() * SPEED;
	addPos.y = noClip ? addPos.y : 0;

	Vector3f toPos = playerPos + addPos * delta;

	toPos.y += noClip ? 0 : vertVel - GRAVITY * delta;
	float oldY = playerPos.y;

	setPosition(noClip ? toPos : collider.tryMove(playerPos, toPos));
	vertVel = playerPos.y - oldY;

	camera.setPosition(playerPos + camOffset);

	// Mouse Control
	Vector2f diff = inputManager.consumeMouseDelta() / 1000.f;
	if (!diff.equals(Vectors::ZERO2F)) {
		if (abs(camera.getRotation().x) >= 0.5f * Math::PI) { diff.x = -diff.x; }
		Vector3f newRot = camera.getRotation() + Vector3f(diff.y, diff.x, 0.f);
		newRot.x = fmod(newRot.x, 2 * Math::PI);
		newRot.y = fmod(newRot.y, 2 * Math::PI);
		newRot.z = fmod(newRot.z, 2 * Math::PI);
		camera.setRotation(newRot);
	}
}

void PlayerController::setPosition(const Vector3f& inPos) {
	playerPos = inPos;
}

void PlayerController::nonMoveUpdates() {
	if (one->isHit()) {
		world.swapFps();
	}
	if (two->isHit()) {
		world.swapPos();
	}
	if (three->isHit()) {
		world.swapId();
	}
	if (escape->isHit()) {
		world.togglePaused();
	}
	for (unsigned i = 0; i < 12; i++) {
		if (debug[i]->isHit()) {
			world.getResources()->getRoomShader().getFragmentShaderConstant("debug").setValue(i);
		}
	}
	if (checky->isHit()) {
		for (const IRoomInfo* r : world.getRoomset()) {
			r->getRoom().toggleDebug();
		}
	}
}
