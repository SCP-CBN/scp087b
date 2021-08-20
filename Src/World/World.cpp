#include "World.h"

#include <iostream>

#include "../Graphics/Rooms/RoomInstance.h"
#include "../Graphics/Camera.h"
#include "../Collision/Collider.h"
#include "../Utilities/Directories.h"
#include "../Graphics/Text/TextRenderer.h"
#include "StatWorld.h"

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

static Font* font;
static TextRenderer* text;
//

World::World(TimeMaster& tm) : tm(tm) {
    TimeMaster ctor;
    Timer* _ = new Timer(ctor, "all");

    constexpr int WIDTH = 999; constexpr int HEIGHT = 666;

    { Timer _(ctor, "gfx");
        screenMiddle = Vector2f(WIDTH, HEIGHT) / 2;
        graphics = Graphics::create("SCP-087-B", WIDTH, HEIGHT);
        graphics->setVsync(false);
    }

    camera = new Camera(WIDTH, HEIGHT, 90);
    resources = new Resources(*graphics, *camera);

    font = new Font(*resources, Directories::GFX + "Vegur");
    text = new TextRenderer(*resources, *font);
    text->setScale(10.f);
    text->setText("Funny pog pog funny");
    text->setPosition(Vector2f(50.f, -50.f));

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
    delete text;
    delete font;
    delete resources;
    delete camera;
    delete graphics;
}

void World::run() {
    { Timer _(tm, "update");
        SysEvents::update();
        graphics->update();
        inputManager->update();

        if (escape->isHit()) {
            togglePaused();
        }

        // During one second delta will have been about this much in sum.
        constexpr int TICKS_PER_SECOND = 60;
        constexpr float CLOCK_TIME_SECOND = (float)Clock::period::den / Clock::period::num;
        constexpr float CLOCK_TIME_PER_TICK = CLOCK_TIME_SECOND / TICKS_PER_SECOND;

        std::chrono::time_point<Clock> now = Clock::now();
        u64 diff = (now - prev).count();

        accumulator += diff;
        if (accumulator >= CLOCK_TIME_SECOND) {
            text->setText(String::from(fps));
            accumulator = 0;
            fps = 0;
        }
        fps++;

        float delta = diff / CLOCK_TIME_PER_TICK;
        prev = now;

        if (!paused) {
            Vector3f addPos;

            constexpr float SPEED = 10.f;
            if (forward->isDown()) {
                addPos += camera->getForward() * SPEED;
            }
            if (back->isDown()) {
                addPos -= camera->getForward() * SPEED;
            }
            if (right->isDown()) {
                addPos -= camera->getForward().crossProduct(camera->getUpward()) * SPEED;
            }
            if (left->isDown()) {
                addPos += camera->getForward().crossProduct(camera->getUpward()) * SPEED;
            }

            { Timer _(tm, "coll");
                camera->setPosition(coll.tryMove(camera->getPosition(), camera->getPosition() + addPos * delta));
            }

            if (inputManager->getMousePosition() != screenMiddle) {
                Vector2f diff = (inputManager->getMousePosition() - screenMiddle) / 1000.f;
                camera->setRotation(camera->getRotation() + Vector3f(diff.y, diff.x, 0.f));
                inputManager->setMousePosition(screenMiddle);
            }
        }
    }

    { Timer _(tm, "render");
        
        { Timer _(tm, "clear");
            graphics->clear(Colors::BLUE);
        }

        { Timer _(tm, "cam");
            camera->applyTransforms();
        }

        { Timer _(tm, "inst");
            inst->render();
            inst2->render();
        }

        { Timer _(tm, "text");
            text->render();
        }

        { Timer _(tm, "swap");
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
