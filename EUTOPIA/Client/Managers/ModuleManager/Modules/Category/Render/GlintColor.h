#pragma once
#include "../../ModuleBase/Module.h"

class GlintColor : public Module {
   public:
    bool isGlint = false;
    UIColor glintColor = UIColor(255, 255, 255, 255);

   public:
    GlintColor();
};
