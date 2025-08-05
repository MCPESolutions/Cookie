#pragma once
#include <chrono>
#include <vector>

#include "../../ModuleBase/Module.h"

class KillAura : public Module {
   private:
    float range = 5.f;
    float wallRange = 0.f;
    int mode = 0;
    bool throughWalls = false;
    bool includeMobs = false;
    int minCPS = 5;
    int maxCPS = 10;
    int rotMode = 0;
    int autoWeapon = 0;

    std::vector<Actor*> targetList;
    std::chrono::steady_clock::time_point lastAttack;
    size_t switchIndex = 0;
    bool shouldRotate = false;
    Vec2<float> rotAngle;

    void Attack(Actor* target);
    int getBestWeaponSlot();
    Actor* getNextTarget();
    bool canAttack();

   public:
    KillAura();
    virtual void onEnable() override;
    virtual void onDisable() override;
    virtual void onNormalTick(LocalPlayer* localPlayer) override;
    virtual void onUpdateRotation(LocalPlayer* localPlayer) override;
    virtual void onSendPacket(Packet* packet) override;
    std::string getModeText() override;
};
