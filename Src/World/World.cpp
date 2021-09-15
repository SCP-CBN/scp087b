#include "World.h"

#include <iostream>

#include <PGE/Math/Random.h>
#include <PGE/Math/Interpolator.h>

#include "../Graphics/Rooms/RoomInstance.h"
#include "../Graphics/Camera.h"
#include "../Collision/Collider.h"
#include "../Utilities/Directories.h"
#include "../Graphics/Text/TextRenderer.h"
#include "StatWorld.h"

using namespace PGE;

// Shit that needs a proper place.
static std::vector<RoomInstance*> instances;
static int currIndex;

static CollisionMeshCollection coMeCo;
static Collider coll = Collider(10, 50);

static std::unique_ptr<Input> forward = std::make_unique<KeyboardInput>(KeyboardInput::Keycode::W);
static std::unique_ptr<Input> right = std::make_unique<KeyboardInput>(KeyboardInput::Keycode::D);
static std::unique_ptr<Input> left = std::make_unique<KeyboardInput>(KeyboardInput::Keycode::A);
static std::unique_ptr<Input> back = std::make_unique<KeyboardInput>(KeyboardInput::Keycode::S);
static std::unique_ptr<Input> escape = std::make_unique<KeyboardInput>(KeyboardInput::Keycode::ESCAPE);
static std::unique_ptr<Input> one = std::make_unique<KeyboardInput>(KeyboardInput::Keycode::NUM1);
static std::unique_ptr<Input> two = std::make_unique<KeyboardInput>(KeyboardInput::Keycode::NUM2);
static std::unique_ptr<Input> three = std::make_unique<KeyboardInput>(KeyboardInput::Keycode::NUM3);
static std::unique_ptr<Input> flash = std::make_unique<KeyboardInput>(KeyboardInput::Keycode::F);
static std::unique_ptr<Input> checky = std::make_unique<KeyboardInput>(KeyboardInput::Keycode::RCTRL);
static std::unique_ptr<Input> n = std::make_unique<KeyboardInput>(KeyboardInput::Keycode::N);

static std::vector<std::unique_ptr<Input>> debug(12);

static Vector2f screenMiddle;

static Font* font;
static TextRenderer* text;
static TextRenderer* posText;
static TextRenderer* idText;

bool showFps = false;
bool showPos = false;
bool showId = false;

bool noclip = false;

static Resources::Handle<Texture> glimpseTex;
static Mesh* glimpseMesh;

constexpr int ROOM_HEIGHT = 200;

static void updateIndex(int newIndex) {
    for (int i = -1; i < 2; i++) {
        int newCheckedIndex = newIndex + i;
        if (newCheckedIndex >= 0 && newCheckedIndex < instances.size()) {
            instances[newIndex + i]->setCollision(true);
        }

        int checkedIndex = currIndex + i;
        if (checkedIndex >= 0 && newCheckedIndex < instances.size() && abs(checkedIndex - newIndex) > 1) {
            instances[checkedIndex]->setCollision(false);
        }
    }
    currIndex = newIndex;
    idText->setText(String::from(currIndex));
    idText->setPosition(Vector2f(-50.f + idText->getWidth(), -50.f));
}
//

