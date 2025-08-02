#include "Clip.h"
#include "../../SDK/Client/ClientInstance.h"
#include "../../Utils/GlobalInstance.h"
#include "../../Utils/DrawUtil.h"
#include "../../Utils/UIColor.h"

Clip::Clip() : Module("Clip", "Player", "Allows you to clip through blocks", 0) {
    addSlider("Reduce", "Reduce player hitbox", &offset, 0.0f, 1.0f, 0.1f);
}

void Clip::onNormalTick() {
    auto localPlayer = GI::getLocalPlayer();
    if (!localPlayer) return;
    
    auto region = GI::getRegion();
    if (!region) return;

    // Get player position and convert to BlockPos
    Vec3<float> playerPos = localPlayer->getPos();
    BlockPos blockPos = playerPos.floor().CastTo<int>();

    // Adjust player AABB
    auto aabbComponent = localPlayer->getAABBShapeComponent();
    if (aabbComponent) {
        AABB originalAABB = aabbComponent->aabb;
        AABB newAABB = originalAABB;
        
        // Reduce AABB size
        float reduction = offset;
        newAABB.lower.x += reduction;
        newAABB.lower.z += reduction;
        newAABB.upper.x -= reduction;
        newAABB.upper.z -= reduction;
        
        aabbComponent->setAABB(newAABB);
    }

    // Check blocks around player
    blockcheck1 = false;
    blockcheck2 = false;
    blockcheck3 = false;
    blockcheck4 = false;

    // Check block at player position
    Block* block = region->getBlock(blockPos);
    if (block && block->blockLegcy) {
        uint16_t blockId = block->blockLegcy->blockid;
        if (blockId == 7 || blockId == 49) { // Bedrock or Obsidian
            blockcheck1 = true;
        }
    }

    // Check blocks around player (4 directions)
    BlockPos checkPositions[4] = {
        blockPos.add(Vec3<int>(1, 0, 0)),   // East
        blockPos.add(Vec3<int>(-1, 0, 0)),  // West
        blockPos.add(Vec3<int>(0, 0, 1)),   // South
        blockPos.add(Vec3<int>(0, 0, -1))   // North
    };

    bool* checkResults[4] = {&blockcheck2, &blockcheck3, &blockcheck4, &blockcheck1};

    for (int i = 0; i < 4; i++) {
        Block* checkBlock = region->getBlock(checkPositions[i]);
        if (checkBlock && checkBlock->blockLegcy) {
            uint16_t checkBlockId = checkBlock->blockLegcy->blockid;
            if (checkBlockId == 7 || checkBlockId == 49) {
                *checkResults[i] = true;
            }
        }
    }
}

void Clip::onLevelRender() {
    auto localPlayer = GI::getLocalPlayer();
    if (!localPlayer) return;

    Vec3<float> playerPos = localPlayer->getPos();
    BlockPos blockPos = playerPos.floor().CastTo<int>();

    UIColor renderColor = UIColor(255, 0, 0, 100); // Red with transparency
    UIColor lineColor = UIColor(255, 0, 0, 255);   // Red outline

    // Draw boxes based on block checks
    if (blockcheck1) {
        Vec3<float> pos1 = blockPos.CastTo<float>();
        Vec3<float> pos2 = blockPos.add(Vec3<int>(2, 1, 0)).CastTo<float>();
        DrawUtil::drawBoxFilledPre(pos1, pos2, renderColor, lineColor);
    }

    if (blockcheck2 && !blockcheck1) {
        Vec3<float> pos1 = blockPos.CastTo<float>();
        Vec3<float> pos2 = blockPos.add(Vec3<int>(1, 1, 1)).CastTo<float>();
        DrawUtil::drawBoxFilledPre(pos1, pos2, renderColor, lineColor);
    }

    if (blockcheck3 && !blockcheck1 && !blockcheck2) {
        Vec3<float> pos1 = blockPos.CastTo<float>();
        Vec3<float> pos2 = blockPos.add(Vec3<int>(0, 1, 2)).CastTo<float>();
        DrawUtil::drawBoxFilledPre(pos1, pos2, renderColor, lineColor);
    }

    if (blockcheck4 && !blockcheck1 && !blockcheck2 && !blockcheck3) {
        Vec3<float> pos1 = blockPos.CastTo<float>();
        Vec3<float> pos2 = blockPos.add(Vec3<int>(1, 1, 1)).CastTo<float>();
        DrawUtil::drawBoxFilledPre(pos1, pos2, renderColor, lineColor);
    }
}