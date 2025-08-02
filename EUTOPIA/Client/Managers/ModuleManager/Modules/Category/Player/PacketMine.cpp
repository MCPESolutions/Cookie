// Full transplant from Lexical's PacketMine.cpp
#include "PacketMine.h"
#include <string>
#include <Minecraft/WorldUtil.h>
#include <Minecraft/InvUtil.h>
#include <Minecraft/PlayerUtil.h>
#include "../../../../../../SDK/NetWork/Packets/PlayActionPacket.h"
#include "../../../../../../Utils/D2D.h"
#include <MCR.h>

PacketMine::PacketMine() : Module("PacketMine", "mine blocks with packet", Category::PLAYER) {
    registerSetting(new EnumSetting(
        "Packet",
        "Type of packet\nAuthInput: Uses player auth input packet to mine, good for "
        "ignpvp\nPlayerAction: Uses player action packet to mine, good for geyser",
        {"AuthInput"}, &packetMode, 0));
    registerSetting(new SliderSetting<float>("Speed", "Mining speed", &mineSpeed, 1.f, 0.1f, 2.f));
    registerSetting(
        new SliderSetting<float>("Mine Range", "Mining range", &mineRange, 6.f, 1.f, 12.f));
    registerSetting(new SliderSetting<float>(
        "Continue Reset", "The break progress at the 2nd and more mine if using continue mode",
        &continueReset, 1.f, 0.f, 1.f));
    registerSetting(new EnumSetting("Mine Type",
                                    "Mining Type\nNormal: Mines normally\nContinue: Mines the same "
                                    "block over and over again\nInstant: Instamines all blocks",
                                    {"Normal", "Continue", "Instant"}, &mineType, 0));
    registerSetting(new KeybindSetting("Mine Bind", "Set this to none to automine", &bind, bind));
    registerSetting(new EnumSetting(
        "MultiTask",
        "Multitasking mode\nNormal: When eating the client will continue mining but will only "
        "break when the player is no longer eating/doing something\nStrict: Breaking will reset "
        "when eating/bowing/using an item\nStrict2: Eating will reset when placing, breaking, "
        "attacking, using an item\nNone: Mines even if doing something",
        {"Normal", "Strict", "Strict2", "None"}, &multiTask, 0));
    registerSetting(new BoolSetting(
        "Switch Back",
        "Switches back to old slot after mining, this may nullify the mine on some servers",
        &switchBack, false));
    registerSetting(new BoolSetting(
        "Show Percent", "Display mining progress percent in block center", &showPercent, true));
    registerSetting(new BoolSetting("Animate Shrink",
                                    "Render shrinking block animation based on progress",
                                    &animateShrink, true));
    registerSetting(new ColorSetting("Fill Color", "Color for the filled shrinking box", &fillColor,
                                     UIColor(0, 255, 0), false));
    registerSetting(new ColorSetting("Line Color", "Color for the outline of shrinking box",
                                     &lineColor, UIColor(255, 255, 255), false));
    registerSetting(new SliderSetting<int>("Fill Alpha", "Alpha value for the filled shrinking box", &fillAlpha, 40, 0, 255));
    registerSetting(new SliderSetting<int>("Line Alpha", "Alpha value for the outline of shrinking box", &lineAlpha, 175, 0, 255));
}

bool PacketMine::canBreakBlock(const BlockPos& blockPos) {
    if(blockPos == BlockPos(0, 0, 0))
        return false;
    if(blockPos.CastTo<float>().dist(GI::getLocalPlayer()->getPos()) > mineRange)
        return false;
    if(WorldUtil::getBlock(blockPos)->getblockLegcy()->canBeBuiltOver(
           GI::getRegion(), blockPos))
        return false;
    if(WorldUtil::getBlock(blockPos)->getblockLegcy()->blockid == 7)
        return false;
    return true;
}

void PacketMine::reset() {
    LocalPlayer* lp = GI::getLocalPlayer();
    Block* block = WorldUtil::getBlock(BlockPos(0, 0, 0));
    breakPair.first = BlockPos(0, 0, 0);
    breakPair.second = -1;
    lp->gamemode->mBreakProgress = 0;
}

bool PacketMine::breakBlock(const BlockPos& blockPos, uint8_t face) {
    LocalPlayer* localPlayer = GI::getLocalPlayer();
    GameMode* gm = localPlayer->gamemode;
    if(bind != 0x0 && !GI::isKeyDown(bind))
        return false;

    uint8_t bestSlot = getMineTool(blockPos).second;

    if(packetMode == 0) {
        gm->destroyBlock(blockPos, face);
    }

    if(mineType != 1) {
        breakPair = std::make_pair(BlockPos(0, 0, 0), -1);
        GI::getLocalPlayer()->gamemode->lastBreakPos = BlockPos(0, 0, 0);
    } else {
        GI::getLocalPlayer()->gamemode->mBreakProgress = continueReset;
    }
    return true;
}

void PacketMine::mineBlock(const BlockPos& blockPos, uint8_t face) {
    if(!canBreakBlock(blockPos))
        return;
    LocalPlayer* lp = GI::getLocalPlayer();
    if(mineType == 2)
        lp->gamemode->mBreakProgress = 1.f;
    else
        lp->gamemode->mBreakProgress = 0.f;
    breakPair = std::make_pair(blockPos, face);
}

