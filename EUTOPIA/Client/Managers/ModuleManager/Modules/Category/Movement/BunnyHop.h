#pragma once
#include "../../ModuleBase/Module.h"

class BunnyHop : public Module {
   private:
    float hopHeight = 0.42f;
    float hopSpeed = 1.2f;
    float strafeMultiplier = 1.0f;
    bool autoJump = true;
    bool autoStrafe = true;

    bool wasOnGround = false;
    float moveYaw = 0.f;

   public:
    BunnyHop();

    void onNormalTick(LocalPlayer* localPlayer) override;
    void onSendPacket(Packet* packet) override;
};
