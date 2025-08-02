#pragma once
#include "../FuncHook.h"
#include <unordered_map>
#include <memory>

static inline void* NetworkIdentifier = nullptr;

class PacketReceiveHook {
private:
    static std::unordered_map<PacketID, std::unique_ptr<FuncHook>> mDetours;

public:
    static void* onPacketSend(void* _this, void* networkIdentifier, void* netEventCallback, std::shared_ptr<Packet> packet);
    static void handlePacket(std::shared_ptr<Packet> packet);
    static void init();
    static void shutdown();

    template<PacketID ID>
    class PacketHook : public FuncHook {
    private:
        using func_t = void*(__thiscall*)(void*, void*, void*, std::shared_ptr<Packet>);
        static inline func_t oFunc;
        
        static void* PacketCallback(void* _this, void* networkIdentifier, void* netEventCallback, std::shared_ptr<Packet> packet) {
           return PacketReceiveHook::onPacketSend(_this, networkIdentifier, netEventCallback, packet);
        }
        
    public:
        PacketHook() {
            OriginFunc = (void*)&oFunc;
            func = (void*)&PacketCallback;
        }
        
        template<auto FuncPtr>
        func_t getOriginal() { return oFunc; }
    };
};