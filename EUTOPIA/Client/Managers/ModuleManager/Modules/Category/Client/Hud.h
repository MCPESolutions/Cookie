#pragma once
#include "../../ModuleBase/Module.h"

class HUD : public Module {
   private:
    bool showPosition = true;
    bool showDirection = false;
    bool showArmor = true;
    bool inventoryHud = false;
    int offset = 15;
    int opacity = 125;
    int spacing = -1;

   public:
    HUD();
    ~HUD();

    void onD2DRender() override;
};
