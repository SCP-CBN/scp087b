#ifndef B_MATCH_H_INCLUDED
#define B_MATCH_H_INCLUDED

#include <PGE/Math/Random.h>
#include "../Resources.h"
#include "../../Player/PlayerController.h"
#include "./Light.h"
#include "../Camera.h"

using namespace PGE;

constexpr int LIGHT_SIZE = 32;
constexpr Vector3f FIRE(0xF5 / 255.f, 0x58 / 255.f, 0x22 / 255.f);

class Match {
    public:
        Match(Resources& resources, PlayerController& player, Camera& cam, float range = 300.0f);
        void tick();
        void render(float interp);
        void update();

    private:
        class Light* lightObj;

        class Resources& resources;
        class PlayerController& playerCon;
        Camera& camera;

        Vector3f prevColor;
        Vector3f color;

        float lightVal[LIGHT_SIZE];
        int lightIndex = 0;
        float lightSum = 0.f;
        Random randomLight;
};

#endif // B_MATCH_H_INCLUDED
