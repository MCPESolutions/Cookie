#pragma once
#include "../FuncHook.h"

class doBobHurt : public FuncHook {
   private:
    using func_t = void*(__thiscall*)(void*, glm::mat4*);
    static inline func_t oFunc;

    static void* doBobHurtCallback(void* _this, glm::mat4* matrix) {
        auto result = oFunc(_this, matrix);
        static ViewModel* viewMod = ModuleManager::getModule<ViewModel>();

        if(!viewMod || !viewMod->isEnabled())
            return result;

        float scaleX = viewMod->mhScale.x;
        float scaleY = viewMod->mhScale.y;
        float scaleZ = viewMod->mhScale.z;
        *matrix = glm::scale(*matrix, glm::vec3(scaleX, scaleY, scaleZ));

        float rotX = glm::radians(viewMod->mhRot.x);
        float rotY = glm::radians(viewMod->mhRot.y);
        float rotZ = glm::radians(viewMod->mhRot.z);

        if(rotX != 0.0f)
            *matrix = glm::rotate(*matrix, rotX, glm::vec3(1.0f, 0.0f, 0.0f));
        if(rotY != 0.0f)
            *matrix = glm::rotate(*matrix, rotY, glm::vec3(0.0f, 1.0f, 0.0f));
        if(rotZ != 0.0f)
            *matrix = glm::rotate(*matrix, rotZ, glm::vec3(0.0f, 0.0f, 1.0f));

        float x = viewMod->mhTrans.x;
        float y = viewMod->mhTrans.y;
        float z = viewMod->mhTrans.z;
        *matrix = glm::translate(*matrix, glm::vec3(x, y, z));

        return result;
    }

   public:
    doBobHurt() {
        OriginFunc = (void*)&oFunc;
        func = (void*)&doBobHurtCallback;
    }
};
