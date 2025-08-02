#include "InvManager.h"
#include "../../../../../../SDK/GlobalInstance.h"
#include <Minecraft/InvUtil.h>
#include <chrono>

InvManager::InvManager() : Module("InvManager", "Manages your inventory automatically", Category::PLAYER) {
    registerSetting(new BoolSetting("Drop Useless Items", "Drop useless items", &dropUselessItems, true));
    registerSetting(new BoolSetting("Auto Equip Armor", "Automatically equip better armor", &autoEquipArmor, true));
    registerSetting(new BoolSetting("Manage Tools", "Organize tools in hotbar", &manageTools, true));
    registerSetting(new BoolSetting("Keep Blocks", "Keep building blocks", &keepBlocks, true));
    registerSetting(new BoolSetting("Keep Ender Pearls", "Keep ender pearls", &keepEnderPearls, true));
    registerSetting(new BoolSetting("Keep Fire Protection", "Keep fire protection armor", &keepFireProtection, false));
    
    registerSetting(new SliderSetting<float>("Delay", "Delay between actions (ms)", &delayValue, 100.0f, 50.0f, 300.0f));
    registerSetting(new BoolSetting("Silent Mode", "Reduce chat messages", &silentMode, false));
    registerSetting(new SliderSetting<int>("Max Drops Per Cycle", "Maximum drops per cycle", &maxDropsPerCycle, 15, 5, 36));
    
    // 优先槽位设置
    registerSetting(new SliderSetting<int>("Sword Slot", "Preferred sword slot (0=disabled)", &preferredSwordSlot, 1, 0, 9));
    registerSetting(new SliderSetting<int>("Pickaxe Slot", "Preferred pickaxe slot (0=disabled)", &preferredPickaxeSlot, 2, 0, 9));
    registerSetting(new SliderSetting<int>("Axe Slot", "Preferred axe slot (0=disabled)", &preferredAxeSlot, 3, 0, 9));
    registerSetting(new SliderSetting<int>("Blocks Slot", "Preferred blocks slot (0=disabled)", &preferredBlocksSlot, 9, 0, 9));
}

void InvManager::onNormalTick(LocalPlayer* lp) {
    if (!lp || !lp->getsupplies()) return;
    
    // 获取当前屏幕名称
    std::string currentScreenName = GI::getClientInstance()->getScreenName();
    
    // 检测背包界面状态变化
    if (currentScreenName == "inventory_screen") {
        if (!wasInventoryOpen) {
            // 刚打开背包，标记状态并设置执行标志
            wasInventoryOpen = true;
            shouldExecuteFullCycle = true;
            
            if (!silentMode) {
                lp->displayClientMessage("InvManager: Inventory opened, starting cleanup...");
            }
        }
        
        // 如果需要执行完整流程
        if (shouldExecuteFullCycle) {
            executeFullCleanup(lp);
        }
    } else {
        // 不在背包界面，重置状态
        if (wasInventoryOpen) {
            wasInventoryOpen = false;
            if (!silentMode) {
                lp->displayClientMessage("InvManager: Inventory closed.");
            }
        }
    }
}

void InvManager::executeFullCleanup(LocalPlayer* player) {
    if (!player || !player->getsupplies()) return;
    
    static auto lastExecution = std::chrono::high_resolution_clock::now();
    auto now = std::chrono::high_resolution_clock::now();
    auto timeSinceLastExecution = std::chrono::duration_cast<std::chrono::milliseconds>(now - lastExecution).count();
    
    // 防止过于频繁执行
    if (timeSinceLastExecution < delayValue) {
        return;
    }
    
    if (!silentMode) {
        player->displayClientMessage("InvManager: Executing full cleanup cycle...");
    }
    
    // 步骤1：扫描并找到最佳装备
    scanBestEquipment();
    
    // 步骤2：丢弃所有非最佳装备和无用物品（一次性完成）
    if (dropUselessItems) {
        dropAllNonBestItems(player);
    }
    
    // 步骤3：装备更好的护甲（全部）
    if (autoEquipArmor) {
        equipAllBetterArmor(player);
    }
    
    // 步骤4：整理热键栏（全部）
    if (manageTools) {
        organizeAllHotbar(player);
    }
    
    // 完成后标记不再执行
    shouldExecuteFullCycle = false;
    lastExecution = now;
    
    if (!silentMode) {
        player->displayClientMessage("InvManager: Full cleanup cycle completed!");
    }
}

