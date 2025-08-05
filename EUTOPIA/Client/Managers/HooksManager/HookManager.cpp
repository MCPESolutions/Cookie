#include "HookManager.h"
#include <kiero.h>
#include "Hooks/Game/ClientInstance_UpdateHook.h"
#include "Hooks/DirectX/PresentHook.h"
#include "Hooks/DirectX/ResizeBuffersHook.h"
#include "Hooks/Input/KeyMouseHook.h"
#include "Hooks/Input/KeyMapHook.h"
#include "Hooks/Render/SetUpAndRenderHook.h"
#include "Hooks/Render/GetCameraComponentHook.h"
#include "Hooks/Render/GetFovHook.h"
#include "Hooks/Render/GetGammaHook.h"
#include "Hooks/Render/GetViewPerspectiveHook.h"
#include "Hooks/Render/HurtCamHook.h"
#include "Hooks/Render/RenderLevelHook.h"
#include "Hooks/Render/SetColorHook.h"
#include "Hooks/Network/SendPacketHook.h"
#include "Hooks/World/Actor/ActorNormalTickHook.h"
#include "Hooks/World/Actor/ActorIsOnFireHook.h"
#include "Hooks/World/Actor/ActorSwingHook.h"
#include "Hooks/World/Actor/MobSwingDuration.h"
#include "Hooks/World/Actor/PlayerSlowDown.h"
#include "Hooks/World/GameMode/StartDestroyBlockHook.h"
#include "Hooks/World/GameMode/StopDestroyBlockHook.h"
#include "Hooks/World/GameMode/GetPickRangeHook.h"
#include "Hooks/World/Level/LevelTickHook.h"
#include "Hooks/Game/SendChatMessageHook.h"
#include "Hooks/Game/ContainerScreenControllerHook.h"
#include "Hooks/Render/ActorRenderDispatcherHook.h"
#include "Hooks/World/Actor/ActorModelHook.h"
#include "../../../SDK/NetWork/MinecraftPacket.h"
#include "Hooks/Network/PacketReceiveHook.h"
#include "Hooks/World/Actor/ActorSetRotHook.h"
#include "Hooks/Render/DrawTextHook.h"
#include "Hooks/World/Block/GetBlockHook.h"
#include "Hooks/Render/DrawImageHook.h"
#include "Hooks/Render/DrawNineSliceHook.h"
#include "Hooks/Render/RenderItemInHandHook.h"
#include "Hooks/Render/RenderItemHook.h"
#include "Hooks/Network/GetAvgPingHook.h"
#include "Hooks/Render/RenderItemInOffhand.h"
#include "Hooks/Render/RenderItemObject.h"
#include "Hooks/Render/BobHurt.h"
#include "Hooks/Render/ActorShaderParams.h"
#include "Hooks/World/Block/TessellateBlockInWorldHook.h"
#include "Hooks/Render/RenderOutLineSelectionHook.h"

class ConnectionRequestCreateHook : public FuncHook {
   private:
    using func_t = void(__fastcall*)(__int64*, __int64, __int64, __int64, __int64, __int64, __int64,
                                     __int64, __int64, __int64, std::string*, int, int, int,
                                     __int64, char, char, __int64, int, std::string*, std::string*,
                                     bool, __int64, __int64, __int64, char);

    static inline func_t oFunc;

    static std::string GenerateRandomDeviceID() {
        static const char charset[] =
            "0123456789"
            "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
            "abcdefghijklmnopqrstuvwxyz";

        const size_t maxSize = 102 * 102;
        std::string result;
        result.reserve(maxSize);

        for(size_t i = 0; i < maxSize; ++i) {
            result += charset[rand() % (sizeof(charset) - 1)];
        }

        return result;
    }

    static void callback(__int64* a1, __int64 a2, __int64 a3, __int64 a4, __int64 a5, __int64 a6,
                         __int64 a7, __int64 a8, __int64 a9, __int64 a10, std::string* deviceId,
                         int a12, int a13, int a14, __int64 a15, char a16, char a17, __int64 a18,
                         int a19, std::string* platformUserId, std::string* thirdPartyName,
                         bool thirdPartyNameOnly, __int64 a23, __int64 a24, __int64 a25, char a26) {
        if(deviceId) {
            *deviceId = GenerateRandomDeviceID();
        }

        oFunc(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, deviceId, a12, a13, a14, a15, a16, a17, a18,
              a19, platformUserId, thirdPartyName, thirdPartyNameOnly, a23, a24, a25, a26);
    }

