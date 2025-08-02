#include "CustomFont.h"

CustomFont::CustomFont() : Module("Fonts", "Font of Client", Category::CLIENT) {
    fontEnumSetting = (EnumSetting*)registerSetting(new EnumSetting("Font", "NULL", {"Arial", "Bahnschrift", "Verdana"}, &fontMode, 1));
	registerSetting(new SliderSetting<int>("FontSize", "NULL", &fontSize, 29, 15, 40));
	registerSetting(new BoolSetting("Italic", "NULL", &italic, false));
	registerSetting(new BoolSetting("Shadow", "NULL", &shadow, true));
}

bool CustomFont::isEnabled() {
	return true;
}

bool CustomFont::isVisible() {
	return false;
}

std::string CustomFont::getSelectedFont() {
	return fontEnumSetting->enumList[fontMode];
}