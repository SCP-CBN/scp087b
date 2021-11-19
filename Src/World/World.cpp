#include "World.h"

#include <iostream>

#include <PGE/Math/Random.h>
#include <PGE/Math/Interpolator.h>
#include <PGE/Graphics/Material.h>
#include <PGE/Types/Range.h>

#include "../Utilities/Directories.h"
#include "../Graphics/Rooms/RoomInstance.h"
#include "../Graphics/Text/TextRenderer.h"
#include "../Graphics/Camera.h"
#include "../Collision/Collider.h"
#include "../Player/PlayerController.h"
#include "../Entities/Glimpse.h"
#include "StatWorld.h"

using namespace PGE;

// Shit that needs a proper place.
constexpr float PLAYER_HEIGHT = 150.f;
constexpr Vector3f PLAYER_SPAWN(345.f, -45.f, -90.f);
constexpr Vector3f GLIMPSE_POS(250.f, -100.f, -50.f);

static CollisionMeshCollection coMeCo;
static std::unique_ptr<Input> escape = std::make_unique<KeyboardInput>(Keycode::ESCAPE);
static std::unique_ptr<Input> one = std::make_unique<KeyboardInput>(Keycode::NUM1);
static std::unique_ptr<Input> two = std::make_unique<KeyboardInput>(Keycode::NUM2);
static std::unique_ptr<Input> three = std::make_unique<KeyboardInput>(Keycode::NUM3);
static std::unique_ptr<Input> flash = std::make_unique<KeyboardInput>(Keycode::F);
static std::unique_ptr<Input> checky = std::make_unique<KeyboardInput>(Keycode::RCTRL);

static std::vector<std::unique_ptr<Input>> debug(12);

static Vector2f screenMiddle;

static Font* font;
static TextRenderer* text;
static TextRenderer* posText;
static TextRenderer* idText;

bool showFps = false;
bool showPos = false;
bool showId = false;

constexpr int ROOM_HEIGHT = 200;

static std::vector<RoomInstance*> instances;
static int currIndex = -999;
static void updateIndex(int newIndex) {
    for (int i : Range(-1, 2)) {
        int newCheckedIndex = newIndex + i;
        if (newCheckedIndex >= 0 && newCheckedIndex < instances.size() && abs(newCheckedIndex - currIndex) > 1) {
            instances[newIndex + i]->activate();
        }

        int checkedIndex = currIndex + i;
        if (checkedIndex >= 0 && checkedIndex < instances.size() && abs(checkedIndex - newIndex) > 1) {
            instances[checkedIndex]->deactive();
        }
    }
    currIndex = newIndex;
    idText->setText(String::from(currIndex));
    idText->setPosition(Vector2f(-50.f + idText->getWidth(), -50.f));
}

static void applyToActiveRooms(const std::function<void(RoomInstance&)>& func) {
    for (int i : Range(std::max(0, currIndex - 1), std::min((int)instances.size(), currIndex + 2))) {
        func(*instances[i]);
    }
}
//

