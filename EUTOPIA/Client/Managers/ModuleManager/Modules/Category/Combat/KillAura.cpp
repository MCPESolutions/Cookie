#include "KillAura.h"

#include <Minecraft/TargetUtil.h>
#include <Minecraft/WorldUtil.h>

#include <algorithm>
#include <random>

#include "../../../ModuleManager.h"

KillAura::KillAura() : Module("KillAura", "Attacks nearby entities", Category::COMBAT) {
    registerSetting(new SliderSetting<float>("Reach", "", &range, 5.f, 1.f, 16.f));
    registerSetting(new SliderSetting<float>("WallRange", "", &wallRange, 0.f, 0.f, 16.f));
    registerSetting(new EnumSetting("Mode", "", {"Single", "Multi", "Switch"}, &mode, 0));
    registerSetting(new SliderSetting<int>("MinCPS", "", &minCPS, 5, 0, 20));
    registerSetting(new SliderSetting<int>("MaxCPS", "", &maxCPS, 10, 0, 20));
    registerSetting(new EnumSetting("Rotation", "", {"None", "Strafe", "Client"}, &rotMode, 0));
    registerSetting(new EnumSetting("AutoWeapon", "", {"None", "Regular"}, &autoWeapon, 0));
    registerSetting(new BoolSetting("Mobs", "", &includeMobs, false));
}

std::string KillAura::getModeText() {
    switch(mode) {
        case 0:
            return "Single";
        case 1:
            return "Multi";
        case 2:
            return "Switch";
        default:
            return "";
    }
}

void KillAura::onEnable() {
    targetList.clear();
    switchIndex = 0;
    lastAttack = std::chrono::steady_clock::now();
}

void KillAura::onDisable() {
    targetList.clear();
}

bool compareDist(Actor* a, Actor* b) {
    Vec3<float> pos = GI::getLocalPlayer()->getPos();
    return a->getPos().dist(pos) < b->getPos().dist(pos);
}

int KillAura::getBestWeaponSlot() {
    LocalPlayer* player = GI::getLocalPlayer();
    if(!player || autoWeapon == 0)
        return -1;

    int bestSlot = -1;
    float bestDmg = 0.f;

    for(int i = 0; i < 9; ++i) {
        ItemStack* stack = player->getsupplies()->container->getItem(i);
        if(!stack || !stack->mItem)
            continue;

        float dmg = stack->getDamageValue();
        if(dmg > bestDmg) {
            bestDmg = dmg;
            bestSlot = i;
        }
    }
    return bestSlot;
}

bool KillAura::canAttack() {
    if(minCPS == 0 && maxCPS == 0)
        return true;

    static std::default_random_engine rng(std::random_device{}());
    std::uniform_int_distribution<int> dist(minCPS, maxCPS);
    int cps = std::max(1, dist(rng));
    int delayMs = 1000 / cps;

    auto now = std::chrono::steady_clock::now();
    if(std::chrono::duration_cast<std::chrono::milliseconds>(now - lastAttack).count() >= delayMs) {
        lastAttack = now;
        return true;
    }
    return false;
}

Actor* KillAura::getNextTarget() {
    if(targetList.empty())
        return nullptr;
    if(mode == 0)
        return targetList[0];
    if(mode == 2) {
        if(switchIndex >= targetList.size())
            switchIndex = 0;
        return targetList[switchIndex++];
    }
    return nullptr;
}

void KillAura::Attack(Actor* target) {
    if(!target)
        return;

    LocalPlayer* player = GI::getLocalPlayer();
    if(!player || !player->getgamemode())
        return;

    int bestSlot = getBestWeaponSlot();
    int prevSlot = player->getsupplies()->getmSelectedSlot();

    if(autoWeapon == 1 && bestSlot != -1 && bestSlot != prevSlot) {
        player->getsupplies()->mSelectedSlot = bestSlot;
    }

    if(autoWeapon == 2 && bestSlot != -1 && bestSlot != prevSlot) {
    }

    player->getgamemode()->attack(target);
    player->swing();

    if(autoWeapon == 2 && bestSlot != -1 && bestSlot != prevSlot) {
    }

    if(autoWeapon == 1 && bestSlot != -1 && bestSlot != prevSlot) {
        player->getsupplies()->mSelectedSlot = prevSlot;
    }
}

void KillAura::onNormalTick(LocalPlayer* player) {
    Level* level = player->level;
    BlockSource* region = GI::getRegion();
    targetList.clear();

    for(auto& entity : level->getRuntimeActorList()) {
        if(!TargetUtil::isTargetValid(entity, includeMobs))
            continue;

        float checkRange =
            (region->getSeenPercent(player->getEyePos(), entity->getAABB(true)) == 0.f) ? wallRange
                                                                                        : range;

        if(player->getPos().dist(entity->getPos()) <= checkRange)
            targetList.push_back(entity);
    }

    if(targetList.empty()) {
        shouldRotate = false;
        return;
    }

    std::sort(targetList.begin(), targetList.end(), compareDist);

    if(rotMode == 1 || rotMode == 2) {
        Vec3<float> targetPos = targetList[0]->getEyePos();
        rotAngle = player->getEyePos().CalcAngle(targetPos);
        shouldRotate = true;
    } else {
        shouldRotate = false;
    }

    if(!canAttack())
        return;

    if(mode == 1) {
        for(auto& target : targetList)
            Attack(target);
    } else {
        Attack(getNextTarget());
    }
}

void KillAura::onUpdateRotation(LocalPlayer* player) {
    if(!shouldRotate || rotMode == 0)
        return;

    auto* rot = player->getActorRotationComponent();
    auto* head = player->getActorHeadRotationComponent();
    auto* body = player->getMobBodyRotationComponent();

    if(rot) {
        if(rotMode == 1) {
            rot->mYaw = rotAngle.y;
        }
        rot->mPitch = rotAngle.x;
        rot->mOldYaw = rotAngle.y;
        rot->mOldPitch = rotAngle.x;
    }

    if(head)
        head->mHeadRot = rotAngle.y;
    if(body)
        body->yBodyRot = rotAngle.y;
}

void KillAura::onSendPacket(Packet* packet) {
    if(!packet)
        return;
    if(!shouldRotate || rotMode == 0)
        return;

    if(packet->getId() == PacketID::PlayerAuthInput) {
        auto paip = static_cast<PlayerAuthInputPacket*>(packet);
        if(paip) {
            paip->mRot = rotAngle;
        }
    }
}