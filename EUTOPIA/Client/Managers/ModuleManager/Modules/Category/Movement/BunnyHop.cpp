#include "BunnyHop.h"

#include <Windows.h>

#include <cmath>

BunnyHop::BunnyHop()
    : Module("BunnyHop", "Continuously jump and strafe for faster movement", Category::MOVEMENT) {
    registerSetting(new SliderSetting<float>("Hop Height", "", &hopHeight, 0.42f, 0.2f, 1.0f));
    registerSetting(new SliderSetting<float>("Hop Speed", "", &hopSpeed, 1.2f, 0.5f, 3.0f));
    registerSetting(
        new SliderSetting<float>("Strafe Multiplier", "", &strafeMultiplier, 1.0f, 0.5f, 2.0f));
    registerSetting(new BoolSetting("Auto Jump", "", &autoJump, true));
    registerSetting(new BoolSetting("Strafe", "", &autoStrafe, true));
}

void BunnyHop::onNormalTick(LocalPlayer* localPlayer) {
    auto state = localPlayer->getStateVectorComponent();
    if(!state)
        return;

    Vec3<float>& velocity = state->mVelocity;
    bool onGround = localPlayer->onGround();

    if(autoJump && onGround) {
        velocity.y = hopHeight;
        wasOnGround = true;
    }

    if(autoStrafe && !onGround) {
        float yaw = localPlayer->getRotation().x;
        bool forward = GI::isKeyDown('W');
        bool back = GI::isKeyDown('S');
        bool left = GI::isKeyDown('A');
        bool right = GI::isKeyDown('D');

        Vec2<int> move;
        if(right)
            move.x += 1;
        if(left)
            move.x -= 1;
        if(forward)
            move.y += 1;
        if(back)
            move.y -= 1;

        if(move.x != 0 || move.y != 0) {
            float angle = std::atan2(move.x, move.y);
            float deg = yaw + (angle * 180.f / PI);
            float rad = (deg + 90.f) * PI / 180.f;
            velocity.x = std::cos(rad) * hopSpeed * strafeMultiplier;
            velocity.z = std::sin(rad) * hopSpeed * strafeMultiplier;
        }
    }
}

void BunnyHop::onSendPacket(Packet* packet) {}