World::World(TimeMaster& tm) : tm(tm),
    rooms({
        new RoomInfo("default", 100),
        new RoomInfo("defaultFull", 50)
    }) {

    TimeMaster ctor;
    { Timer _(ctor, "all");
        constexpr int WIDTH = 999; constexpr int HEIGHT = 666;

        { Timer _(ctor, "gfx");
            screenMiddle = Vector2f(WIDTH, HEIGHT) / 2;
            graphics = Graphics::create("SCP-087-B", WIDTH, HEIGHT);
            graphics->setVsync(false);
        }

        camera = new Camera(WIDTH, HEIGHT, 90);
        camera->setPosition(Vector3f(345.f, -45.f, -90.f));
        resources = new Resources(*graphics, *camera);

        { Timer _(ctor, "rooms");
            rooms.load(*resources);
        }

        font = new Font(*resources, Directories::GFX + "Vegur");
        text = new TextRenderer(*resources, *font);
        text->setScale(10.f);
        text->setPosition(Vector2f(50.f, -50.f + text->getHeight()));

        posText = new TextRenderer(*resources, *font);
        posText->setScale(10.f);
        posText->setPosition(Vector2f(50.f, 50.f - text->getHeight()));

        idText = new TextRenderer(*resources, *font);
        idText->setScale(10.f);

        { Timer _(ctor, "input");
            inputManager = InputManager::create(*graphics);

            //
            // Do we *need* to untrack these?
            inputManager->trackInput(forward.get());
            inputManager->trackInput(right.get());
            inputManager->trackInput(left.get());
            inputManager->trackInput(back.get());
            inputManager->trackInput(escape.get());
            inputManager->trackInput(one.get());
            inputManager->trackInput(two.get());
            inputManager->trackInput(three.get());
            inputManager->trackInput(flash.get());
            inputManager->trackInput(checky.get());
            inputManager->trackInput(n.get());

            for (int i = 0; i < 12; i++) {
                debug[i] = std::make_unique<KeyboardInput>((KeyboardInput::Keycode)((int)KeyboardInput::Keycode::F1 + i));
                inputManager->trackInput(debug[i].get());
            }
        }

        { Timer _(ctor, "map");
            constexpr int ROOM_COUNT = 100;
            instances.reserve(ROOM_COUNT);
            Vector3f basePos[2];
            Vector3f baseRot[2];
            basePos[1] = Vector3f(800.f, 0.f, -700.f);
            baseRot[1] = Vector3f(0.f, Math::degToRad(180.f), 0.f);
            Random rand;
            for (int i = 0; i < ROOM_COUNT; i++) {
                RoomInstance* newRoom = rooms.getRandomRoom(rand).instantiate(coMeCo);
                instances.push_back(newRoom);
                newRoom->setPosition(basePos[i % 2] - Vector3f(0.f, (float)(ROOM_HEIGHT * i), 0.f));
                newRoom->setRotation(baseRot[i % 2]);
            }
            updateIndex(0);
        }

        StructuredData data(resources->getGlimpseShader().getVertexLayout(), 4);
        data.setValue(0, "position", Vector3f(-50, -50, 0)); data.setValue(1, "position", Vector3f(50, -50, 0));
        data.setValue(2, "position", Vector3f(-50, 50, 0)); data.setValue(3, "position", Vector3f(50, 50, 0));
        data.setValue(0, "uv", Vector2f(0.f, 1.f)); data.setValue(1, "uv", Vector2f(1.f, 1.f));
        data.setValue(2, "uv", Vector2f(0.f, 0.f)); data.setValue(3, "uv", Vector2f(1.f, 0.f));
        glimpseMesh = Mesh::create(*graphics);
        glimpseMesh->setGeometry(std::move(data), Mesh::PrimitiveType::TRIANGLE, { 0, 1, 2, 3, 2, 1 });
        glimpseTex = resources->getTexture(Directories::TEXTURES + "glimpse.png");
        glimpseMesh->setMaterial(Mesh::Material(resources->getGlimpseShader(), *glimpseTex, Mesh::Material::Opaque::YES));
    

        coll.setCollisionMeshCollection(&coMeCo);
        //

        togglePaused();

    }
    std::cout << ctor.print() << std::endl;
}

World::~World() {
    glimpseTex.drop();
    delete glimpseMesh;
    for (RoomInstance* inst : instances) {
        delete inst;
    }
    rooms.unload();
    delete inputManager;
    delete text;
    delete font;
    delete resources;
    delete camera;
    delete graphics;
}

//
bool lightOn = false;
static Vector3f prevColor;
static Vector3f color;

static Vector3f glimpsePos = Vector3f(250.f, -100.f, -50.f);
//

void World::run() {
    ticker.run();
}

//
constexpr int FUNNY_SIZE = 32;
static float funny[FUNNY_SIZE];
static int funnyIndex = 0;
static float funnySum = 0.f;
static Random randd;
constexpr Vector3f FIRE(0xF5 / 255.f, 0x58 / 255.f, 0x22 / 255.f);
//

