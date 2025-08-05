#include "ClickGUI.h"

#include "../../../../../../Libs/json.hpp"
#include "../../../../../../Utils/TimerUtil.h"
#include "../../../ModuleManager.h"

ClickGUI::ClickGUI() : Module("ClickGUI", "Display all modules", Category::CLIENT, VK_INSERT) {
    registerSetting(new ColorSetting("Color", "NULL", &mainColor, mainColor));
    registerSetting(new ColorSetting("Header Color", "NULL", &headerColor, headerColor));

    registerSetting(new SliderSetting<float>("Blur", "Background blur intensity", &blurStrength,
                                             4.f, 0.f, 20.f));
    registerSetting(new BoolSetting("Description", "Show Description", &showDescription, true));

    registerSetting(new SliderSetting<float>("ShadowStrength", "", &windowShadowStrength, 10.f, 0.0f, 100.0f));
    registerSetting(new SliderSetting<float>("Rounding", "", &rounding, 1.0f, 0.0f, 20.0f));
    registerSetting(new SliderSetting<float>("Rounding2", "", &rounding2, 1.0f, 0.0f, 20.0f));
}

ClickGUI::~ClickGUI() {
    for(auto& window : windowList) {
        delete window;
    }
    windowList.clear();
}

ClickGUI::ClickWindow::ClickWindow(std::string windowName, Vec2<float> startPos, Category c) {
    this->name = windowName;
    this->category = c;
    this->pos = startPos;
    this->extended = true;

    for(auto& mod : ModuleManager::moduleList) {
        if(mod->getCategory() == c) {
            this->moduleList.push_back(mod);
        }
    }

    std::sort(this->moduleList.begin(), this->moduleList.end(),
              [](Module* lhs, Module* rhs) { return lhs->getModuleName() < rhs->getModuleName(); });
}

void ClickGUI::onDisable() {
    GI::getClientInstance()->grabVMouse();

    isLeftClickDown = false;
    isRightClickDown = false;
    isHoldingLeftClick = false;
    isHoldingRightClick = false;

    draggingWindowPtr = nullptr;

    capturingKbSettingPtr = nullptr;
    draggingSliderSettingPtr = nullptr;

    openAnim = 0.0f;
}

void ClickGUI::onEnable() {
    GI::getClientInstance()->releaseVMouse();
    openAnim = 0.0f;
}

bool ClickGUI::isVisible() {
    return false;
}

void ClickGUI::onKeyUpdate(int key, bool isDown) {
    if(!isEnabled()) {
        if(key == getKeybind() && isDown) {
            setEnabled(true);
        }
    } else {
        if(isDown) {
            if(key < 192) {
                if(capturingKbSettingPtr != nullptr) {
                    if(key != VK_ESCAPE)
                        *capturingKbSettingPtr->value = key;
                    capturingKbSettingPtr = nullptr;
                    return;
                }
            }
            if(key == getKeybind() || key == VK_ESCAPE) {
                setEnabled(false);
            }
        }
    }
}

void ClickGUI::onMouseUpdate(Vec2<float> mousePosA, char mouseButton, char isDown) {
    // MouseButtons
    // 0 = mouse move
    // 1 = left click
    // 2 = right click
    // 3 = middle click
    // 4 = scroll   (isDown: 120 (SCROLL UP) and -120 (SCROLL DOWN))

    switch(mouseButton) {
        case 0:
            mousePos = mousePosA;
            break;
        case 1:
            isLeftClickDown = isDown;
            isHoldingLeftClick = isDown;
            break;
        case 2:
            isRightClickDown = isDown;
            isHoldingRightClick = isDown;
            break;
        case 4:
            float moveVec = (isDown < 0) ? -15.f : 15.f;
            for(auto& window : windowList) {
                if(window == draggingWindowPtr)
                    continue;

                window->pos.y += moveVec;
            }
            break;
    }

    if(draggingWindowPtr != nullptr) {
        if(!isHoldingLeftClick)
            draggingWindowPtr = nullptr;
    }

    if(capturingKbSettingPtr != nullptr) {
        if(isRightClickDown) {
            *capturingKbSettingPtr->value = 0;
            capturingKbSettingPtr = nullptr;
            isRightClickDown = false;
        }
    }

    if(draggingSliderSettingPtr != nullptr) {
        if(!isHoldingLeftClick)
            draggingSliderSettingPtr = nullptr;
    }
}