void InvManager::scanBestEquipment() {
    // 重置所有最佳装备槽位
    bestHelmet = -1; bestChestplate = -1; bestLeggings = -1; bestBoots = -1;
    bestSword = -1; bestPickaxe = -1; bestAxe = -1; bestShovel = -1;
    
    int bestValues[8] = {0}; // 对应8种装备的最佳价值
    
    for (int i = 0; i < 36; i++) {
        ItemStack* item = InvUtil::getItem(i);
        if (!InvUtil::isVaildItem(item)) continue;
        
        Item* itemData = item->getItem();
        if (!itemData) continue;
        
        SItemType itemType = itemData->getItemType();
        if (!isEquipmentType(itemType)) continue;
        
        int itemValue = calculateItemValue(item, itemType);
        updateBestEquipment(itemType, i, itemValue, bestValues);
    }
}

bool InvManager::isEquipmentType(SItemType itemType) {
    return itemType == SItemType::Helmet || itemType == SItemType::Chestplate ||
           itemType == SItemType::Leggings || itemType == SItemType::Boots ||
           itemType == SItemType::Sword || itemType == SItemType::Pickaxe ||
           itemType == SItemType::Axe || itemType == SItemType::Shovel;
}

void InvManager::updateBestEquipment(SItemType itemType, int slot, int value, int* bestValues) {
    switch (itemType) {
        case SItemType::Helmet:
            if (value > bestValues[0]) { bestValues[0] = value; bestHelmet = slot; }
            break;
        case SItemType::Chestplate:
            if (value > bestValues[1]) { bestValues[1] = value; bestChestplate = slot; }
            break;
        case SItemType::Leggings:
            if (value > bestValues[2]) { bestValues[2] = value; bestLeggings = slot; }
            break;
        case SItemType::Boots:
            if (value > bestValues[3]) { bestValues[3] = value; bestBoots = slot; }
            break;
        case SItemType::Sword:
            if (value > bestValues[4]) { bestValues[4] = value; bestSword = slot; }
            break;
        case SItemType::Pickaxe:
            if (value > bestValues[5]) { bestValues[5] = value; bestPickaxe = slot; }
            break;
        case SItemType::Axe:
            if (value > bestValues[6]) { bestValues[6] = value; bestAxe = slot; }
            break;
        case SItemType::Shovel:
            if (value > bestValues[7]) { bestValues[7] = value; bestShovel = slot; }
            break;
    }
}

int InvManager::calculateItemValue(ItemStack* item, SItemType itemType) {
    if (!InvUtil::isVaildItem(item)) return 0;
    
    Item* itemData = item->getItem();
    if (!itemData) return 0;

    int value = 0;
    
    // 基础等级评分
    if (itemType == SItemType::Helmet || itemType == SItemType::Chestplate || 
        itemType == SItemType::Leggings || itemType == SItemType::Boots) {
        
        int tier = itemData->getArmorTier();
        value += tier * 100;
        
        // 只检测保护附魔
        value += item->getEnchantValue(static_cast<int>(Enchant::PROTECTION)) * 25;
        
        // 火焰保护特殊处理
        if (keepFireProtection && item->getEnchantValue(static_cast<int>(Enchant::FIRE_PROTECTION)) > 0) {
            value += 200;
        }
        
    } else if (itemType == SItemType::Sword) {
        
        int tier = itemData->getItemTier();
        value += tier * 80;
        
        // 武器只检测锋利
        value += item->getEnchantValue(static_cast<int>(Enchant::SHARPNESS)) * 15;
        
    } else if (itemType == SItemType::Pickaxe || itemType == SItemType::Axe || itemType == SItemType::Shovel) {
        
        int tier = itemData->getItemTier();
        value += tier * 80;
        
        // 工具只检测效率
        value += item->getEnchantValue(static_cast<int>(Enchant::EFFICIENCY)) * 12;
    }
    
    // 简化耐久度计算
    if (itemData->getMaxDamage() > 0) {
        short damage = item->getDamageValue();
        float durabilityPercent = 1.0f - (float(damage) / itemData->getMaxDamage());
        if (durabilityPercent < 0.5f) {
            value = int(value * 0.7f); // 低耐久度降低价值
        }
    }
    
    return value;
}

