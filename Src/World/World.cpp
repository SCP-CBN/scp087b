#include "World.h"

#include "../Graphics/Rooms/RoomInstance.h"

using namespace PGE;

//
static Room* room;
static RoomInstance* inst;
//

World::World() {
    graphics = Graphics::create("SCP-087-B", 999, 666);
    resources = new Resources(*graphics);

    room = new Room(*resources, FilePath::fromStr("GFX/sdf.b"));
    inst = new RoomInstance(*room);

    //
    auto roomS = resources->getShader(FilePath::fromStr("GFX/Shaders/Room/"));
    roomS->getVertexShaderConstant("viewMatrix").setValue(Matrix4x4f::constructViewMat(Vectors::ZERO3F, Vector3f(0.f, 0.f, 1.f), Vector3f(0.f, 1.f, 0.f)));
    roomS->getVertexShaderConstant("projectionMatrix").setValue(Matrix4x4f::constructPerspectiveMat(90.f, 3.f, 0.01f, 1000.f));
    roomS.drop();
    //
}

World::~World() {
    delete inst;
    delete room;
    delete resources;
    delete graphics;
}

void World::run() {
    SysEvents::update();
    graphics->update();
    graphics->clear(Colors::BLUE);
    inst->render();
    graphics->swap();
}

bool World::shouldEnd() const {
    return !graphics->isWindowOpen();
}