void ClickGUI::InitClickGUI() {
    setEnabled(false);

    Vec2<float> startPos = Vec2<float>(25.f, 35.f);
    windowList.push_back(new ClickWindow("Combat", startPos, Category::COMBAT));
    startPos.x += 225;
    windowList.push_back(new ClickWindow("Movement", startPos, Category::MOVEMENT));
    startPos.x += 225;
    windowList.push_back(new ClickWindow("Render", startPos, Category::RENDER));
    startPos.x += 225;
    windowList.push_back(new ClickWindow("Player", startPos, Category::PLAYER));
    startPos.x += 225;
    windowList.push_back(new ClickWindow("World", startPos, Category::WORLD));
    startPos.x += 225;
    windowList.push_back(new ClickWindow("Misc", startPos, Category::MISC));
    startPos.x += 225;
    windowList.push_back(new ClickWindow("Client", startPos, Category::CLIENT));

    initialized = true;
}

void ClickGUI::Render() {
    if(!initialized)
        return;

    if(GI::canUseMoveKeys())
        GI::getClientInstance()->releaseVMouse();

    static Vec2<float> oldMousePos = mousePos;
    mouseDelta = mousePos.sub(oldMousePos);
    oldMousePos = mousePos;

    Vec2<float> screenSize = GI::getClientInstance()->guiData->windowSizeReal;
    float deltaTime = D2D::deltaTime;

    /*text*/
    const float textSize = 0.9f;
    const float textPaddingX = 6.0f;
    const float textPaddingY = 2.5f;
    const float textHeight = D2D::getTextHeight("", textSize) + 4.0f;

    const float borderWidth = 1.5f;
    const UIColor bgColor(12, 12, 12, 235);
    const UIColor moduleBgColor(20, 20, 20, 200);
    const UIColor darkMainColor(mainColor.r * 0.65f, mainColor.g * 0.65f, mainColor.b * 0.65f,
                              mainColor.a);
    const UIColor hoverColor(255, 255, 255, 40);
    const UIColor textColor(245, 245, 245);
    const UIColor disabledTextColor(140, 140, 140);
    const float moduleSpacing = 4.0f;
    const float settingSpacing = 3.0f;


    std::string descriptionText = "NULL";

    openAnim += deltaTime * 2.5f;
    if(openAnim > 1.f)
        openAnim = 1.f;

    if(blurStrength > 0.1f)
        D2D::addBlur(Vec4<float>(0.f, 0.f, screenSize.x, screenSize.y), blurStrength * openAnim);

    // Background overlay
    D2D::fillRoundedRectangle(Vec4<float>(0.f, 0.f, screenSize.x, screenSize.y),
                              UIColor(0, 0, 0, (int)(150 * openAnim)), 0.f);

    for(auto& window : windowList) {
        if(window == draggingWindowPtr) {
            window->pos = window->pos.add(mouseDelta);
        }

        static CustomFont* customFontMod = ModuleManager::getModule<CustomFont>();
        float fontPercent = (float)customFontMod->fontSize / 25.f;

        // Window header
        Vec4<float> hRectPos =
            Vec4<float>(window->pos.x, window->pos.y,
                        window->pos.x + (int)(220 * fontPercent) + (textPaddingX * 2.f),
                        window->pos.y + textHeight + (textPaddingY * 2.f));

        Vec2<float> hTextPos = Vec2<float>(
            hRectPos.x +
                (hRectPos.z - hRectPos.x - D2D::getTextWidth(window->name, textSize)) / 2.f,
            hRectPos.y +
                (hRectPos.w - hRectPos.y - D2D::getTextHeight(window->name, textSize)) / 2.f);

        if(hRectPos.contains(mousePos)) {
            if(isLeftClickDown) {
                draggingWindowPtr = window;
                isLeftClickDown = false;
            } else if(isRightClickDown) {
                window->extended = !window->extended;
                isRightClickDown = false;
            }
        }

        updateSelectedAnimRect(hRectPos, window->selectedAnim);

        // Window header with shadow
        D2D::addDropShadow(hRectPos, windowShadowStrength, UIColor(0, 0, 0, 255), Vec2<float>(0, 2),
                           rounding2);
        D2D::fillRoundedRectangle(hRectPos, bgColor, rounding2);
        D2D::fillRoundedRectangle(hRectPos, headerColor, rounding2);

        std::string windowName = window->name;
        std::transform(windowName.begin(), windowName.end(), windowName.begin(), ::tolower);
        D2D::drawText(hTextPos, windowName, textColor, textSize);

        if(window->selectedAnim > 0.01f) {
            D2D::fillRoundedRectangle(hRectPos,
                                      UIColor(hoverColor.r, hoverColor.g, hoverColor.b,
                                              (int)(hoverColor.a * window->selectedAnim)),
                                      rounding2);
        }

        if(window->extended) {
            float yHeight = moduleSpacing;

            for(auto& mod : window->moduleList) {
                yHeight += textHeight + (textPaddingY * 2.f);

                if(mod->extended) {
                    yHeight += settingSpacing;
                    for(auto& setting : mod->getSettingList()) {
                        if(setting->type != SettingType::COLOR_S) {
                            yHeight += textHeight + (textPaddingY * 2.f);
                        } else {
                            ColorSetting* colorSetting = static_cast<ColorSetting*>(setting);
                            yHeight += textHeight + (textPaddingY * 2.f);
                            if(colorSetting->extended) {
                                yHeight += settingSpacing;
                                for(auto& slider : colorSetting->colorSliders) {
                                    yHeight += textHeight + (textPaddingY * 2.f) + settingSpacing;
                                }
                                yHeight -= settingSpacing;
                            }
                        }
                        yHeight += settingSpacing;
                    }
                }
                yHeight += moduleSpacing;
            }

            float wbgPaddingX = 4.0f * textSize * fontPercent;
            Vec4<float> wbgRectPos =
                Vec4<float>(hRectPos.x, hRectPos.w, hRectPos.z, hRectPos.w + yHeight);

            // Window body with shadow
            D2D::addDropShadow(wbgRectPos, windowShadowStrength, UIColor(0, 0, 0, 255),
                               Vec2<float>(0, 2), rounding2);
            D2D::fillRoundedRectangle(wbgRectPos, bgColor, rounding2);
            D2D::drawRoundedRectangle(wbgRectPos, UIColor(50, 50, 50, 200), rounding2, borderWidth);

            float yOffset = hRectPos.w + moduleSpacing;
            for(auto& mod : window->moduleList) {
                float modPaddingX = wbgPaddingX;
                Vec4<float> mRectPos =
                    Vec4<float>(hRectPos.x + modPaddingX, yOffset, hRectPos.z - modPaddingX,
                                yOffset + textHeight + (textPaddingY * 2.f));

                Vec2<float> mTextPos =
                    Vec2<float>(mRectPos.x + textPaddingX,
                                mRectPos.y + (mRectPos.w - mRectPos.y -
                                              D2D::getTextHeight(mod->getModuleName(), textSize)) /
                                                 2.f);

                if(mRectPos.contains(mousePos)) {
                    descriptionText = mod->getDescription();
                    if(isLeftClickDown) {
                        mod->toggle();
                        isLeftClickDown = false;
                    } else if(isRightClickDown) {
                        mod->extended = !mod->extended;
                        isRightClickDown = false;
                    }
                }

                updateSelectedAnimRect(mRectPos, mod->selectedAnim);

                        D2D::addDropShadow(mRectPos, windowShadowStrength, UIColor(0, 0, 0, 255),
                                   Vec2<float>(0, 2), rounding2);
                UIColor moduleColor = mod->isEnabled() ? darkMainColor : moduleBgColor;
                D2D::fillRoundedRectangle(mRectPos, moduleColor, rounding);
                std::string moduleName = mod->getModuleName();
                std::transform(moduleName.begin(), moduleName.end(), moduleName.begin(), ::tolower);

                D2D::drawText(mTextPos, moduleName,
                              mod->isEnabled() ? textColor
                                               : ColorUtil::lerp(disabledTextColor, textColor,
                                                                 mod->selectedAnim),
                              textSize);

                if(mod->selectedAnim > 0.01f) {
                    D2D::fillRoundedRectangle(mRectPos,
                                              UIColor(hoverColor.r, hoverColor.g, hoverColor.b,
                                                      (int)(hoverColor.a * mod->selectedAnim)),
                                              rounding);
                }

                yOffset += textHeight + (textPaddingY * 2.f);

                if(mod->extended) {
                    yOffset += settingSpacing;
                    float startY = yOffset;

                    float settingPaddingX = 10.0f * textSize * fontPercent;
                    float settingPaddingZ = 6.0f * textSize * fontPercent;

                    for(auto& setting : mod->getSettingList()) {
                        std::string settingName = setting->name;
                        std::transform(settingName.begin(), settingName.end(), settingName.begin(),
                                       ::tolower);

                        Vec4<float> sRectPos = Vec4<float>(
                            mRectPos.x + settingPaddingX, yOffset, mRectPos.z - settingPaddingZ,
                            yOffset + textHeight + (textPaddingY * 2.f));

                        Vec2<float> sTextPos =
                            Vec2<float>(sRectPos.x + textPaddingX,
                                        sRectPos.y + (sRectPos.w - sRectPos.y -
                                                      D2D::getTextHeight(settingName, textSize)) /
                                                         2.f);

                        if(sRectPos.contains(mousePos)) {
                            descriptionText = setting->description;
                        }

                        updateSelectedAnimRect(sRectPos, setting->selectedAnim);

                        switch(setting->type) {
                            case SettingType::BOOL_S: {
                                BoolSetting* boolSetting = static_cast<BoolSetting*>(setting);
                                bool& boolValue = (*boolSetting->value);

                                if(sRectPos.contains(mousePos)) {
                                    if(isLeftClickDown) {
                                        boolValue = !boolValue;
                                        isLeftClickDown = false;
                                    }
                                }

                                float toggleSize = textHeight * 0.8f;
                                Vec4<float> toggleRect = Vec4<float>(
                                    sRectPos.z - textPaddingX - toggleSize,
                                    sRectPos.y + (sRectPos.w - sRectPos.y - toggleSize) / 2.f,
                                    sRectPos.z - textPaddingX,
                                    sRectPos.y + (sRectPos.w - sRectPos.y + toggleSize) / 2.f);

                                D2D::fillRoundedRectangle(
                                    toggleRect, boolValue ? mainColor : moduleBgColor, rounding);

                                D2D::drawText(sTextPos, settingName, textColor, textSize);
                                yOffset += textHeight + (textPaddingY * 2.f);
                                break;
                            }
                            case SettingType::KEYBIND_S: {
                                KeybindSetting* keybindSetting =
                                    static_cast<KeybindSetting*>(setting);
                                int& keyValue = (*keybindSetting->value);

                                if(sRectPos.contains(mousePos)) {
                                    if(isLeftClickDown) {
                                        if(capturingKbSettingPtr == keybindSetting)
                                            capturingKbSettingPtr = nullptr;
                                        else
                                            capturingKbSettingPtr = keybindSetting;
                                        isLeftClickDown = false;
                                    }
                                }

                                std::string keybindName;
                                if(setting == capturingKbSettingPtr) {
                                    keybindName = "...";
                                } else {
                                    keybindName = keyValue != 0 ? KeyNames[keyValue] : "none";
                                }

                                Vec2<float> keybindTextPos =
                                    Vec2<float>(sRectPos.z - textPaddingX -
                                                    D2D::getTextWidth(keybindName, textSize),
                                                sTextPos.y);

                                std::transform(keybindName.begin(), keybindName.end(),
                                               keybindName.begin(), ::tolower);

                                D2D::drawText(sTextPos, settingName + ":", textColor, textSize);
                                D2D::drawText(keybindTextPos, keybindName, textColor, textSize);

                                yOffset += textHeight + (textPaddingY * 2.f);
                                break;
                            }
                            case SettingType::ENUM_S: {
                                EnumSetting* enumSetting = static_cast<EnumSetting*>(setting);
                                int& enumValue = (*enumSetting->value);

                                if(sRectPos.contains(mousePos)) {
                                    if(isLeftClickDown) {
                                        enumValue++;
                                        if(enumValue > enumSetting->enumList.size() - 1)
                                            enumValue = 0;
                                        isLeftClickDown = false;
                                    } else if(isRightClickDown) {
                                        enumValue--;
                                        if(enumValue < 0)
                                            enumValue = (int)enumSetting->enumList.size() - 1;
                                        isRightClickDown = false;
                                    }
                                }

                                std::string modeName = enumSetting->enumList[enumValue];
                                Vec2<float> modeTextPos =
                                    Vec2<float>(sRectPos.z - textPaddingX -
                                                    D2D::getTextWidth(modeName, textSize),
                                                sTextPos.y);

                                std::transform(modeName.begin(), modeName.end(), modeName.begin(),
                                               ::tolower);

                                D2D::drawText(sTextPos, settingName + ":", textColor, textSize);
                                D2D::drawText(modeTextPos, modeName, textColor, textSize);

                                yOffset += textHeight + (textPaddingY * 2.f);
                                break;
                            }
                            case SettingType::COLOR_S: {
                                ColorSetting* colorSetting = static_cast<ColorSetting*>(setting);

                                if(sRectPos.contains(mousePos)) {
                                    if(isRightClickDown) {
                                        colorSetting->extended = !colorSetting->extended;
                                        isRightClickDown = false;
                                    }
                                }

                                float colorBoxSize = std::round(textHeight / 1.4f);
                                float colorBoxPaddingX = textPaddingX + (3.f * textSize);
                                Vec4<float> colorBoxRect =
                                    Vec4<float>(sRectPos.z - colorBoxPaddingX - colorBoxSize,
                                                (sRectPos.y + sRectPos.w - colorBoxSize) / 2.f,
                                                sRectPos.z - colorBoxPaddingX,
                                                (sRectPos.y + sRectPos.w + colorBoxSize) / 2.f);

                                D2D::drawText(sTextPos, settingName + ":", textColor, textSize);
                                D2D::fillRoundedRectangle(colorBoxRect, (*colorSetting->colorPtr),
                                                          rounding);
                                D2D::drawRoundedRectangle(colorBoxRect, UIColor(255, 255, 255, 120),
                                                          rounding, borderWidth);

                                yOffset += textHeight + (textPaddingY * 2.f);

                                if(colorSetting->extended) {
                                    yOffset += settingSpacing;

                                    for(auto& slider : colorSetting->colorSliders) {
                                        Vec4<float> colorSliderRect = Vec4<float>(
                                            sRectPos.x + settingPaddingX, yOffset,
                                            sRectPos.z - settingPaddingZ,
                                            yOffset + textHeight + (textPaddingY * 2.f));

                                        Vec2<float> colorSliderTextPos =
                                            Vec2<float>(colorSliderRect.x + textPaddingX,
                                                        colorSliderRect.y + textPaddingY);

                                        updateSelectedAnimRect(colorSliderRect,
                                                               slider->selectedAnim);

                                        if(colorSliderRect.contains(mousePos)) {
                                            if(isLeftClickDown) {
                                                draggingSliderSettingPtr = slider;
                                                isLeftClickDown = false;
                                            }
                                        }

                                        uint8_t& value = (*slider->valuePtr);
                                        float minValue = (float)slider->minValue;
                                        float maxValue = (float)slider->maxValue;

                                        if(draggingSliderSettingPtr == slider) {
                                            float draggingPercent =
                                                (mousePos.x - colorSliderRect.x) /
                                                (colorSliderRect.z - colorSliderRect.x);
                                            draggingPercent = std::clamp(draggingPercent, 0.f, 1.f);
                                            value = (int)minValue +
                                                    (int)std::round((maxValue - minValue) *
                                                                    draggingPercent);
                                        }

                                        float valuePercent = std::clamp(
                                            (value - minValue) / (maxValue - minValue), 0.f, 1.f);
                                        Vec4<float> valueRectPos = Vec4<float>(
                                            colorSliderRect.x, colorSliderRect.y,
                                            colorSliderRect.x +
                                                (colorSliderRect.z - colorSliderRect.x) *
                                                    valuePercent,
                                            colorSliderRect.w);

                                        char valueText[10];
                                        sprintf_s(valueText, 10, "%i", (int)value);
                                        std::string valueTextStr(valueText);

                                        Vec2<float> valueTextPos = Vec2<float>(
                                            colorSliderRect.z - textPaddingX -
                                                D2D::getTextWidth(valueTextStr, textSize),
                                            colorSliderTextPos.y);

                                        D2D::fillRoundedRectangle(colorSliderRect, moduleBgColor,
                                                                  rounding);
                                        D2D::drawRoundedRectangle(colorSliderRect,
                                                                  UIColor(50, 50, 50), rounding,
                                                                  borderWidth);

                                        D2D::fillRoundedRectangle(valueRectPos, mainColor,
                                                                  rounding);

                                        D2D::drawText(colorSliderTextPos, slider->name + ":",
                                                      textColor, textSize);
                                        D2D::drawText(valueTextPos, valueTextStr, textColor,
                                                      textSize);

                                        if(slider->selectedAnim > 0.01f) {
                                            D2D::fillRoundedRectangle(
                                                colorSliderRect,
                                                UIColor(hoverColor.r, hoverColor.g, hoverColor.b,
                                                        (int)(hoverColor.a * slider->selectedAnim)),
                                                rounding);
                                        }

                                        yOffset +=
                                            textHeight + (textPaddingY * 2.f) + settingSpacing;
                                    }

                                    yOffset -= settingSpacing;
                                }
                                break;
                            }
                            case SettingType::SLIDER_S: {
                                SliderSettingBase* sliderSettingBase =
                                    static_cast<SliderSettingBase*>(setting);

                                if(sRectPos.contains(mousePos)) {
                                    if(isLeftClickDown) {
                                        draggingSliderSettingPtr = sliderSettingBase;
                                        isLeftClickDown = false;
                                    }
                                }

                                if(sliderSettingBase->valueType == ValueType::INT_T) {
                                    SliderSetting<int>* intSlider =
                                        static_cast<SliderSetting<int>*>(sliderSettingBase);
                                    int& value = (*intSlider->valuePtr);
                                    float minValue = (float)intSlider->minValue;
                                    float maxValue = (float)intSlider->maxValue;

                                    if(draggingSliderSettingPtr == sliderSettingBase) {
                                        float draggingPercent =
                                            (mousePos.x - sRectPos.x) / (sRectPos.z - sRectPos.x);
                                        draggingPercent = std::clamp(draggingPercent, 0.f, 1.f);
                                        value =
                                            (int)minValue + (int)std::round((maxValue - minValue) *
                                                                            draggingPercent);
                                    }

                                    float valuePercent = std::clamp(
                                        (value - minValue) / (maxValue - minValue), 0.f, 1.f);
                                    Vec4<float> valueRectPos = Vec4<float>(
                                        sRectPos.x, sRectPos.y,
                                        sRectPos.x + (sRectPos.z - sRectPos.x) * valuePercent,
                                        sRectPos.w);

                                    char valueText[10];
                                    sprintf_s(valueText, 10, "%i", value);
                                    std::string valueTextStr(valueText);

                                    Vec2<float> valueTextPos =
                                        Vec2<float>(sRectPos.z - textPaddingX -
                                                        D2D::getTextWidth(valueTextStr, textSize),
                                                    sTextPos.y);

                                    D2D::fillRoundedRectangle(sRectPos, moduleBgColor, rounding);
                                    D2D::drawRoundedRectangle(sRectPos, UIColor(50, 50, 50),
                                                              rounding, borderWidth);

                                    D2D::fillRoundedRectangle(valueRectPos, mainColor, rounding);

                                    D2D::drawText(sTextPos, settingName + ":", textColor, textSize);
                                    D2D::drawText(valueTextPos, valueTextStr, textColor, textSize);

                                } else if(sliderSettingBase->valueType == ValueType::FLOAT_T) {
                                    SliderSetting<float>* floatSlider =
                                        static_cast<SliderSetting<float>*>(sliderSettingBase);
                                    float& value = (*floatSlider->valuePtr);
                                    float minValue = floatSlider->minValue;
                                    float maxValue = floatSlider->maxValue;

                                    if(draggingSliderSettingPtr == sliderSettingBase) {
                                        float draggingPercent =
                                            (mousePos.x - sRectPos.x) / (sRectPos.z - sRectPos.x);
                                        draggingPercent = std::clamp(draggingPercent, 0.f, 1.f);
                                        value = minValue + (maxValue - minValue) * draggingPercent;
                                    }

                                    float valuePercent = std::clamp(
                                        (value - minValue) / (maxValue - minValue), 0.f, 1.f);
                                    Vec4<float> valueRectPos = Vec4<float>(
                                        sRectPos.x, sRectPos.y,
                                        sRectPos.x + (sRectPos.z - sRectPos.x) * valuePercent,
                                        sRectPos.w);

                                    char valueText[10];
                                    sprintf_s(valueText, 10, "%.2f", value);
                                    std::string valueTextStr(valueText);

                                    Vec2<float> valueTextPos =
                                        Vec2<float>(sRectPos.z - textPaddingX -
                                                        D2D::getTextWidth(valueTextStr, textSize),
                                                    sTextPos.y);

                                    D2D::fillRoundedRectangle(sRectPos, moduleBgColor, rounding);
                                    D2D::drawRoundedRectangle(sRectPos, UIColor(50, 50, 50),
                                                              rounding, borderWidth);

                                    D2D::fillRoundedRectangle(valueRectPos, mainColor, rounding);

                                    D2D::drawText(sTextPos, settingName + ":", textColor, textSize);
                                    D2D::drawText(valueTextPos, valueTextStr, textColor, textSize);
                                }

                                yOffset += textHeight + (textPaddingY * 2.f);
                                break;
                            }
                        }

                        if(setting->selectedAnim > 0.01f) {
                            D2D::fillRoundedRectangle(
                                sRectPos,
                                UIColor(hoverColor.r, hoverColor.g, hoverColor.b,
                                        (int)(hoverColor.a * setting->selectedAnim)),
                                rounding);
                        }

                        yOffset += settingSpacing;
                    }
                }
                yOffset += moduleSpacing;
            }
        }
    }

    if(showDescription && descriptionText != "NULL" && draggingWindowPtr == nullptr &&
       draggingSliderSettingPtr == nullptr) {
        Vec2<float> mousePadding = Vec2<float>(18.f, 18.f);
        float descWidth = D2D::getTextWidth(descriptionText, 0.8f) + 14.f;
        float descHeight = D2D::getTextHeight(descriptionText, 0.8f) + 8.f;

        Vec4<float> rectPos = Vec4<float>(mousePos.x + mousePadding.x, mousePos.y + mousePadding.y,
                                          mousePos.x + mousePadding.x + descWidth,
                                          mousePos.y + mousePadding.y + descHeight);

        if(rectPos.z > screenSize.x)
            rectPos.x = screenSize.x - descWidth - 5.f;
        if(rectPos.w > screenSize.y)
            rectPos.y = screenSize.y - descHeight - 5.f;

        Vec2<float> textPos = Vec2<float>(rectPos.x + 7.f, rectPos.y + 4.f);

        // Description box with shadow
        D2D::addDropShadow(rectPos, 10.0f, UIColor(0, 0, 0, 255), Vec2<float>(0, 2), 6.f);
        D2D::fillRoundedRectangle(rectPos, UIColor(18, 18, 18, 240), 6.f);
        D2D::drawRoundedRectangle(rectPos, mainColor, 6.f, 1.5f);

        std::string lowerDesc = descriptionText;
        std::transform(lowerDesc.begin(), lowerDesc.end(), lowerDesc.begin(), ::tolower);
        D2D::drawText(textPos, lowerDesc, textColor, 0.8f);
    }
}

