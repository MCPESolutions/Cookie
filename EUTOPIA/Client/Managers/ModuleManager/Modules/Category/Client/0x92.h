#pragma once
#include "../../ModuleBase/Module.h"
#include "xorstr.h"

class Ox92 : public Module {
   public:
    Ox92();
    void onClientTick() override;
};