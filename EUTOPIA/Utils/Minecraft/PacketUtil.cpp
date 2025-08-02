#include "PacketUtil.h"
#include "../../SDK/GlobalInstance.h"
void PacketUtil::spoofSlot(int slot, bool bypassHook) {
    auto player = GI::getLocalPlayer();
    if(!player)
        return;

    auto itemStack = player->supplies->container->getItem(slot);
    if(itemStack == nullptr)
        return;
    if(itemStack->mItem.get() == nullptr)
        return;

    auto* sender = GI::getPacketSender();
    if(!sender)
        return;

    auto meppkt = MinecraftPacket::createPacket(PacketID::MobEquipment);
    if(!meppkt)
        return;

    auto mep = reinterpret_cast<MobEquipmentPacket*>(meppkt.get());

    mep->mRuntimeId = (player->getRuntimeIDComponent()->mRuntimeID);
    mep->mItem = (NetworkItemStackDescriptor(*itemStack));
    mep->mSlot = (slot);
    mep->mSelectedSlot = (slot);
    mep->mContainerId = (static_cast<uint8_t>(ContainerID::Inventory));
    mep->mContainerIdByte = (static_cast<unsigned char>(ContainerID::Inventory));
    mep->mSelectedSlotByte = (static_cast<unsigned char>(slot));
    mep->mSlotByte = (static_cast<unsigned char>(slot));
    if(bypassHook)
        sender->sendToServer(mep);
    else
        sender->send(mep);
}