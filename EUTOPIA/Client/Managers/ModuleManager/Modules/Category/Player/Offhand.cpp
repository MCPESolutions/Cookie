#include "Offhand.h"

#include "../../../../../../Utils/Logger.h"
#include <string>

Offhand::Offhand()
    : Module("Offhand", "Automatically puts totems/shields into your offhand", Category::PLAYER) {
    itemModeEnum = (EnumSetting*)registerSetting(
        new EnumSetting("Item", "Item to switch ", {"Totem", "Shield"}, &itemMode, 0));
}

std::string Offhand::getModeText() {
    return itemModeEnum->enumList[itemMode];
}

void Offhand::onNormalTick(LocalPlayer* localPlayer) {
    int desiredMode = itemMode;

    ItemStack* offhandStack = localPlayer->getOffhandSlot();
    if(offhandStack && offhandStack->mItem.get() != nullptr)
        return;

    uint16_t ItemID = (desiredMode == 0) ? 601 : 362;

    Container* inventory = localPlayer->getsupplies()->container;
    for(int i = 0; i < 36; i++) {
        ItemStack* itemStack = inventory->getItem(i);
        Item* item = itemStack->getItem();
        if(item != nullptr && item->mItemId == ItemID) {
            InventoryAction action = InventoryAction(i, itemStack, ItemStack::getEmptyItem());
            action.mSource.mType = InventorySourceType::ContainerInventory;
            action.mSource.mContainerId = static_cast<int>(ContainerID::Inventory);

            InventoryAction action2 = InventoryAction(0, ItemStack::getEmptyItem(), itemStack);
            action2.mSource.mType = InventorySourceType::ContainerInventory;
            action2.mSource.mContainerId = static_cast<int>(ContainerID::Offhand);

            auto pkt = MinecraftPacket::createPacket<InventoryTransactionPacket>();
            auto cit = std::make_unique<ComplexInventoryTransaction>();
            cit->data.addAction(action);
            cit->data.addAction(action2);
            pkt->mTransaction = std::move(cit);
            GI::getPacketSender()->sendToServer(pkt.get());
            break;
        }
    }
}