void InvManager::dropAllNonBestItems(LocalPlayer* player) {
    if (!player || !player->getsupplies()) return;
    
    int droppedCount = 0;
    std::vector<int> itemsToDrop;
    
    // 收集所有要丢弃的物品
    for (int i = 0; i < 36; i++) {
        ItemStack* item = InvUtil::getItem(i);
        if (!InvUtil::isVaildItem(item)) continue;
        
        Item* itemData = item->getItem();
        if (!itemData) continue;
        
        // 检查是否应该保留这个物品
        if (!shouldKeepItem(item, i)) {
            itemsToDrop.push_back(i);
            if (itemsToDrop.size() >= maxDropsPerCycle) {
                break;
            }
        }
    }
    
    // 批量丢弃
    for (int slot : itemsToDrop) {
        try {
            ItemStack* item = InvUtil::getItem(slot);
            if (!InvUtil::isVaildItem(item)) continue;
            
            Item* itemData = item->getItem();
            if (!itemData) continue;
            
            player->getsupplies()->dropSlot(slot);
            
            if (!silentMode) {
                player->displayClientMessage("InvManager: Dropped " + itemData->mName + " from slot " + std::to_string(slot));
            }
            
            droppedCount++;
            
        } catch (...) {
            if (!silentMode) {
                player->displayClientMessage("InvManager: Failed to drop item from slot " + std::to_string(slot));
            }
        }
    }
    
    if (droppedCount > 0 && !silentMode) {
        player->displayClientMessage("InvManager: Dropped " + std::to_string(droppedCount) + " items");
    }
}

bool InvManager::shouldKeepItem(ItemStack* item, int slot) {
    if (!InvUtil::isVaildItem(item)) return false;
    
    Item* itemData = item->getItem();
    if (!itemData) return false;
    
    // 检查是否是方块
    if (keepBlocks && item->isBlock()) {
        return true;
    }
    
    // 检查是否是末影珍珠
    if (keepEnderPearls && itemData->mName.find("ender_pearl") != std::string::npos) {
        return true;
    }
    
    // 检查是否是最佳装备
    SItemType itemType = itemData->getItemType();
    if (isEquipmentType(itemType)) {
        return isBestEquipment(slot);
    }
    
    // 其他物品都不保留
    return false;
}

bool InvManager::isBestEquipment(int slot) {
    return slot == bestHelmet || slot == bestChestplate || slot == bestLeggings || 
           slot == bestBoots || slot == bestSword || slot == bestPickaxe || 
           slot == bestAxe || slot == bestShovel;
}

