#pragma once
#include "../../ModuleBase/Module.h"
#include <vector>
#include <set>

class InvManager : public Module {
private:
    // 基础设置变量
    bool dropUselessItems = true;
    bool autoEquipArmor = true;
    bool manageTools = true;
    bool keepBlocks = true;
    bool keepEnderPearls = true;
    bool keepFireProtection = false;

    float delayValue = 100.0f;
    bool silentMode = false;
    int maxDropsPerCycle = 15;

    // 优先槽位设置
    int preferredSwordSlot = 1;
    int preferredPickaxeSlot = 2;
    int preferredAxeSlot = 3;
    int preferredBlocksSlot = 9;

    // 状态变量
    bool wasInventoryOpen = false;
    bool shouldExecuteFullCycle = false;

    // 最佳装备槽位
    int bestHelmet = -1;
    int bestChestplate = -1;
    int bestLeggings = -1;
    int bestBoots = -1;
    int bestSword = -1;
    int bestPickaxe = -1;
    int bestAxe = -1;
    int bestShovel = -1;

public:
    InvManager();
    void onNormalTick(LocalPlayer* lp) override;

private:
    // 核心功能函数
    void executeFullCleanup(LocalPlayer* player);
    void scanBestEquipment();
    int calculateItemValue(ItemStack* item, SItemType itemType);

    // 物品分类函数
    bool shouldKeepItem(ItemStack* item, int slot);
    bool isEquipmentType(SItemType itemType);
    bool isBestEquipment(int slot);

    // 装备管理函数
    void dropAllNonBestItems(LocalPlayer* player);
    void equipAllBetterArmor(LocalPlayer* player);
    void organizeAllHotbar(LocalPlayer* player);

    // 辅助函数
    void updateBestEquipment(SItemType itemType, int slot, int value, int* bestValues);
    int findFirstBlock();
};