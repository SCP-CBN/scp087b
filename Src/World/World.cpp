#include "World.h"

#include "../Graphics/Rooms/RoomInstance.h"
#include "../Graphics/Camera.h"

using namespace PGE;

// Shit that needs a proper place.
static Room* room;
static RoomInstance* inst;

static std::unique_ptr<Input> forward = std::make_unique<KeyboardInput>(KeyboardInput::Keycode::W);
static std::unique_ptr<Input> right = std::make_unique<KeyboardInput>(KeyboardInput::Keycode::D);
static std::unique_ptr<Input> left = std::make_unique<KeyboardInput>(KeyboardInput::Keycode::A);
static std::unique_ptr<Input> back = std::make_unique<KeyboardInput>(KeyboardInput::Keycode::S);
static std::unique_ptr<Input> escape = std::make_unique<KeyboardInput>(KeyboardInput::Keycode::ESCAPE);

static Vector2f screenMiddle;
//

World::World() {
    constexpr int WIDTH = 999; constexpr int HEIGHT = 666;
    screenMiddle = Vector2f(WIDTH, HEIGHT) / 2;
    graphics = Graphics::create("SCP-087-B", WIDTH, HEIGHT);

    camera = new Camera(WIDTH, HEIGHT, 90);
    resources = new Resources(*graphics, *camera);

    inputManager = InputManager::create(*graphics);

    //
    // Do we *need* to untrack these?
    inputManager->trackInput(forward.get());
    inputManager->trackInput(right.get());
    inputManager->trackInput(left.get());
    inputManager->trackInput(back.get());
    inputManager->trackInput(escape.get());

    room = new Room(*resources, FilePath::fromStr("GFX/sdf.b"));
    inst = new RoomInstance(*room);
    //

    togglePaused();
}

World::~World() {
    delete inst;
    delete room;
    delete inputManager;
    delete resources;
    delete camera;
    delete graphics;
}

void World::run() {
    SysEvents::update();
    graphics->update();
    inputManager->update();

    if (escape->isHit()) {
        togglePaused();
    }

    constexpr float SPEED = 10.f;

    if (!paused) {
        if (forward->isDown()) {
            camera->setPosition(camera->getPosition() + camera->getForward() * SPEED);
        }
        if (back->isDown()) {
            camera->setPosition(camera->getPosition() - camera->getForward() * SPEED);
        }
        if (right->isDown()) {
            camera->setPosition(camera->getPosition() - camera->getForward().crossProduct(camera->getUpward()) * SPEED);
        }
        if (left->isDown()) {
            camera->setPosition(camera->getPosition() + camera->getForward().crossProduct(camera->getUpward()) * SPEED);
        }

        if (inputManager->getMousePosition() != screenMiddle) {
            Vector2f diff = (inputManager->getMousePosition() - screenMiddle) / 1000.f;
            camera->setRotation(camera->getRotation() + Vector3f(diff.y, diff.x, 0.f));
            inputManager->setMousePosition(screenMiddle);
        }
    }

    graphics->clear(Colors::BLUE);
    camera->applyTransforms();
    inst->render();
    graphics->swap();
}

bool World::shouldEnd() const {
    return !graphics->isWindowOpen();
}

void World::togglePaused() {
    paused = !paused;
    inputManager->setMouseVisibility(paused);
    inputManager->setMousePosition(screenMiddle);
}
