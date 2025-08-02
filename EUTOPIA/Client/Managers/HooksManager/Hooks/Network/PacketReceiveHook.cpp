#include "PacketReceiveHook.h"
#include "../../../../../SDK/NetWork/Packets/NetworkStackLatencyPacket.h"

std::unordered_map<PacketID, std::unique_ptr<FuncHook>> PacketReceiveHook::mDetours;

void* PacketReceiveHook::onPacketSend(void* _this, void* networkIdentifier, void* netEventCallback, std::shared_ptr<Packet> packet) {
    auto packetHook = static_cast<PacketHook<static_cast<PacketID>(0)>*>(mDetours[packet->getId()].get());
    auto ofunc = packetHook->getOriginal<&PacketReceiveHook::onPacketSend>();

    NetworkIdentifier = networkIdentifier;

    bool cancel = false;
    ModuleManager::onReceivePacket(packet.get(), &cancel);

    
    return ofunc(_this, networkIdentifier, netEventCallback, packet);
}

void PacketReceiveHook::handlePacket(std::shared_ptr<Packet> packet) {
    if (!NetworkIdentifier || !packet) return;
    
    auto ci = GI::getClientInstance();
    if (!ci || !ci->getminecraftSim() || !ci->getminecraftSim()->getGameSession()) {
        return;
    }
    
    onPacketSend(packet->getpacketHandler(), NetworkIdentifier, 
                ci->getminecraftSim()->getGameSession()->EventCallback(), packet);
}

void PacketReceiveHook::init() {
    static bool called = false;
    if (called) return;
    called = true;
    
    uint64_t start = TimerUtil::getCurrentMs();
   // logF("Initializing PacketReceiveHook...");
    
    std::vector<PacketID> packetsToHook = {
        PacketID::SetActorMotion,
    };
    
    int hookedCount = 0;
    
    for (auto id : packetsToHook) {
        try {
            auto packet = MinecraftPacket::createPacket(id);
            if (!packet) {
                //logF("Failed to create packet for ID: 0x%X", static_cast<int>(id));
                continue;
            }
            
            auto packetFunc = packet->packetHandler->getPacketHandler();
            if (!packetFunc) {
               // logF("Failed to get packet handler for packet ID: 0x%X", static_cast<int>(id));
                continue;
            }
            
            auto hook = std::make_unique<PacketHook<static_cast<PacketID>(0)>>();
            hook->name = ("PacketHook_" + std::to_string(static_cast<int>(id))).c_str();
            hook->address = (packetFunc);
            hook->onHookRequest();
            
            if (hook->enableHook()) {
                mDetours[id] = std::move(hook);
                hookedCount++;
                //logF("Successfully hooked packet ID: 0x%X", static_cast<int>(id));
            } else {
               // logF("Failed to enable hook for packet ID: 0x%X", static_cast<int>(id));
            }
            
        } catch (...) {
          //  logF("Exception occurred while hooking packet ID: 0x%X", static_cast<int>(id));
        }
    }
    
    uint64_t timeTaken = TimerUtil::getCurrentMs() - start;
   // logF("Successfully hooked %d packets in %lldms", hookedCount, timeTaken);
}

void PacketReceiveHook::shutdown() {
   // logF("Shutting down PacketReceiveHook...");
    mDetours.clear();
}