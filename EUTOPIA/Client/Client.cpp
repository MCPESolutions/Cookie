#include "Client.h"
#include "Managers/HooksManager/HookManager.h"
#include "../SDK/MCTextFormat.h"
#include "../SDK/GlobalInstance.h"
#include "Managers/ConfigManager/ConfigManager.h"
#include "Managers/ModuleManager/ModuleManager.h"
#include "Managers/CommandManager/CommandManager.h"
#include "Managers/NotificationManager/NotificationManager.h"


void Client::DisplayClientMessage(const char* fmt, ...) {
	LocalPlayer* localPlayer = GI::getLocalPlayer();
	auto GuiData = GI::getGuiData();
	if (localPlayer == nullptr)
		return;

	va_list arg;
	va_start(arg, fmt);
	char message[300];
	vsprintf_s(message, 300, fmt, arg);
	va_end(arg);

	static std::string headerMessage;
	if (headerMessage.empty()) {
		char headerCStr[50];
		sprintf_s(headerCStr, 50, "%s[%sCookie%s] %s", MCTF::GRAY, MCTF::BLUE, MCTF::GRAY, MCTF::WHITE);
		headerMessage = std::string(headerCStr);
	}

	std::string messageStr = headerMessage + std::string(message);
	GuiData->displayMessage(messageStr);
}


void AddFont(const std::string& fontPath) {
	std::wstring temp = std::wstring(fontPath.begin(), fontPath.end());
	AddFontResource(temp.c_str());
}

void Client::init() {

	std::string ClientPath = FileUtil::getClientPath();
	if (!FileUtil::doesFilePathExist(ClientPath)) {
		FileUtil::createPath(ClientPath);
	}

	Logger::init();
	ConfigManager::init();
	
	MCTF::initMCTF();

	ModuleManager::init();
	CommandManager::init();
	HookManager::init();

	if (GI::getClientInstance() == nullptr)
		return;
	/*
	logF("%llx",GI::clientInstance);
	logF("%llx",GI::clientInstance->mcGame);
	logF("%llx",GI::getLocalPlayer());
	*/

	initialized = true;

	NotificationManager::addNotification("hallo", 5.f);

	if (!ConfigManager::doesConfigExist("default"))
		ConfigManager::createNewConfig("default");

	 ConfigManager::loadConfig("default");
}

void Client::shutdown() {
	ConfigManager::saveConfig();
	initialized = false;
}