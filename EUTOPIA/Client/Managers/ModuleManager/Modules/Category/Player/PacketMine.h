#pragma once
// Full transplant from Lexical's PacketMine.h
#pragma once
#include "../../ModuleBase/Module.h"

class PacketMine : public Module {
   private:
    float mineSpeed = 1.f;
    float continueReset = 1.f;
    float mineRange = 6.f;
    int mineType = 0;
    int multiTask = 0;
    int packetMode = 0;
    int bind = 0x0;
    bool switchBack = false;
    bool doubleMine = false;
    bool showPercent = true;
    bool animateShrink = true;
    int fillAlpha = 40;
    int lineAlpha = 175;
    UIColor fillColor = UIColor(0, 255, 0);
    UIColor lineColor = UIColor(255, 255, 255);

   private:
    std::pair<BlockPos, uint8_t> breakPair;

   public:
    BlockPos getBreakPos() const {
        return breakPair.first;
    }
    uint8_t getFace() const {
        return breakPair.second;
    }
    int getMultiTask() const {
        return multiTask;
    }
    int getPacket() const {
        return packetMode;
    }
    bool getDoubleMine() const {
        return doubleMine;
    }

   public:
    void reset();
    bool canBreakBlock(const BlockPos& blockPos);
    bool breakBlock(const BlockPos& blockPos, uint8_t face = 0);
    void mineBlock(const BlockPos& blockPos, uint8_t face);
    std::pair<float, uint8_t> getMineTool(const BlockPos& blockPos);

   public:
    PacketMine();
    virtual void onNormalTick(LocalPlayer* localPlayer) override;
    virtual void onLevelRender() override;
    virtual void onD2DRender() override;


    void onSendPacket(Packet* packet, bool& cancel);
};