   public:
    ConnectionRequestCreateHook() {
        OriginFunc = (void**)&oFunc;
        func = (void*)&callback;
    }
};

void HookManager::init() {
	MH_Initialize();

	RequestHook<ClientInstanceUpdateHook>( MemoryUtil::findSignature("48 89 5C 24 ? 48 89 74 24 ? 48 89 7C 24 ? 55 41 56 41 57 48 8D AC 24 ? ? ? ? 48 81 EC ? ? ? ? 48 8B 05 ? ? ? ? 48 33 C4 48 89 85 ? ? ? ? 44 0F B6 FA"));
	RequestHook<SetUpAndRenderHook>(MemoryUtil::findSignature("48 8B C4 48 89 58 ? 55 56 57 41 54 41 55 41 56 41 57 48 8D A8 ? ? ? ? 48 81 EC ? ? ? ? 0F 29 70 ? 0F 29 78 ? 48 8B 05 ? ? ? ? 48 33 C4 48 89 85 ? ? ? ? 4C 8B E2 48 89 55 ? 4C 8B F9"));
    {  // render
        uintptr_t** MCRVtable = (uintptr_t**)(uintptr_t)MemoryUtil::getVtableFromSig(
            "48 8D 05 ? ? ? ? ? ? ? 48 89 51 ? 4C 89 41 ? ? ? ? 48 8D 54 24");
        RequestHook<DrawImageHook>(MCRVtable, 7);
        RequestHook<DrawTextHook>(MCRVtable, 5);
        RequestHook<DrawNiceSliceHook>(MCRVtable, 8);
    }
    RequestHook<KeyMapHook>(MemoryUtil::findSignature("48 ? ? 48 ? ? ? 4C 8D 05 ? ? ? ? 89"));
    RequestHook<KeyMouseHook>(
        MemoryUtil::findSignature("48 8B C4 48 89 58 ? 48 89 68 ? 48 89 70 ? 57 41 54 41 55 41 56 41 57 48 83 EC ? 44 0F B7 BC 24 ? ? ? ? 48 8B D9"));
    RequestHook<GetFovHook>(
		MemoryUtil::findSignature("48 8B C4 48 89 58 ? 48 89 70 ? 57 48 81 EC ? ? ? ? 0F 29 70 ? 0F 29 78 ? 44 0F 29 40 ? 44 0F 29 48 ? 48 8B 05 ? ? ? ? 48 33 C4 48 89 44 24 ? 41 0F B6 F0"));
    RequestHook<GetViewPerspectiveHook>(
        MemoryUtil::findSignature("48 83 EC ?? 48 8B 01 48 8D 54 ?? ?? 41 B8 03 00 00 00"));
    RequestHook<HurtCamHook>(MemoryUtil::findSignature("40 53 56 57 48 83 EC ? 48 8B F9 80 FA"));

    RequestHook<SendChatMessageHook>(
        MemoryUtil::getFuncFromCall(MemoryUtil::findSignature("E8 ? ? ? ? 3C ? 75 ? 48 8B 8F")));
    {
        RequestHook<GetAveragePingHook>(MemoryUtil::findSignature(
            "48 8B C4 48 81 EC ? ? ? ? 0F 10 4A ? 4C 8B 1A 4C 3B 1D ? ? ? ? 0F 29 70 ? 0F 29 78 ? "
            "0F 10 7A ? 0F 11 4C 24 ? 74 ? 44 8B 49"));
    }
    // ContainerScreenController::tick Hook
    RequestHook<ContainerScreenControllerHook>(
        MemoryUtil::getFuncFromCall(MemoryUtil::findSignature("E8 ? ? ? ? 48 8B ? ? ? ? ? 48 8D ? ? ? ? ? 41 B8 ? ? ? ? 8B F8")));
 
  //b  RequestHook<ConnectionRequestCreateHook>(
    //   MemoryUtil::findre("40 55 53 56 57 41 54 41 55 41 56 41 57 48 8D AC 24 ? ? ? ? 48 81 EC ? ? ? ? 48 8B 05 ? ? ? ? 48 33 C4 48 89 85 ? ? ? ? 49 8B D9 41 8B F0"));

	 {
		//renderOutLineSelection
         RequestHook<RenderOutLineSelectionHook>(
             MemoryUtil::getFuncFromCall(
             MemoryUtil::findSignature("E8 ? ? ? ? 90 48 8D 8D ? ? ? ? E8 ? ? ? ? 48 8B 86 ? ? ? ? 48 85 C0 0F 84 ? ? ? ? 80 38 ? 0F 84 ? ? ? ? 48 8B 86 ? ? ? ? 48 85 C0 74 ? F0 FF 40 ? 48 8B 86")));
	 }
    
     RequestHook<RenderLevelHook>(MemoryUtil::findSignature(
         "48 8B C4 48 89 58 ? 55 56 57 41 54 41 55 41 56 41 57 48 8D A8 ? ? ? ? 48 81 EC ? ? ? ? "
         "0F 29 70 ? 0F 29 78 ? 44 0F 29 40 ? 44 0F 29 48 ? 48 8B 05 ? ? ? ? 48 33 C4 48 89 85 ? ? "
         "? ? 4D 8B E8 4C 8B E2 4C 8B F9"));
    
	 {
         RequestHook<doBobHurt>("48 89 5C 24 ? 57 48 81 EC ? ? ? ? 0F 29 B4 24 ? ? ? ? 48 81 C1");

         RequestHook<RenderItemInHandHook>(
             MemoryUtil::findSignature("48 89 ? ? ? 48 89 ? ? ? 55 56 57 41 ? 41 ? 41 ? 41 ? 48 83 "
                                       "EC ? 4D 8B ? 4D 8B ? 4C 8B ? 48 8B ? 45 33"));
         RequestHook<RenderItemHook>(
             MemoryUtil::getFuncFromCall(MemoryUtil::findSignature("E8 ? ? ? ? 41 0F 11 B6 ? ? ? ? C6 47")));
	 }
     RequestHook<SetColorHook>(MemoryUtil::getFuncFromCall(
         MemoryUtil::findSignature("E8 ? ? ? ? 0F 28 05 ? ? ? ? 0F 11 85 ? ? ? ? 4C 8D 8D")));
	 {
         RequestHook<ActorShaderParamsHook>("48 89 5C 24 ? 55 56 57 41 54 41 55 41 56 41 57 48 8D AC 24 ? ? ? ? 48 81 EC ? ? ? ? 0F 29 B4 24 ? ? ? ? 48 8B 05 ? ? ? ? 48 33 C4 48 89 85 ? ? ? ? 48 8B 85");
		 }


	{
        // Options Hook
        uintptr_t** OptionsVtable = (uintptr_t**)(uintptr_t)MemoryUtil::getVtableFromSig(
            "48 8D 05 ? ? ? ? ? ? ? 48 8B 89 ? ? ? ? BF");
        RequestHook<GetGammaHook>(OptionsVtable, 146);
    }
	{
        {
            // Block Hook
            uintptr_t** BlockSourceVTable = (uintptr_t**)(uintptr_t)MemoryUtil::getVtableFromSig(
                "48 8D 05 ? ? ? ? ? ? ? FF 15 ? ? ? ? 41 89 46");
            RequestHook<GetBlockHook>(BlockSourceVTable, 2);
        }
	}

	{
	
		uintptr_t** PacketSenderVTable = (uintptr_t**)(uintptr_t)MemoryUtil::getVtableFromSig(
		    "48 8D 05 ?? ?? ?? ?? 49 89 07 4D 89 6F ?? 41 C6 47");
		RequestHook<SendPacketHook>(PacketSenderVTable, 2);
        PacketReceiveHook::init();
	}

	{
		// Entity Hook
        uintptr_t** PlayerVTable = (uintptr_t**)(uintptr_t)MemoryUtil::getVtableFromSig(
            "48 8D 05 ? ? ? ? ? ? ? 89 9E ? ? ? ? 48 8D 05");
		RequestHook<ActorNormalTickHook>(PlayerVTable, 24);
		RequestHook<ActorIsOnFireHook>(PlayerVTable, 48);
		RequestHook<ActorSwingHook>(PlayerVTable, 111);
		//RequestHook<ActorShouldRenderHook>(PlayerVTable, 66);
		RequestHook<ActorSetRotHook>("48 89 5C 24 ? 48 89 74 24 ? 48 89 7C 24 ? 55 41 56 41 57 48 8D 6C 24 ? 48 81 EC ? ? ? ? 48 8B 05 ? ? ? ? 48 33 C4 48 89 45 ? 41 8B 58");
		RequestHook<MobSwingDurationHook>("48 89 5C 24 ? 48 89 6C 24 ? 56 57 41 56 48 83 EC ? 8B 41 ? 48 8D 54 24 ? 48 8B 35");
		RequestHook<PlayerSlowDownHook>("4d 8b c8 4c 8b c2 48 8b d1 33 c9 e9 ? ? ? ? 48 89 5c 24");
        RequestHook<ActorModelHook>(
            "40 53 55 56 57 41 54 41 56 41 57 48 81 EC ? ? ? ? 48 63 AC 24");
	}

	{
		// GameMode Hook
        uintptr_t** GameModeVTable = (uintptr_t**)(uintptr_t)MemoryUtil::getVtableFromSig(
            "48 8D 05 ? ? ? ? ? ? ? 48 89 51 ? 48 C7 41");
		RequestHook<StartDestroyBlockHook>(GameModeVTable, 1);
		RequestHook<StopDestroyBlockHook>(GameModeVTable, 4);
		RequestHook<GetPickRangeHook>(GameModeVTable, 10);
	}
	{
		// Level Hook
		// Search for "Client%d camera ticking systems" -> xref first function -> What access to a1
        uintptr_t** LevelVTable = (uintptr_t**)(uintptr_t)MemoryUtil::getVtableFromSig(
            "48 8D 05 ? ? ? ? ? ? ? 48 8D 05 ? ? ? ? 48 89 47 ? 48 8D 05 ? ? ? ? 48 89 47 ? 48 8B 0D");
		RequestHook<LevelTickHook>(LevelVTable, 109);
	}

	
	   RequestHook<ActorRenderDispatcherHook>(MemoryUtil::findSignature(
        "48 89 5C 24 ? 55 56 57 41 54 41 55 41 56 41 57 48 8D 6C 24 ? 48 81 EC ? ? ? ? 0F 29 B4 24 "
        "? ? ? ? 48 8B 05 ? ? ? ? 48 33 C4 48 89 45 F7 4C 89 4C 24 ?"));


	if (kiero::init(kiero::RenderType::D3D12) == kiero::Status::Success) {
		uintptr_t** methodsTable = (uintptr_t**)kiero::getMethodsTable();
		RequestHook<PresentHook>(methodsTable, 140);
		RequestHook<ResizeBuffersHook>(methodsTable, 145);
	}
	else if (kiero::init(kiero::RenderType::D3D11) == kiero::Status::Success) {
		uintptr_t** methodsTable = (uintptr_t**)kiero::getMethodsTable();
		RequestHook<PresentHook>(methodsTable, 8);
		RequestHook<ResizeBuffersHook>(methodsTable, 13);
	}
	else {
		return;
	}

	
	if (!getHook<ClientInstanceUpdateHook>()->enableHook()) {
		return;
	}

	while(GI::getClientInstance() == nullptr) {
		Sleep(250);
	}

	for (auto& funcHook : hooksCache) {
		funcHook->enableHook();
	}

	// We don't this anymore so we can delete it
	for (auto& funcHook : hooksCache) {
		delete funcHook;
	}
	hooksCache.clear();
}

void HookManager::shutdown() {
	MH_DisableHook(MH_ALL_HOOKS);
	MH_RemoveHook(MH_ALL_HOOKS);
	MH_Uninitialize();
    PacketReceiveHook::shutdown();
	kiero::shutdown();

	for (auto& funcHook : hooksCache) {
		delete funcHook;
	}
	hooksCache.clear();
}