World::World(TimeMaster& tm) : tm(tm),
    rooms({
        new RoomInfo("default", 100),
        new RoomInfo("room1", 50)
    }) {

    TimeMaster ctor;
    { Timer _(ctor, "all");
        constexpr int WIDTH = 999; constexpr int HEIGHT = 666;

        { Timer _(ctor, "gfx");
            screenMiddle = Vector2f(WIDTH, HEIGHT) / 2;
            graphics = Graphics::create("SCP-087-B", WIDTH, HEIGHT, WindowMode::Windowed, Renderer::DirectX11);
            graphics->setVsync(false);
        }

        camera = new Camera(WIDTH, HEIGHT, 90);
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
            inputManager->trackInput(escape.get());
            inputManager->trackInput(one.get());
            inputManager->trackInput(two.get());
            inputManager->trackInput(three.get());
            inputManager->trackInput(flash.get());
            inputManager->trackInput(checky.get());

            for (int i : Range(12)) {
                debug[i] = std::make_unique<KeyboardInput>((Keycode)((int)Keycode::F1 + i));
                inputManager->trackInput(debug[i].get());
            }
        }

        { Timer _(ctor, "map");
            constexpr int ROOM_COUNT = 100;
            instances.reserve(ROOM_COUNT);
            Vector3f basePos[2];
            basePos[1] = Vector3f(800.f, 0.f, -700.f);
            Random rand(String("juan hates cheese").getHashCode());
            Room::RenderInfo rInfo;
            for (int i : Range(ROOM_COUNT)) {
                const IRoomInfo* info = &rooms.getRandomRoom(rand);
                rInfo.rotation = i % 2 == 0 ? 0 : Math::degToRad(180);
                RoomInstance* newRoom = info->instantiate(coMeCo, rInfo, basePos[i % 2] - Vector3f(0.f, (float)(ROOM_HEIGHT * i), 0.f));
                for (int j : Range(4)) {
                    rInfo.offsets[j] += (i % 2 == 0 || j >= 2 ? 1.f : -1.f) * info->getRoom().getUvOffset((Room::MeshType)j);
                }
                instances.push_back(newRoom);
            }
            updateIndex(0);
        }

        glimpse = new Glimpse(*resources);
        glimpse->setPosition(GLIMPSE_POS);

        // CREATE PLAYER
        playerCon = new PlayerController(*inputManager, *camera, coMeCo, PLAYER_HEIGHT);
        playerCon->setPosition(PLAYER_SPAWN);
        togglePaused();
    }
    std::cout << ctor.print() << std::endl;
}

World::~World() {
    applyToActiveRooms([](RoomInstance& r) { r.deactive(); });
    for (RoomInstance* inst : instances) {
        delete inst;
    }
    rooms.unload();
    delete glimpse;
    delete playerCon;
    delete inputManager;
    delete text;
    delete posText;
    delete idText;
    delete font;
    delete resources;
    delete camera;
    delete graphics;
}

//
bool lightOn = false;
static Vector3f prevColor;
static Vector3f color;
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
    
    if (checky->isHit()) {
        for (const IRoomInfo* r : rooms) {
            r->getRoom().toggleDebug();
        }
    }

    if (!graphics->isWindowFocused() && !paused) {
        togglePaused();
    }

    if (!paused) {
        { Timer _(tm, "coll");
            playerCon->update(delta);
            resources->getRoomShader().getFragmentShaderConstant("lightPos")->setValue(camera->getPosition()); // Set torch to player pos
            if (int newIndex = (int)(-camera->getPosition().y / ROOM_HEIGHT); currIndex != newIndex) {
                if (currIndex >= 0 && currIndex < instances.size()) {
                    instances[currIndex]->leave();
                }
                updateIndex(newIndex);
                if (newIndex >= 0 && newIndex < instances.size()) {
                    instances[newIndex]->enter();
                }
            }
            posText->setText("X: " + String::from(camera->getPosition().x) + '\n'
                + "Y: " + String::from(camera->getPosition().y) + '\n'
                + "Z: " + String::from(camera->getPosition().z) + '\n'
            );

            applyToActiveRooms([=](RoomInstance& r) { r.update(delta); });
        }
    }
    
    text->setText(String::from(ticker.getFPS()) + "\n" + String::from(ticker.getInterpFPS()));

    return !shouldEnd();
}

void World::render(float interp) const {
    for (unsigned i : Range(12)) {
        if (debug[i]->isHit()) {
            resources->getRoomShader().getFragmentShaderConstant("debug")->setValue(i);
        }
    }

    resources->getRoomShader().getFragmentShaderConstant("intensity")->setValue(Interpolator::lerp(prevColor, color, interp));

    { Timer _(tm, "clear");
        graphics->clear(Colors::GRAY);
    }

    { Timer _(tm, "cam");
        camera->applyTransforms();
    }

    { Timer _(tm, "inst");
        applyToActiveRooms([=](RoomInstance& r) { r.render(interp); });
    }

    { Timer _(tm, "glimpse");
        glimpse->update(camera->getPosition());
        glimpse->render();
    }

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

    applyToActiveRooms([](RoomInstance& r) { r.tick(); });
}

bool World::shouldEnd() const {
    return !graphics->isWindowOpen();
}

void World::togglePaused() {
    paused = !paused;
    inputManager->setMouseRelativeInput(!paused);
    inputManager->setMousePosition(screenMiddle);
}