bool World::update(float delta) {
    SysEvents::update();
    graphics->update();
    inputManager->update();

    if (escape->isHit()) { togglePaused(); }

    if (one->isHit()) { showFps = !showFps; }
    if (two->isHit()) { showPos = !showPos; }
    if (three->isHit()) { showId = !showId; }

    if (flash->isHit()) { lightOn = !lightOn; }

    if (n->isHit()) { noclip = !noclip; }
    
    if (checky->isHit()) {
        for (const IRoomInfo* r : rooms) {
            r->getRoom().toggleDebug();
        }
    }

    if (!graphics->isWindowFocused() && !paused) {
        togglePaused();
    }

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

        {
            Timer _(tm, "coll");
            Vector3f camPos = camera->getPosition();
            Vector3f toPos = camPos + addPos * delta;
            camera->setPosition(noclip ? toPos : coll.tryMove(camPos, toPos));
            resources->getRoomShader().getFragmentShaderConstant("lightPos").setValue(camera->getPosition());
            if (int newIndex = (int)(-camera->getPosition().y / ROOM_HEIGHT); currIndex != newIndex) {
                updateIndex(newIndex);
            }
            posText->setText("X: " + String::from(camera->getPosition().x) + '\n'
                + "Y: " + String::from(camera->getPosition().y) + '\n'
                + "Z: " + String::from(camera->getPosition().z) + '\n'
            );
        }

        if (inputManager->getMousePosition() != screenMiddle) {
            Vector2f diff = (inputManager->getMousePosition() - screenMiddle) / 1000.f;
            if (abs(camera->getRotation().x) >= 0.5f * Math::PI) { diff.x = -diff.x; }
            Vector3f newRot = camera->getRotation() + Vector3f(diff.y, diff.x, 0.f);
            newRot.x = fmod(newRot.x, 2 * Math::PI);
            newRot.y = fmod(newRot.y, 2 * Math::PI);
            newRot.z = fmod(newRot.z, 2 * Math::PI);
            camera->setRotation(newRot);
            inputManager->setMousePosition(screenMiddle);
        }
    }
    
    text->setText(String::from(ticker.getFPS()) + "\n" + String::from(ticker.getInterpFPS()));

    return !shouldEnd();
}

void World::render(float interp) const {
    for (unsigned i = 0; i < 12; i++) {
        if (debug[i]->isHit()) {
            resources->getRoomShader().getFragmentShaderConstant("debug").setValue(i);
        }
    }

    resources->getRoomShader().getFragmentShaderConstant("intensity").setValue(Interpolator::lerp(prevColor, color, interp));

    { Timer _(tm, "clear");
        graphics->clear(Colors::GRAY);
    }

    { Timer _(tm, "cam");
        camera->applyTransforms();
    }

    { Timer _(tm, "inst");
        for (int i = std::max(0, currIndex - 1); i < std::min((int)instances.size(), currIndex + 2); i++) {
            resources->getRoomShader().getVertexShaderConstant("uvOff").setValue(i % 2 == 0 ? Vector2f() :  Vector2f(-6.6774f, -5.395f));
            resources->getRoomShader().getVertexShaderConstant("uvScale").setValue(i % 2 == 0 ? Vector2f(1) : Vector2f(-1));
            instances[i]->render();
        }
    }

    resources->getGlimpseShader().getVertexShaderConstant("worldMatrix").setValue(Matrix4x4f::translate(glimpsePos) *
        Matrix4x4f::lookAt(glimpsePos, camera->getPosition())
    );

    glimpseMesh->render();

    { Timer _(tm, "text");
        if (showFps) { text->render(); }
        if (showPos) { posText->render(); }
        if (showId) { idText->render(); }
    }

    { Timer _(tm, "swap");
        graphics->swap();
    }
}

void World::tick() {
    prevColor = color;

    funnySum -= funny[funnyIndex];
    funny[funnyIndex] = lightOn ? randd.nextFloat() : 0.f;
    funnySum += funny[funnyIndex];
    funnyIndex = (funnyIndex + 1) % FUNNY_SIZE;

    color = FIRE * funnySum / FUNNY_SIZE;
}

bool World::shouldEnd() const {
    return !graphics->isWindowOpen();
}

void World::togglePaused() {
    paused = !paused;
    inputManager->setMouseVisibility(paused);
    inputManager->setMousePosition(screenMiddle);
}
