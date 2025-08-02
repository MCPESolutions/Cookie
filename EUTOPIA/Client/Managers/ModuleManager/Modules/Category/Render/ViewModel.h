#pragma once
#include "../../ModuleBase/Module.h"

class ViewModel : public Module {
   public:
    glm::mat4x4 oldMatrix;
    Vec3<float> mhTrans = Vec3<float>(0.f, 0.f, 0.f);
    Vec3<float> mhScale = Vec3<float>(1.f, 1.f, 1.f);
    Vec3<float> mhRot = Vec3<float>(0.f, 0.f, 0.f);

   public:
    bool Reset = false;

   public:
    ViewModel();
    virtual void onClientTick() override;
};
