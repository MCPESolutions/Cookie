#include "ViewModel.h"

ViewModel::ViewModel() : Module("ViewModel", "Custom item view model", Category::RENDER) {
    registerSetting(new BoolSetting("Reset", "Reset all settings to default", &Reset, false));

    registerSetting(
        new SliderSetting<float>("Translate X", "NULL", &mhTrans.x, mhTrans.x, -2.f, 2.f));
    registerSetting(
        new SliderSetting<float>("Translate Y", "NULL", &mhTrans.y, mhTrans.y, -2.f, 2.f));
    registerSetting(
        new SliderSetting<float>("Translate Z", "NULL", &mhTrans.z, mhTrans.z, -2.f, 2.f));

    registerSetting(new SliderSetting<float>("Scale X", "NULL", &mhScale.x, mhScale.x, -2.f, 2.f));
    registerSetting(new SliderSetting<float>("Scale Y", "NULL", &mhScale.y, mhScale.y, -2.f, 2.f));
    registerSetting(new SliderSetting<float>("Scale Z", "NULL", &mhScale.z, mhScale.z, -2.f, 2.f));

    registerSetting(
        new SliderSetting<float>("Rotation X", "NULL", &mhRot.x, mhRot.x, -180.f, 180.f));
    registerSetting(
        new SliderSetting<float>("Rotation Y", "NULL", &mhRot.y, mhRot.y, -180.f, 180.f));
    registerSetting(
        new SliderSetting<float>("Rotation Z", "NULL", &mhRot.z, mhRot.z, -180.f, 180.f));
}

void ViewModel::onClientTick() {
    if(Reset) {
        mhTrans = Vec3<float>(0.f, 0.f, 0.f);
        mhScale = Vec3<float>(1.f, 1.f, 1.f);
        mhRot = Vec3<float>(0.f, 0.f, 0.f);
        Reset = false;
    }
}
