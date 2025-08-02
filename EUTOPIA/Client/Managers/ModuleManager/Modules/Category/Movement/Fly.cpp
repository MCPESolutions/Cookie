#include "Fly.h"
#include <cmath>
#include <Windows.h>

Fly::Fly()
    : Module("Fly", "Fly like Superman, defy gravity and move through the air",
             Category::MOVEMENT) {
    registerSetting(new SliderSetting<float>(
        "Horizontal Speed", "Controls your horizontal flying speed", &hSpeed, 1.f, 0.2f, 15.f));
    registerSetting(new SliderSetting<float>(
        "Vertical Speed", "Controls your vertical flying speed", &vSpeed, 0.5f, 0.2f, 3.f));
    registerSetting(new SliderSetting<float>(
        "Glide Rate", "Adjusts how much you descend while gliding", &Glide, -0.02, -0.6, 0.f));
}

#include "../../../ModuleManager.h"

void Fly::onNormalTick(LocalPlayer* localPlayer) {
    auto state = localPlayer->getStateVectorComponent();
    if(state == nullptr) {
        return;
    }

    currentSpeed = hSpeed;

    Vec3<float>& velocity = state->mVelocity;
    velocity = Vec3<float>(0.f, 0.f, 0.f);
    velocity.y += Glide;

    if(GI::canUseMoveKeys()) {
        float yaw = localPlayer->getRotation().x;

        bool isForward = GI::isKeyDown('W');
        bool isLeft = GI::isKeyDown('A');
        bool isBackward = GI::isKeyDown('S');
        bool isRight = GI::isKeyDown('D');
        bool isUp = GI::isKeyDown(VK_SPACE);
        bool isDown = GI::isKeyDown(VK_SHIFT);

        Vec2<int> moveValue;
        if(isRight)
            moveValue.x += 1;
        if(isLeft)
            moveValue.x += -1;
        if(isForward)
            moveValue.y += 1;
        if(isBackward)
            moveValue.y += -1;
        if(isUp)
            velocity.y += vSpeed;
        if(isDown)
            velocity.y -= vSpeed;

        float angleRad = (float)std::atan2(moveValue.x, moveValue.y);
        float angleDeg = angleRad * (180.f / PI);
        yaw += angleDeg;
        Vec3<float> moveVec = Vec3<float>(0.f, velocity.y, 0.f);
        if((moveValue.x != 0) || (moveValue.y != 0)) {
            float calcYaw = (yaw + 90.f) * (PI / 180.f);
            moveVec.x = cos(calcYaw) * currentSpeed;
            moveVec.z = sin(calcYaw) * currentSpeed;
        }
        localPlayer->lerpMotion(moveVec);
    }
}

void Fly::onSendPacket(Packet* packet) {}