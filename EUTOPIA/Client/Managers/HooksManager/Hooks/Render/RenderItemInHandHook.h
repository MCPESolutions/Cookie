#pragma once
#pragma once
#include "../FuncHook.h"
#include "../../../../../SDK/Render/BaseActorRenderContext.h"
#include "../../../../../SDK/Render/RenderItemInHandDescription.h"

class RenderItemInHandHook : public FuncHook {
   private:
    using func_t = void*(__thiscall*)(RenderItemInHandDescription*, void*, void*,void*,void*,void*,bool,void*,unsigned __int16, void*);
    static inline func_t oFunc;

    static void* ItemInHandRenderer_renderCallback(RenderItemInHandDescription* _this,
                                                  void* renderObject, void* itemFlags,
                                                  void* material, void* glintTexture,
                                                  void* worldMatrix, bool isDrawingUI,
                                                  void* globalConstantBuffers,
                                                  unsigned __int16 viewId, void* renderMetadata) {
        static GlintColor* viewMod = ModuleManager::getModule<GlintColor>();
        auto result = oFunc(_this, renderObject, itemFlags, material, glintTexture, worldMatrix,
                            isDrawingUI, globalConstantBuffers, viewId, renderMetadata);
        if(!viewMod || !viewMod->isEnabled())
            return result;

        float r = viewMod->glintColor.r / 255.0f;
        float g = viewMod->glintColor.g / 255.0f;
        float b = viewMod->glintColor.b / 255.0f;
        float a = viewMod->glintColor.a / 255.0f;
        _this->mGlintColor = Vec3<float>(r, g, b);
        _this->mGlintAlpha = a;
        return result;
    }

   public:
    RenderItemInHandHook() {
        OriginFunc = (void*)&oFunc;
        func = (void*)&ItemInHandRenderer_renderCallback;
    }
};