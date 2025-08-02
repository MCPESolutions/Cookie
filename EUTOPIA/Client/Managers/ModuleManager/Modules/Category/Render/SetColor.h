#pragma once
#pragma once
#include "../../ModuleBase/Module.h"

class ModelOverlayColor : public Module {
   public:
	UIColor ArmorColor = (255, 0, 0, 175);
    float brightness = 1.0f;
    ModelOverlayColor();
};