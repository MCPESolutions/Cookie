#pragma once

class PacketHandlerDispatcherInstance {
   public:  
   CLASS_MEMBER(uintptr_t**, vtable, 0x0);
    virtual ~PacketHandlerDispatcherInstance();
    virtual void handle(void* networkIdentifier, void* netEventCallback,
                        std::shared_ptr<class Packet>) = 0;
   public:
    uintptr_t getPacketHandler() {
        return MemoryUtil::GetVTableFunction(this, 1);
    }
};