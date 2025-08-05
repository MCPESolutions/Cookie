#include "ModuleManager.h"
#include "../../Client.h"

void ModuleManager::init() {
	//Client
	moduleList.push_back(new Arraylist());
	moduleList.push_back(new ClickGUI());
    moduleList.push_back(new Ox92());
    moduleList.push_back(new HUD());
	moduleList.push_back(new CustomFont());

	//World
	moduleList.push_back(new AirPlace());

	//Combat
    moduleList.push_back(new KillAura());
    moduleList.push_back(new Hitbox());
    moduleList.push_back(new BowSpam());
    moduleList.push_back(new Criticals());
    moduleList.push_back(new Reach());

	//Movement
    moduleList.push_back(new Velocity());
    moduleList.push_back(new NoSlow());
	moduleList.push_back(new AutoSprint());
    moduleList.push_back(new BunnyHop());
    moduleList.push_back(new Fly());
    moduleList.push_back(new Phase());

	//Render
	moduleList.push_back(new NoRender());
    moduleList.push_back(new NameTags());
	moduleList.push_back(new Swing());
	moduleList.push_back(new NoSwing());
	moduleList.push_back(new CustomFov());
	moduleList.push_back(new CameraTweaks());
	moduleList.push_back(new Fullbright());
    moduleList.push_back(new ChunkBorders());
    moduleList.push_back(new GlintColor());
	moduleList.push_back(new NoHurtCam());
	moduleList.push_back(new BlockHighlight());
    moduleList.push_back(new ModelOverlayColor());
    moduleList.push_back(new ViewModel());

	//Player
	moduleList.push_back(new BlockReach());
    moduleList.push_back(new Offhand());
    moduleList.push_back(new PacketMine());
    moduleList.push_back(new ChestStealer()); 
    moduleList.push_back(new InvManager()); 

	//Misc
	moduleList.push_back(new NoPacket());
    moduleList.push_back(new Timer());

	std::sort(moduleList.begin(), moduleList.end(), [](Module* lhs, Module* rhs) {
		return lhs->getModuleName() < rhs->getModuleName();
		});
	getModule<ClickGUI>()->InitClickGUI();
	getModule<Arraylist>()->setEnabled(true);



}

void ModuleManager::shutdown() {
	for (auto& mod : moduleList) {
		mod->setEnabled(false);
	}
	for (auto& mod : moduleList) {
		delete mod;
		mod = nullptr;
	}
	moduleList.clear();
}

void ModuleManager::onKeyUpdate(int key, bool isDown) {
	if (!Client::isInitialized())
		return;

	for (auto& mod : moduleList) {
		mod->onKeyUpdate(key, isDown);
	}
}

void ModuleManager::onClientTick() {
	if (!Client::isInitialized())
		return;

	for (auto& mod : moduleList) {
		if (mod->isEnabled() || mod->runOnBackground()) {
			mod->onClientTick();
		}
	}
}

void ModuleManager::onNormalTick(LocalPlayer* localPlayer) {
	if (!Client::isInitialized())
		return;

	for (auto& mod : moduleList) {
		if (mod->isEnabled() || mod->runOnBackground()) {
			mod->onNormalTick(localPlayer);
		}
	}
}

void ModuleManager::onLevelTick(Level* level) {
	if (!Client::isInitialized())
		return;

	for (auto& mod : moduleList) {
		if (mod->isEnabled() || mod->runOnBackground()) {
			mod->onLevelTick(level);
		}
	}
}

void ModuleManager::onUpdateRotation(LocalPlayer* localPlayer) {
	if (!Client::isInitialized())
		return;

	for (auto& mod : moduleList) {
		if (mod->isEnabled() || mod->runOnBackground()) {
			mod->onUpdateRotation(localPlayer);
		}
	}
}

void ModuleManager::onSendPacket(Packet* packet) {
	if (!Client::isInitialized())
		return;

	for (auto& mod : moduleList) {
		if (mod->isEnabled() || mod->runOnBackground()) {
			mod->onSendPacket(packet);
		}
	}
}

void ModuleManager::onReceivePacket(Packet* packet,bool* cancel) {
    if(!Client::isInitialized())
        return;

    for(auto& mod : moduleList) {
        if(mod->isEnabled() || mod->runOnBackground()) {
            mod->onReceivePacket(packet, cancel);
        }
    }
}



void ModuleManager::onRenderActorBefore(Actor* actor,Vec3<float>* camera,Vec3<float>* pos) {
    if(!Client::isInitialized())
        return;

    for(auto& mod : moduleList) {
        if(mod->isEnabled() || mod->runOnBackground()) {
            mod->onRenderActorBefore(actor,camera,pos);
        }
    }
}
void ModuleManager::onRenderActorAfter(Actor* actor) {
    if(!Client::isInitialized())
        return;

    for(auto& mod : moduleList) {
        if(mod->isEnabled() || mod->runOnBackground()) {
            mod->onRenderActorAfter(actor);
        }
    }
}

void ModuleManager::onChestScreen(ContainerScreenController* csc) {
    if(!Client::isInitialized())
        return;

    for(auto& mod : moduleList) {
        if(mod->isEnabled() || mod->runOnBackground()) {
            mod->onChestScreen(csc);
        }
    }
}

void ModuleManager::onD2DRender() {
    if(!Client::isInitialized())
        return;

    for(auto& mod : moduleList) {
        if(mod->isEnabled() || mod->runOnBackground()) {
            mod->onD2DRender();
        }
    }
}

void ModuleManager::onMCRender(MinecraftUIRenderContext* renderCtx) {
	if (!Client::isInitialized())
		return;

	for (auto& mod : moduleList) {
		if (mod->isEnabled() || mod->runOnBackground()) {
			mod->onMCRender(renderCtx);
		}
	}
}

void ModuleManager::onLevelRender() {
	if (!Client::isInitialized())
		return;

	for (auto& mod : moduleList) {
		if (mod->isEnabled() || mod->runOnBackground()) {
			mod->onLevelRender();
		}
	}
}

void ModuleManager::onLoadConfig(void* conf) {
	if (!Client::isInitialized())
		return;

	for (auto& mod : moduleList) {
		mod->onLoadConfig(conf);
	}
}

void ModuleManager::onSaveConfig(void* conf) {
	if (!Client::isInitialized())
		return;

	for (auto& mod : moduleList) {
		mod->onSaveConfig(conf);
	}
}