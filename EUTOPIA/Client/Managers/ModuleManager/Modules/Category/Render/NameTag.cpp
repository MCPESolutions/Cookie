#include "NameTag.h"

#include <Minecraft/TargetUtil.h>

#include "../../../../../../SDK/Render/Matrix.h"
#include "../../Utils/MCR.h"

NameTags::NameTags() : Module("NameTags", "Better nametags.", Category::RENDER) {
    registerSetting(new SliderSetting<float>("Size", "NULL", &tagSize, 0.5f, 0.f, 1.f));
    registerSetting(new SliderSetting<float>("Opacity", "NULL", &opacity, 1.f, 0.f, 1.f));
    registerSetting(new BoolSetting("Self", "Render urself", &showSelf, true));
    registerSetting(new BoolSetting("Underline", "Show a line under", &underline, false));
    registerSetting(new BoolSetting("Dynamic Size", "NULL", &dynamicSize, true));
}

static bool envaledchar(char c) {
    return !(c >= 0 && *reinterpret_cast<unsigned char*>(&c) < 128);
}

std::string sanitizex(const std::string& text) {
    std::string out;
    bool wasValid = true;
    for(char c : text) {
        bool isValid = !envaledchar(c);
        if(wasValid) {
            if(!isValid) {
                wasValid = false;
            } else {
                out += c;
            }
        } else {
            wasValid = isValid;
        }
    }
    return out;
}

void NameTags::onD2DRender() {
    LocalPlayer* lp = GI::getLocalPlayer();
    if(!lp)
        return;

    for(Actor* actor : lp->getlevel()->getRuntimeActorList()) {
        if(!TargetUtil::isTargetValid(actor, false, 99.f) && !(actor == lp && showSelf))
            continue;

        Vec2<float> pos;
        if(!Matrix::WorldToScreen(actor->getEyePos().add2(0.f, 0.75f, 0.f), pos))
            continue;

        std::string name = actor->getNameTag();

        float textSize = 1.f * tagSize;
        if(dynamicSize) {
            float distance = lp->getPos().dist(actor->getPos());
            float dynamicFactor;
            if(distance <= 1.f) {
                dynamicFactor = 3.f;
            } else if(distance >= 4.f) {
                dynamicFactor = 1.0f;
            } else {
                dynamicFactor = 3.f - ((distance - 1.f) * (2.0f / 3.f));
            }
            textSize = tagSize * dynamicFactor;
        }

        std::string sanitizedMessage = sanitizex(name);
        float textWidth = D2D::getTextWidth(sanitizedMessage, textSize);
        float textHeight = D2D::getTextHeight(sanitizedMessage, textSize);
        float textPadding = 1.f * textSize;
        Vec2<float> textPos = Vec2<float>(pos.x - textWidth / 2.f, pos.y - textHeight / 2.f);
        Vec4<float> rectPos = Vec4<float>(textPos.x - textPadding * 3.0f, textPos.y - textPadding,
                                          textPos.x + textWidth + textPadding * 3.0f,
                                          textPos.y + textHeight + textPadding);

        float health = actor->getHealth();
        float maxHealth = actor->getMaxHealth();
        float healthRatio = std::min(health / maxHealth, 1.0f);

        float barWidth = rectPos.z - rectPos.x;
        float barHeight = 2.f * textSize;
        Vec4<float> healthBg =
            Vec4<float>(rectPos.x, rectPos.y - barHeight - 2.f, rectPos.z, rectPos.y - 2.f);
        Vec4<float> healthFg = Vec4<float>(rectPos.x, rectPos.y - barHeight - 2.f,
                                           rectPos.x + (barWidth * healthRatio), rectPos.y - 2.f);

        D2D::fillRectangle(healthBg, UIColor(40, 40, 40, static_cast<int>(255 * opacity)));
        D2D::fillRectangle(healthFg, UIColor(255 - static_cast<int>(healthRatio * 255),
                                                    static_cast<int>(healthRatio * 255), 0,
                                                    static_cast<int>(255 * opacity)));
        D2D::fillRectangle(rectPos, UIColor(12, 27, 46, static_cast<int>(255 * opacity)));
        if(underline)
            D2D::fillRectangle(
                Vec4<float>(rectPos.x, rectPos.w - 1.f * textSize, rectPos.z, rectPos.w),
                UIColor(255, 255, 255, 255));
        D2D::drawText(textPos, sanitizedMessage, UIColor(255, 255, 255, 255), textSize,
                             true);
    }
}

int waitForInit = 0;
void NameTags::onMCRender(MinecraftUIRenderContext* renderCtx) {}
