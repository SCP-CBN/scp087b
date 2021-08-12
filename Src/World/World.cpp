#include "World.h"

#include "../Graphics/Rooms/RoomInstance.h"
#include "../Graphics/Camera.h"

using namespace PGE;

//
static Room* room;
static RoomInstance* inst;
//

World::World() {
    constexpr int WIDTH = 999; constexpr int HEIGHT = 666;
    graphics = Graphics::create("SCP-087-B", WIDTH, HEIGHT);
    camera = new Camera(WIDTH, HEIGHT, 90);
    resources = new Resources(*graphics, *camera);

    room = new Room(*resources, FilePath::fromStr("GFX/sdf.b"));
    inst = new RoomInstance(*room);
}

World::~World() {
    delete inst;
    delete room;
    delete resources;
    delete camera;
    delete graphics;
}

void World::run() {
    SysEvents::update();
    graphics->update();
    graphics->clear(Colors::BLUE);
    camera->applyTransforms();
    inst->render();
    graphics->swap();
}

bool World::shouldEnd() const {
    return !graphics->isWindowOpen();
}
