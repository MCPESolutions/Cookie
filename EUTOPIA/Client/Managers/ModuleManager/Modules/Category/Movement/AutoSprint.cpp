#include "AutoSprint.h"

#include <Windows.h>

AutoSprint::AutoSprint()
    : Module("AutoSprint", "Automatically sprint while moving forward", Category::MOVEMENT) {}

void pressKey(WORD keyCode) {
    INPUT input = {0};
    input.type = INPUT_KEYBOARD;
    input.ki.wVk = keyCode;
    input.ki.dwFlags = 0;
    SendInput(1, &input, sizeof(INPUT));
}

void releaseKey(WORD keyCode) {
    INPUT input = {0};
    input.type = INPUT_KEYBOARD;
    input.ki.wVk = keyCode;
    input.ki.dwFlags = KEYEVENTF_KEYUP;
    SendInput(1, &input, sizeof(INPUT));
}

void AutoSprint::onNormalTick(LocalPlayer* localPlayer) {
    if(!localPlayer)
        return;

    bool isUsingItem = localPlayer->getItemUseDuration() > 0;
    bool isSneaking = localPlayer->getMoveInputComponent()->getmIsSneakDown();
    bool isSprint = localPlayer->getMoveInputComponent()->getmIsSprinting();

    if(GI::isKeyDown('W') && !isSneaking && !isUsingItem && !isSprint) {
        pressKey(VK_CONTROL);
    } else {
        releaseKey(VK_CONTROL);
    }
}
