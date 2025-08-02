#include "Actor.h"
#include "../../GlobalInstance.h"
#include "../../../Client/Managers/HooksManager/Hooks/Network/PacketReceiveHook.h"
void sendToSelf(const std::shared_ptr<Packet>& packet) {
    PacketReceiveHook::handlePacket(packet);
}
  
void Actor::setPosition(Vec3<float> pos) {  
    auto pkt = MinecraftPacket::createPacket(PacketID::MovePlayer);  
    auto mpp = std::static_pointer_cast<MovePlayerPacket>(pkt);  
    mpp->mPlayerID = this->getRuntimeID();  
    auto rotComp = this->getActorRotationComponent();  
    mpp->mRot = {rotComp->mPitch, rotComp->mYaw};  
    mpp->mYHeadRot = this->getActorHeadRotationComponent()->mHeadRot;  
    mpp->mPos = pos;  
    mpp->mResetPosition = PositionMode::Teleport;  
    mpp->mRidingID = -1;  
    mpp->mCause = TeleportationCause::Unknown;  
    mpp->mSourceEntityType = ActorType::Player;  
    mpp->mTick = 0;  
    sendToSelf(mpp);  
}

void Actor::setGameType(int gameType) {
    auto gtpkt = MinecraftPacket::createPacket(PacketID::SetPlayerGameType);
    auto pkt = reinterpret_cast<SetPlayerGameTypePacket*>(gtpkt.get());
    pkt->mPlayerGameType = static_cast<GameType>(gameType);
    GI::getPacketSender()->sendToServer(pkt);
}
