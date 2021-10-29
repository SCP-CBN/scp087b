#include "Match.h"

#include <PGE/Math/Random.h>

Match::Match(Resources& res, PlayerController& player, Camera& cam, float range) : resources(res), playerCon(player), camera(cam) {
    lightObj = new Light(
        resources.getRoomShader().getFragmentShaderConstant("lightPos"),
        resources.getRoomShader().getFragmentShaderConstant("viewPos"),
        resources.getRoomShader().getFragmentShaderConstant("intensity"),
        resources.getRoomShader().getFragmentShaderConstant("effectiveRangeSquared")
    );
    lightObj->setRange(range * range);
}

void Match::tick() {
    prevColor = color;

    lightSum -= lightVal[lightIndex];
    lightVal[lightIndex] = playerCon.isLightOn() ? randomLight.nextFloat() : 0.f;
    lightSum += lightVal[lightIndex];
    lightIndex = (lightIndex + 1) % LIGHT_SIZE;

    color = FIRE * lightSum / LIGHT_SIZE;
}

void Match::render(float interp) {
    lightObj->setIntensity(Interpolator::lerp(prevColor, color, interp));
}

void Match::update() {
    lightObj->setPos(camera.getPosition());
    lightObj->setView(camera.getRotation());
}
