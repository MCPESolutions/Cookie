#pragma once
#include "../../Client/Client.h"
#include "../FuncHook.h"
#include <MCR.h>
Vec2<float> hotbarPos = Vec2<float>(0, 0);


class DrawImageHook : public FuncHook {
   private:
    using func_t = __int64(__fastcall*)(MinecraftUIRenderContext*, mce::TexturePtr*, Vec2<float>&,
                                        Vec2<float>&, Vec2<float>&, Vec2<float>&);
    static inline func_t oFunc;

    static __int64 DrawImageCallback(MinecraftUIRenderContext* ctx, mce::TexturePtr* texture,
                                     Vec2<float>& pos, Vec2<float>& size, Vec2<float>& uvPos,
                                     Vec2<float>& uvSize) {
        return oFunc(ctx, texture, pos, size, uvPos, uvSize);
    }

   public:
    DrawImageHook() {
        OriginFunc = (void*)&oFunc;
        func = (void*)&DrawImageCallback;
    }
};
