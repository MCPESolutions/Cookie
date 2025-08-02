#pragma once
#include "../../ModuleBase/Module.h"

class CustomFont : public Module {
public:
	EnumSetting* fontEnumSetting = nullptr;
	int fontMode = 1;
    int fontSize = 29;
	bool italic = false;
	bool shadow = false;
	std::string getSelectedFont();
public:

	CustomFont();

	bool isEnabled() override;
	bool isVisible() override;
};
