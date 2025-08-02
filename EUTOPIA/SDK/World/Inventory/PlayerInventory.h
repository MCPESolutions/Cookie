#pragma once

#include "Inventory.h"
#include "Transcation/InventoryAction.h"
#include "ContainerID.h"
#include "../../NetWork/MinecraftPacket.h"
#include "../../NetWork/Packets/InventoryTranscationPacket.h"
#include "../Item/Item.h"
#include "Transcation/ItemUseInventoryTransaction.h"

class PlayerInventory {
public:
CLASS_MEMBER(Container*, container, 0xB8);
CLASS_MEMBER(int, mSelectedSlot, 0x10);
CLASS_MEMBER(int, mInHandSlot, 0x1C);




void dropSlot(int slot);

void swapSlots(int from, int to);

void equipArmor(int slot);

void startUsingItem(int slot);

void releaseUsingItem(int slot);

};
