#pragma once

#include "../../ModuleBase/Module.h"
#include "../../../../../../SDK/GlobalInstance.h"
#include "../../../../../../Utils/TimerUtil.h"
#include "../../../../../../SDK/NetWork/Packets/InventoryTranscationPacket.h"
#include "../../../../../../SDK/World/Inventory/Transcation/InventoryAction.h"
#include "../../../../../../SDK/World/Inventory/Transcation/ComplexInventoryTranscation.h"
#include "../../../../../../SDK/World/Inventory/ContainerID.h"
#include "../../../../../../SDK/World/Item/ItemStack.h"
#include "../../../../../../SDK/World/Inventory/SimpleContainer.h"


class ChestStealer : public Module {
public:
    ChestStealer();

    // Module overrides
    void onEnable() override;
    void onDisable() override;
    void onNormalTick(LocalPlayer* player) override;
    void onReceivePacket(Packet* pkt, bool* cancel) override;
    void onChestScreen(ContainerScreenController* csc) override;

private:
    // Internal helpers
    void        takeItem(int chestSlot, ItemStack* item);
    uint64_t    getDelayMs() const;
    bool        shouldDelay();

    // Settings
    int  mDelayValue     = 0;   // ms
    bool mRandomizeDelay = false;
    int  mRandomMin      = 50;  // ms
    int  mRandomMax      = 120; // ms

    // State
    bool     mChestOpen     = false;
    uint64_t mLastItemTaken = 0;
    uint64_t mChestOpenedAt = 0;
}; 