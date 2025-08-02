#include <Windows.h>
#include "Client/Client.h"
#include "Client/Managers/HooksManager/HookManager.h"
#include "Client/Managers/CommandManager/CommandManager.h"

//#include "Client/Client.h"
//#include "Client/Managers/CommandManager/CommandManager.h"
//#include "Client/Managers/HooksManager/HookManager.h"
//#include "Client/Managers/ModuleManager/ModuleManager.h"
//#include "Utils/D2D.h"

DWORD WINAPI initClient(LPVOID lpParameter) {
    Client::init();

    while(Client::isInitialized()) {
        ModuleManager::onClientTick();
        Sleep(50);
    }

    Sleep(25);

    FreeLibraryAndExitThread((HMODULE)lpParameter, 1);
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved) {
    switch(ul_reason_for_call) {
        case DLL_PROCESS_ATTACH:
            CreateThread(0, 0, (LPTHREAD_START_ROUTINE)initClient, hModule, 0, 0);
            break;
        case DLL_PROCESS_DETACH:
                Client::shutdown();
                Sleep(50);
                D2D::Clean();
                HookManager::shutdown();
                ModuleManager::shutdown();
                CommandManager::shutdown();
                Client::DisplayClientMessage("%sEjected", MCTF::RED);

            break;
    }
    return TRUE;
}
