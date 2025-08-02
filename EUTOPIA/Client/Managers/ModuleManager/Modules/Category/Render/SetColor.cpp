#include "SetColor.h"

ModelOverlayColor::ModelOverlayColor()
    : Module("ModelOverlayColor", "Affects armor and items colors.", Category::RENDER) {
    registerSetting(new ColorSetting("Color", "NULL", &ArmorColor, ArmorColor));
	registerSetting(new SliderSetting<float>("Brightness", "NULL", &brightness, brightness,0.f,255.f));
}