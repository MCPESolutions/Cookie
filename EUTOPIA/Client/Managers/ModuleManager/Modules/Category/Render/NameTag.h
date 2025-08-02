#pragma once
#include "../../ModuleBase/Module.h"

class NameTags : public Module {
   private:
    float opacity = 0.4f;
    bool showPops = false;
    bool showItems = true;
    bool showDura = true;
    bool showSelf = true;
    bool underline = true;
    bool dynamicSize = false;
    float tagSize = 0.70f;
    UIColor friendColor = (0, 255, 255, 255);

   public:
    NameTags();
    virtual void onD2DRender() override;
    virtual void onMCRender(MinecraftUIRenderContext* renderCtx) override;
};