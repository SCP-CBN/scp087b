#include "World.h"

#include <iostream>

#include "../Timing/TimeMaster.h"

#include "../Graphics/Rooms/RoomInstance.h"
#include "../Graphics/Camera.h"
#include "../Collision/Collider.h"
#include "../Utilities/Directories.h"

using namespace PGE;

// Shit that needs a proper place.
static Room* room;
static RoomInstance* inst;
static RoomInstance* inst2;

static CollisionMeshCollection cmc;
static Collider coll = Collider(10, 50);

static std::unique_ptr<Input> forward = std::make_unique<KeyboardInput>(KeyboardInput::Keycode::W);
static std::unique_ptr<Input> right = std::make_unique<KeyboardInput>(KeyboardInput::Keycode::D);
static std::unique_ptr<Input> left = std::make_unique<KeyboardInput>(KeyboardInput::Keycode::A);
static std::unique_ptr<Input> back = std::make_unique<KeyboardInput>(KeyboardInput::Keycode::S);
static std::unique_ptr<Input> escape = std::make_unique<KeyboardInput>(KeyboardInput::Keycode::ESCAPE);

static Vector2f screenMiddle;

TimeMaster master;
//

World::World() {
    TimeMaster ctor;
    Timer* _ = new Timer(ctor, "all");

    constexpr int WIDTH = 999; constexpr int HEIGHT = 666;

    { Timer _(ctor, "gfx");
        screenMiddle = Vector2f(WIDTH, HEIGHT) / 2;
        graphics = Graphics::create("SCP-087-B", WIDTH, HEIGHT);
    }

    camera = new Camera(WIDTH, HEIGHT, 90);
    resources = new Resources(*graphics, *camera);

    { Timer _(ctor, "input");
        inputManager = InputManager::create(*graphics);

        //
        // Do we *need* to untrack these?
        inputManager->trackInput(forward.get());
        inputManager->trackInput(right.get());
        inputManager->trackInput(left.get());
        inputManager->trackInput(back.get());
        inputManager->trackInput(escape.get());
    }

    { Timer _(ctor, "room");
        room = new Room(*resources, Directories::ROOMS + "default.b");
        inst = new RoomInstance(*room, cmc);
        inst2 = new RoomInstance(*room, cmc);
        inst2->setPosition(Vector3f(800.f, -200.f, -700.f));
        inst2->setRotation(Vector3f(0.f, Math::degToRad(180.f), 0.f));
    }

    coll.setCollisionMeshCollection(&cmc);
    //

    togglePaused();

    delete _;
    std::cout << ctor.print() << std::endl;
}

World::~World() {
    delete inst2;
    delete inst;
    delete room;
    delete inputManager;
    delete resources;
    delete camera;
    delete graphics;
}

void World::run() {
    { Timer _(master, "update");
        SysEvents::update();
        graphics->update();
        inputManager->update();

        if (escape->isHit()) {
            std::cout << master.print() << std::endl;;
            togglePaused();
        }

        if (!paused) {
            Vector3f goalPos = camera->getPosition();;

            constexpr float SPEED = 10.f;
            if (forward->isDown()) {
                goalPos += camera->getForward() * SPEED;
            }
            if (back->isDown()) {
                goalPos -= camera->getForward() * SPEED;
            }
            if (right->isDown()) {
                goalPos -= camera->getForward().crossProduct(camera->getUpward()) * SPEED;
            }
            if (left->isDown()) {
                goalPos += camera->getForward().crossProduct(camera->getUpward()) * SPEED;
            }

            { Timer _(master, "coll");
                camera->setPosition(coll.tryMove(camera->getPosition(), goalPos));
            }

            if (inputManager->getMousePosition() != screenMiddle) {
                Vector2f diff = (inputManager->getMousePosition() - screenMiddle) / 1000.f;
                camera->setRotation(camera->getRotation() + Vector3f(diff.y, diff.x, 0.f));
                inputManager->setMousePosition(screenMiddle);
            }
        }
    }

    { Timer _(master, "render");
        
        { Timer _(master, "clear");
            graphics->clear(Colors::BLUE);
        }

        { Timer _(master, "cam");
            camera->applyTransforms();
        }

        { Timer _(master, "inst");
            inst->render();
            inst2->render();
        }

        { Timer _(master, "swap");
            graphics->swap();
        }
    }
}

bool World::shouldEnd() const {
    return !graphics->isWindowOpen();
}

void World::togglePaused() {
    paused = !paused;
    inputManager->setMouseVisibility(paused);
    inputManager->setMousePosition(screenMiddle);
}