void ClickGUI::updateSelectedAnimRect(Vec4<float>& rect, float& anim) {
    bool shouldUp = rect.contains(mousePos);

    if(draggingWindowPtr != nullptr)
        shouldUp = false;

    if(draggingSliderSettingPtr != nullptr) {
        if(&draggingSliderSettingPtr->selectedAnim != &anim)
            shouldUp = false;
        else
            shouldUp = true;
    }

    // anim += D2D::deltaTime * (shouldUp ? 5.f : -2.f);
    if(shouldUp)
        anim = 1.f;
    else
        anim -= D2D::deltaTime * 2.f;

    if(anim > 1.f)
        anim = 1.f;
    if(anim < 0.f)
        anim = 0.f;
}

using json = nlohmann::json;

void ClickGUI::onLoadConfig(void* confVoid) {
    json* conf = reinterpret_cast<json*>(confVoid);
    std::string modName = this->getModuleName();

    if(conf->contains(modName)) {
        json obj = conf->at(modName);
        if(obj.is_null())
            return;

        if(obj.contains("enabled")) {
            this->setEnabled(obj.at("enabled").get<bool>());
        }

        for(auto& setting : getSettingList()) {
            std::string settingName = setting->name;

            if(obj.contains(settingName)) {
                json confValue = obj.at(settingName);
                if(confValue.is_null())
                    continue;

                switch(setting->type) {
                    case SettingType::BOOL_S: {
                        BoolSetting* boolSetting = static_cast<BoolSetting*>(setting);
                        (*boolSetting->value) = confValue.get<bool>();
                        break;
                    }
                    case SettingType::KEYBIND_S: {
                        KeybindSetting* keybindSetting = static_cast<KeybindSetting*>(setting);
                        (*keybindSetting->value) = confValue.get<int>();
                        break;
                    }
                    case SettingType::ENUM_S: {
                        EnumSetting* enumSetting = static_cast<EnumSetting*>(setting);
                        (*enumSetting->value) = confValue.get<int>();
                        break;
                    }
                    case SettingType::COLOR_S: {
                        ColorSetting* colorSetting = static_cast<ColorSetting*>(setting);
                        (*colorSetting->colorPtr) =
                            ColorUtil::HexStringToColor(confValue.get<std::string>());
                        break;
                    }
                    case SettingType::SLIDER_S: {
                        SliderSettingBase* sliderSettingBase =
                            static_cast<SliderSettingBase*>(setting);
                        if(sliderSettingBase->valueType == ValueType::INT_T) {
                            SliderSetting<int>* intSlider =
                                static_cast<SliderSetting<int>*>(sliderSettingBase);
                            (*intSlider->valuePtr) = confValue.get<int>();
                        } else if(sliderSettingBase->valueType == ValueType::FLOAT_T) {
                            SliderSetting<float>* floatSlider =
                                static_cast<SliderSetting<float>*>(sliderSettingBase);
                            (*floatSlider->valuePtr) = confValue.get<float>();
                        }
                        break;
                    }
                }
            }
        }

        for(auto& window : windowList) {
            std::string windowName = window->name;

            if(obj.contains(windowName)) {
                json confValue = obj.at(windowName);
                if(confValue.is_null())
                    continue;

                if(confValue.contains("isExtended")) {
                    window->extended = confValue["isExtended"].get<bool>();
                }

                if(confValue.contains("pos")) {
                    window->pos.x = confValue["pos"]["x"].get<float>();
                    window->pos.y = confValue["pos"]["y"].get<float>();
                }
            }
        }
    }
}

