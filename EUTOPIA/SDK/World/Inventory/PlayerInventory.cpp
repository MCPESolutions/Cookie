#include"PlayerInventory.h"
#include"../../GlobalInstance.h"
void PlayerInventory:: dropSlot(int slot)
{
    ItemStack* itemStack = container->getItem(slot);

    if (!itemStack->mItem) return;

 if(!itemStack->mItem)
        return;

    static ItemStack blankStack = ItemStack();

    InventoryAction action = InventoryAction(slot, itemStack, &blankStack);
    InventoryAction action2 = InventoryAction(0, &blankStack, itemStack);
    action.mSource.mContainerId = static_cast<int>(ContainerID::Inventory);
    action2.mSource.mContainerId = static_cast<int>(ContainerID::Inventory);
    action.mSource.mType = InventorySourceType::ContainerInventory;
    action2.mSource.mType = InventorySourceType::WorldInteraction;

    auto pkt = MinecraftPacket::createPacket<InventoryTransactionPacket>();

    auto cit = std::make_unique<ComplexInventoryTransaction>();
    cit->data.addAction(action);
    cit->data.addAction(action2);

    pkt->mTransaction = std::move(cit);
    GI::getClientInstance()->getpacketSender()->sendToServer(pkt.get());

}

void PlayerInventory::swapSlots(int from, int to)
{
    ItemStack* item1 = container->getItem(from);
    ItemStack* item2 = container->getItem(to);

    auto action1 = InventoryAction(from, item1, item2);
    auto action2 = InventoryAction(to, item2, item1);

    action1.mSource.mType = InventorySourceType::ContainerInventory;
    action2.mSource.mType = InventorySourceType::ContainerInventory;
    action1.mSource.mContainerId = static_cast<int>(ContainerID::Inventory);
    action2.mSource.mContainerId = static_cast<int>(ContainerID::Inventory);

    auto pkt = MinecraftPacket::createPacket<InventoryTransactionPacket>();

    auto cit = std::make_unique<ComplexInventoryTransaction>();

    cit->data.addAction(action1);
    cit->data.addAction(action2);

    pkt->mTransaction = std::move(cit);
    GI::getPacketSender()->sendToServer(pkt.get());
    //GI::getClientInstance()->getPacketSender()->sendToServer(pkt);
}
void PlayerInventory::equipArmor(int slot)
{
    auto player = GI::getLocalPlayer();

    ItemStack* itemStack = container->getItem(slot);

    if (!itemStack->mItem) return;

    static ItemStack blankStack = ItemStack();

    Item* item = itemStack->getItem();
    // Get the current item stack in the armor slot
    ItemStack* armorStack = player->getArmorContainer()->getItem(item->getArmorSlot());

    InventoryAction action = InventoryAction(slot, itemStack, armorStack);
    action.mSource.mType = InventorySourceType::ContainerInventory;
    action.mSource.mContainerId = static_cast<int>(ContainerID::Inventory);

    InventoryAction action2 = InventoryAction(item->getArmorSlot(), armorStack, itemStack);
    action2.mSource.mType = InventorySourceType::ContainerInventory;
    action2.mSource.mContainerId = static_cast<int>(ContainerID::Armor);


    auto pkt = MinecraftPacket::createPacket<InventoryTransactionPacket>();

    auto cit = std::make_unique<ComplexInventoryTransaction>();
    cit->data.addAction(action);
    cit->data.addAction(action2);

    pkt->mTransaction = std::move(cit);
    GI::getPacketSender()->sendToServer(pkt.get());
    /*setItem(slot, armorStack);
    player->getArmorContainer()->setItem(item->getArmorSlot(), itemStack);*/
}

void PlayerInventory::startUsingItem(int slot) {
    auto player = GI::getLocalPlayer();

    auto pkt = MinecraftPacket::createPacket<InventoryTransactionPacket>();
    auto cit = std::make_unique<ItemUseInventoryTransaction>();
    cit->mActionType = ItemUseInventoryTransaction::ActionType::Use;
    cit->mSlot = slot;
    cit->mItemInHand = *player->supplies->container->getItem(slot);


    pkt->mTransaction = std::move(cit);

   GI::getPacketSender()->sendToServer(pkt.get());
}

void PlayerInventory::releaseUsingItem(int slot)
{
    auto player = GI::getLocalPlayer();

    int oldSlot = player->supplies->mSelectedSlot;
    player->supplies->mSelectedSlot = slot;
    player->gamemode->releaseUsingItem();
    player->supplies->mSelectedSlot = oldSlot;
}