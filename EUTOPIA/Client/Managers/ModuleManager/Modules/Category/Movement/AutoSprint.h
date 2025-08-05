#pragma once
#include "../../ModuleBase/Module.h"

class AutoSprint : public Module {
   public:
    AutoSprint();

    void onNormalTick(LocalPlayer* localPlayer) override;
};
