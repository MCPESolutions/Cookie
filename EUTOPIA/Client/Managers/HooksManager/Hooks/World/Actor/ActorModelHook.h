#pragma once
#include "../../../../../../SDK/World/Actor/Actor.h"
#include "../../FuncHook.h"
#include "Bone.h"
#include "ActorModel.h"
class ActorModelHook : public FuncHook {
   private:
    using func_t = void(__fastcall*)(uintptr_t a1, uintptr_t a2, uintptr_t a3, float a4, int a5);
    static inline func_t oFunc;

   public:
    static inline bool hasHeadModel = false;
    static inline ActorPartModel headModel = {};

    static void __fastcall ActorModelCallback(uintptr_t a1, uintptr_t a2, uintptr_t a3, float a4,
                                              int a5) {
        oFunc(a1, a2, a3, a4, a5);

        Actor* ent = *reinterpret_cast<Actor**>(a2 + 0x38);
        if(!ent)
            return;

        auto player = GI::getLocalPlayer();
        if(!player || ent != player)
            return;

        Bone* bone = reinterpret_cast<Bone*>(a3);
        if(!bone)
            return;

        auto partModel = bone->getActorPartModel();
        if(!partModel)
            return;

        if(bone->mBoneStr == "leftarm") {
            headModel = *partModel;
            hasHeadModel = true;
        }
    }

    ActorModelHook() {
        OriginFunc = (void*)&oFunc;
        func = (void*)&ActorModelCallback;
    }
};