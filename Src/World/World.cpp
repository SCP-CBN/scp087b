#include "World.h"

using namespace PGE;

World::World() {
    graphics = Graphics::create("SCP-087-B", 999, 666);
}

World::~World() {
    delete graphics;
}

void World::run() {
    SysEvents::update();
    graphics->update();
}

bool World::shouldEnd() const {
    return !graphics->isWindowOpen();
}
