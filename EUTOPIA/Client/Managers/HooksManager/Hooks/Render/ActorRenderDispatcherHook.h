#pragma once
#include "../../../../../SDK/GlobalInstance.h"
#include "../../../../../SDK/Render/ActorRenderDispatcher.h"
#include "../../../../../SDK/Render/BaseActorRenderContext.h"
#include "../../../../../SDK/World/Actor/Actor.h"
#include "../../../../../Utils/Maths.h"
#include "../FuncHook.h"


class ActorRenderDispatcherHook : public FuncHook {
   private:
    using func_t = void*(__thiscall*)(ActorRenderDispatcher*, BaseActorRenderContext*, Actor*,
                                     Vec3<float>*, Vec3<float>*, Vec2<float>*, bool);
    static inline func_t oFunc;

    static void* ActorRenderDispatcherCallback(ActorRenderDispatcher* _this,
                                              BaseActorRenderContext* entityRenderContext,
                                              Actor* entity, Vec3<float>* cameraTargetPos,
                                              Vec3<float>* pos, Vec2<float>* rot,
                                              bool ignoreLighting) {

        auto type = static_cast<uint64_t>( entity->getActorTypeComponent()->mType);
       return oFunc(_this, entityRenderContext, entity, cameraTargetPos, pos, rot,
                         ignoreLighting);
    }

   public:
    ActorRenderDispatcherHook() {
        OriginFunc = (void*)&oFunc;
        func = (void*)&ActorRenderDispatcherCallback;
    }
};