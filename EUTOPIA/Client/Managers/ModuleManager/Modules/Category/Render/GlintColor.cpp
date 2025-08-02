#include "GlintColor.h"

GlintColor::GlintColor() : Module("GlintColor", "Glint effect color", Category::RENDER) {
    registerSetting(new BoolSetting("Glint", "Enable glint effect", &isGlint, isGlint));
    registerSetting(
        new ColorSetting("Glint Color", "Color for glint effect", &glintColor, glintColor, true));
}