void InvManager::equipAllBetterArmor(LocalPlayer* player) {
    if (!player || !player->getsupplies()) return;
    
    auto armorContainer = player->getArmorContainer();
    if (!armorContainer) return;
    
    int bestSlots[4] = {bestHelmet, bestChestplate, bestLeggings, bestBoots};
    int equippedCount = 0;
    
    // 一次性检查所有护甲槽位
    for (int armorSlot = 0; armorSlot < 4; armorSlot++) {
        int bestSlot = bestSlots[armorSlot];
        if (bestSlot == -1) continue;
        
        ItemStack* currentArmor = armorContainer->getItem(armorSlot);
        ItemStack* bestArmor = InvUtil::getItem(bestSlot);
        
        if (!bestArmor) continue;
        
        SItemType armorType = bestArmor->getItem()->getItemType();
        int currentValue = calculateItemValue(currentArmor, armorType);
        int bestValue = calculateItemValue(bestArmor, armorType);
        
        if (bestValue > currentValue) {
            try {
                player->getsupplies()->equipArmor(bestSlot);
                
                if (!silentMode) {
                    player->displayClientMessage("InvManager: Equipped armor from slot " + std::to_string(bestSlot));
                }
                
                equippedCount++;
                
            } catch (...) {
                if (!silentMode) {
                    player->displayClientMessage("InvManager: Failed to equip armor from slot " + std::to_string(bestSlot));
                }
            }
        }
    }
    
    if (equippedCount > 0 && !silentMode) {
        player->displayClientMessage("InvManager: Equipped " + std::to_string(equippedCount) + " armor pieces");
    }
}

void InvManager::organizeAllHotbar(LocalPlayer* player) {
    if (!player || !player->getsupplies()) return;
    
    int organizedCount = 0;
    
    // 整理剑到指定槽位
    if (preferredSwordSlot > 0 && bestSword != -1) {
        int targetSlot = preferredSwordSlot - 1;
        if (bestSword != targetSlot) {
            try {
                player->getsupplies()->swapSlots(bestSword, targetSlot);
                if (!silentMode) {
                    player->displayClientMessage("InvManager: Moved sword to slot " + std::to_string(preferredSwordSlot));
                }
                organizedCount++;
            } catch (...) {}
        }
    }
    
    // 整理镐到指定槽位
    if (preferredPickaxeSlot > 0 && bestPickaxe != -1) {
        int targetSlot = preferredPickaxeSlot - 1;
        if (bestPickaxe != targetSlot) {
            try {
                player->getsupplies()->swapSlots(bestPickaxe, targetSlot);
                if (!silentMode) {
                    player->displayClientMessage("InvManager: Moved pickaxe to slot " + std::to_string(preferredPickaxeSlot));
                }
                organizedCount++;
            } catch (...) {}
        }
    }
    
    // 整理斧到指定槽位
    if (preferredAxeSlot > 0 && bestAxe != -1) {
        int targetSlot = preferredAxeSlot - 1;
        if (bestAxe != targetSlot) {
            try {
                player->getsupplies()->swapSlots(bestAxe, targetSlot);
                if (!silentMode) {
                    player->displayClientMessage("InvManager: Moved axe to slot " + std::to_string(preferredAxeSlot));
                }
                organizedCount++;
            } catch (...) {}
        }
    }
    
    // 整理方块到指定槽位
    if (preferredBlocksSlot > 0) {
        int targetSlot = preferredBlocksSlot - 1;
        ItemStack* currentItem = InvUtil::getItem(targetSlot);
        
        if (!currentItem || !currentItem->isBlock()) {
            int firstBlockSlot = findFirstBlock();
            if (firstBlockSlot != -1 && firstBlockSlot != targetSlot) {
                try {
                    player->getsupplies()->swapSlots(firstBlockSlot, targetSlot);
                    if (!silentMode) {
                        player->displayClientMessage("InvManager: Moved blocks to slot " + std::to_string(preferredBlocksSlot));
                    }
                    organizedCount++;
                } catch (...) {}
            }
        }
    }
    
    if (organizedCount > 0 && !silentMode) {
        player->displayClientMessage("InvManager: Organized " + std::to_string(organizedCount) + " hotbar slots");
    }
}

int InvManager::findFirstBlock() {
    for (int i = 0; i < 36; i++) {
        ItemStack* item = InvUtil::getItem(i);
        if (InvUtil::isVaildItem(item) && item->isBlock()) {
            return i;
        }
    }
    return -1;
}