std::pair<float, uint8_t> PacketMine::getMineTool(const BlockPos& blockPos) {
    LocalPlayer* localPlayer = GI::getLocalPlayer();
    GameMode* gm = localPlayer->gamemode;
    Block* block = WorldUtil::getBlock(blockPos);
    uint8_t oldSlot = InvUtil::getSelectedSlot();
    uint8_t bestSlot = 0;
    if(block != nullptr) {  // this shouldnt be nullptr but....
        float destroyRate = gm->getDestroyRate(block);
        for(uint8_t i = 0; i < 9; i++) {
            InvUtil::switchSlot(i);
            const float value = gm->getDestroyRate(block);
            if(value > destroyRate) {
                destroyRate = value;
                bestSlot = i;
            }
        }
        InvUtil::switchSlot(oldSlot);
        return std::make_pair(destroyRate, bestSlot);
    }
    return std::make_pair(0.f, 0);
}

void PacketMine::onNormalTick(LocalPlayer* localPlayer) {
    static bool shouldSwitch = false;
    static uint8_t lastSlot = -1;
    GameMode* gm = localPlayer->gamemode;
    if(localPlayer->getItemUseDuration() > 0 && multiTask == 1) {
        gm->mBreakProgress = 0.f;
        breakPair = std::make_pair(BlockPos(0, 0, 0), -1);
        return;
    }
    if(localPlayer->getItemUseDuration() > 0 && multiTask == 0) {
        if(PlayerUtil::selectedSlotServerSide != InvUtil::getSelectedSlot())
            InvUtil::sendMobEquipment(InvUtil::getSelectedSlot());
    }
    if(PlayerUtil::selectedSlotServerSide != InvUtil::getSelectedSlot() && packetMode == 1)
        InvUtil::sendMobEquipment(InvUtil::getSelectedSlot());
    if(!canBreakBlock(breakPair.first))
        return;
    if(switchBack && shouldSwitch && lastSlot != -1) {
        InvUtil::sendMobEquipment(InvUtil::getSelectedSlot());
        lastSlot = -1;
    }
    std::pair<float, uint8_t> bestSlot = getMineTool(breakPair.first);
    if(gm->mBreakProgress < 1.f) {
        gm->mBreakProgress += bestSlot.first * mineSpeed;
        if(mineType == 2)
            gm->mBreakProgress = 1.f;
        if(gm->mBreakProgress > 1.f)
            gm->mBreakProgress = 1.f;
    } else {
        if(localPlayer->getItemUseDuration() > 0 && multiTask == 0)
            return;
        uint8_t oldSlot = InvUtil::getSelectedSlot();
        InvUtil::switchSlot(bestSlot.second);
            breakBlock(breakPair.first, breakPair.second);
        InvUtil::switchSlot(oldSlot);
        InvUtil::sendMobEquipment(oldSlot);
        if(switchBack) {
            shouldSwitch = true;
            lastSlot = oldSlot;
        }
    }
}

void PacketMine::onSendPacket(Packet* packet, bool& cancel) {
    if(packetMode != 1)
        return;
    LocalPlayer* localPlayer = GI::getLocalPlayer();
    if(!localPlayer)
        return;
    if(packet->getId() == PacketID::PlayerAction) {
        PlayerActionPacket* actionPkt = (PlayerActionPacket*)packet;
        if(actionPkt->mAction == PlayerActionType::StartDestroyBlock) {
            mineBlock(actionPkt->mPos.toInt(), actionPkt->mFace);
        }
    }
}

void PacketMine::onLevelRender() {
    if(!animateShrink)
        return;

    LocalPlayer* lp = GI::getLocalPlayer();
    if(lp == nullptr)
        return;
    GameMode* gm = lp->gamemode;
    if(gm == nullptr)
        return;
    if(breakPair.first == BlockPos(0, 0, 0))
        return;

    float progress = gm->mBreakProgress;
    if(progress <= 0.f || progress >= 1.f)
        return;

    BlockSource* region = GI::getRegion();
    if(region == nullptr)
        return;

    Block* block = WorldUtil::getBlock(breakPair.first);
    if(block == nullptr)
        return;

    AABB aabb;
    block->getblockLegcy()->getOutline(block, region, &breakPair.first, &aabb);

    // Shrink based on progress
    Vec3<float> center = aabb.getCenter();
    float scale = 1.f - progress;

    Vec3<float> newLower = center + (aabb.lower - center) * scale;
    Vec3<float> newUpper = center + (aabb.upper - center) * scale;
    AABB shrunkAABB(newLower, newUpper);

    // Prepare colors with alpha
    UIColor fill = fillColor;
    fill.a = fillAlpha;
    UIColor line = lineColor;
    line.a = lineAlpha;

    // Render the shrunk box
    MCR::drawBox3dFilled(shrunkAABB, fill, line, 1.0f);
}

void PacketMine::onD2DRender() {
    if (!showPercent)
        return;

    LocalPlayer* lp = GI::getLocalPlayer();
    if (lp == nullptr)
        return;
    GameMode* gm = lp->gamemode;
    if (gm == nullptr)
        return;
    if (breakPair.first == BlockPos(0, 0, 0))
        return;

    float progress = gm->mBreakProgress;
    if (progress <= 0.f || progress >= 1.f)
        return;

    int percent = static_cast<int>(progress * 100.f);
    std::string text = std::to_string(percent) + "%";

    // Get block center
    Vec3<float> center = breakPair.first.CastTo<float>().add(0.5f);
    
    Vec2<float> screenSize = D2D::getWindowSize();
    Vec2<float> screenPos = D2D::WorldToScreen(center, screenSize);
    
    // Check if the screen position is valid
    if (screenPos.x < 0 || screenPos.y < 0)
        return;
        
    float textSize = 1.f;
    float textWidth = D2D::getTextWidth(text, textSize);
    Vec2<float> textPos(screenPos.x - textWidth / 2.f, screenPos.y);
    D2D::drawText(textPos, text, UIColor(255, 255, 255), textSize);
}