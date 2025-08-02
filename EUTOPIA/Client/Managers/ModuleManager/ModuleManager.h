#pragma once
#include "Modules/ModuleBase/Module.h"
#include "Modules/Category/Client/ClickGUI.h"
#include "Modules/Category/Client/CustomFont.h"
#include "Modules/Category/Client/Hud.h"
#include "Modules/Category/Client/ArrayList.h"

//Player
#include "Modules/Category/Player/BlockReach.h"
#include "Modules/Category/Player/Offhand.h"
#include "Modules/Category/Player/ChestStealer.h"
#include "Modules/Category/Player/InvManager.h"
#include "Modules/Category/Player/PacketMine.h"

//World
#include "Modules/Category/World/AirPlace.h"

// Combat
#include "Modules/Category/Combat/KillAura.h"
#include "Modules/Category/Combat/Hitbox.h"
#include "Modules/Category/Combat/BowSpam.h"
#include "Modules/Category/Combat/Criticals.h"
#include "Modules/Category/Combat/Reach.h"

//MovenMent
#include "Modules/Category/Movement/Velocity.h"
#include "Modules/Category/Movement/NoSlow.h"
#include "Modules/Category/Movement/Fly.h"
#include "Modules/Category/Movement/Phase.h"

//Render
#include "Modules/Category/Render/NoRender.h"
#include "Modules/Category/Render/NoSwing.h"
#include "Modules/Category/Render/NameTag.h"
#include "Modules/Category/Render/Swing.h"
#include "Modules/Category/Render/CustomFov.h"
#include "Modules/Category/Render/GlintColor.h"
#include "Modules/Category/Render/CameraTweaks.h"
#include "Modules/Category/Render/FullBright.h"
#include "Modules/Category/Render/ChunkBorders.h"
#include "Modules/Category/Render/NoHurtCam.h"
#include "Modules/Category/Render/BlockHighLight.h"
#include "Modules/Category/Render/SetColor.h"
#include "Modules/Category/Render/ViewModel.h"

//MISC
#include "Modules/Category/Misc/NoPacket.h"
#include "Modules/Category/Misc/Timer.h"

class ModuleManager {
public:
	static inline std::vector<Module*> moduleList;

	static void init();
	static void shutdown();

	template <typename TRet>
	static TRet* getModule() {
		for (Module* mod : moduleList) {
			TRet* result = dynamic_cast<TRet*>(mod);
			if (result == nullptr)
				continue;
			return result;
		}
		return nullptr;
	}

	static void onKeyUpdate(int key, bool isDown);
	static void onClientTick();
	static void onNormalTick(LocalPlayer* localPlayer);
	static void onLevelTick(Level* level);
	static void onUpdateRotation(LocalPlayer* localPlayer);
	static void onSendPacket(Packet* packet);
    static void onReceivePacket(Packet* packet,bool*cancel);
	static void onD2DRender();
	static void onMCRender(MinecraftUIRenderContext* renderCtx);
	static void onLevelRender();
    static void onRenderActorBefore(Actor* actor, Vec3<float> *camera, Vec3<float> *pos);
    static void onRenderActorAfter(Actor* actor);
    static void onChestScreen(ContainerScreenController* csc);
	static void onLoadConfig(void* conf);
	static void onSaveConfig(void* conf);
};