void ClickGUI::onSaveConfig(void* confVoid) {
    json* conf = reinterpret_cast<json*>(confVoid);
    std::string modName = this->getModuleName();
    json obj = (*conf)[modName];

    obj["enabled"] = this->isEnabled();

    for(auto& setting : getSettingList()) {
        std::string settingName = setting->name;

        switch(setting->type) {
            case SettingType::BOOL_S: {
                BoolSetting* boolSetting = static_cast<BoolSetting*>(setting);
                obj[settingName] = (*boolSetting->value);
                break;
            }
            case SettingType::KEYBIND_S: {
                KeybindSetting* keybindSetting = static_cast<KeybindSetting*>(setting);
                obj[settingName] = (*keybindSetting->value);
                break;
            }
            case SettingType::ENUM_S: {
                EnumSetting* enumSetting = static_cast<EnumSetting*>(setting);
                obj[settingName] = (*enumSetting->value);
                break;
            }
            case SettingType::COLOR_S: {
                ColorSetting* colorSetting = static_cast<ColorSetting*>(setting);
                obj[settingName] = ColorUtil::ColorToHexString((*colorSetting->colorPtr));
                break;
            }
            case SettingType::SLIDER_S: {
                SliderSettingBase* sliderSettingBase = static_cast<SliderSettingBase*>(setting);
                if(sliderSettingBase->valueType == ValueType::INT_T) {
                    SliderSetting<int>* intSlider =
                        static_cast<SliderSetting<int>*>(sliderSettingBase);
                    obj[settingName] = (*intSlider->valuePtr);
                } else if(sliderSettingBase->valueType == ValueType::FLOAT_T) {
                    SliderSetting<float>* floatSlider =
                        static_cast<SliderSetting<float>*>(sliderSettingBase);
                    obj[settingName] = (*floatSlider->valuePtr);
                }
                break;
            }
        }
    }

    for(auto& window : windowList) {
        obj[window->name]["isExtended"] = window->extended;
        obj[window->name]["pos"]["x"] = window->pos.x;
        obj[window->name]["pos"]["y"] = window->pos.y;
    }

    (*conf)[modName] = obj;
}