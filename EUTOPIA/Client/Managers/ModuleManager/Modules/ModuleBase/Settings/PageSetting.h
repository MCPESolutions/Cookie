#pragma once
#include "Setting.h"

class PageSetting : public Setting {
public:
    PageSetting(const std::string& settingName, const std::string& des = "") {
        this->name = settingName;
        this->description = des;
        this->type = SettingType::PAGE_S;
    }
}; 