
#include <string>
#include "InvUtil.h"
#include "../../SDK/World/Item/ItemStack.h"
#include "../../SDK/World/Item/Item.h"
#include "../../SDK/GlobalInstance.h"
#include "../../SDK/NetWork/Packets/MobEquipmentPacket.h"
#include "../../SDK/World/Inventory/PlayerInventory.h"
#include "PlayerUtil.h"

PlayerInventory* InvUtil::getPlayerInventory() {
    return GI::getLocalPlayer()->getsupplies();
}

Container* InvUtil::getInventory() {
    return getPlayerInventory()->container;
}
void InvUtil::switchSlot(int32_t slot) {
    if(getPlayerInventory() == nullptr) {
        return;
    }
    try {
        getPlayerInventory()->mSelectedSlot = slot;
    } catch(const std::exception& e) {
    }
}
 bool InvUtil::isVaildItem(ItemStack* item) {  
    if(item == nullptr || item->getItem() == nullptr || item == ItemStack::getEmptyItem()) {
        return false;
    }
    return true;
}
int InvUtil::getItemId(ItemStack* stack) {
    if(!isVaildItem(stack))
        return 0;
    return stack->mItem->mItemId;
}

std::string InvUtil::getItemName(ItemStack* stack) {
    if(!isVaildItem(stack))
        return "none";
    return stack->mItem->mName;
}

int InvUtil::getHeldItemId() {
    return getItemId(getItem(getSelectedSlot()));
}

std::string InvUtil::getHeldItemName() {
    return getItemName(getItem(getSelectedSlot()));
}

uint8_t InvUtil::getSelectedSlot() {
    return GI::getLocalPlayer()->getsupplies()->mSelectedSlot;
}

ItemStack* InvUtil::getItem(int32_t slot) {
    return GI::getLocalPlayer()->getsupplies()->container->getItem(slot);
}

void InvUtil::sendMobEquipment(uint8_t slot) {
    // Ensure valid local player and supplies
    LocalPlayer* lp = GI::getLocalPlayer();
    if(lp == nullptr)
        return;

    // Create the packet
    auto pkt = MinecraftPacket::createPacket<MobEquipmentPacket>();
    if(!pkt)
        return;

    // Runtime ID of the local player (actor)
    pkt->mRuntimeId = lp->getRuntimeID();

    // Fill item descriptor for the given slot
    PlayerInventory* pInv = lp->getsupplies();
    if(pInv == nullptr || pInv->container == nullptr)
        return;
    ItemStack* stack = pInv->container->getItem(slot);
    pkt->mItem = NetworkItemStackDescriptor(*stack);

    // Hotbar slot information
    pkt->mSlot = slot;
    pkt->mSelectedSlot = slot;
    pkt->mSlotByte = slot;
    pkt->mSelectedSlotByte = slot;

    // Container ID 0 (Inventory) is sufficient for hotbar updates
    pkt->mContainerId = static_cast<uint8_t>(MobEquipmentPacket::ContainerID::Inventory);
    pkt->mContainerIdByte = pkt->mContainerId;

    // Send to server
    GI::getPacketSender()->sendToServer(pkt.get());

    // Update local cached server-side slot index
    PlayerUtil::selectedSlotServerSide = slot;
}
int32_t InvUtil::findItemSlotInHotbar(uint16_t itemId) {
    int32_t result = -1;
    for(int i = 0; i < 9; i++) {
        ItemStack* itemStack = GI::getLocalPlayer()->getsupplies()->container->getItem(i);
        if(isVaildItem(itemStack) && itemStack->mItem.get()->mItemId == itemId) {
            result = i;
            break;
        }
    }
    return result;
}

int32_t InvUtil::findItemSlotInInventory(uint16_t itemId) {
    int32_t result = -1;
    for(int i = 0; i < 36; i++) {
        ItemStack* itemStack = GI::getLocalPlayer()->getsupplies()->container->getItem(i);
        if(isVaildItem(itemStack) && itemStack->mItem.get()->mItemId == itemId) {
            result = i;
            break;
        }
    }
    return result;
}

void InvUtil::moveItem(uint8_t from, uint8_t to) {
    getPlayerInventory()->container->setItem(to, *getItem(from));
    getPlayerInventory()->container->removeItem(from, getItem(from)->mCount);
}

float InvUtil::caculateItemDurability(ItemStack* item) {
    if(!isVaildItem(item)) {
        return 0.0f;
    }
    float durability = item->getDamageValue();
    float maxDurability = item->mItem->getMaxDamage();
    return maxDurability > 0 ? durability / maxDurability : 0.0f;
}
