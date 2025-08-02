#include "ChestStealer.h"

#include "../../../../../../Utils/Maths.h"
#include "../../../../../../SDK/NetWork/PacketSender.h"
#include "../../../../../../SDK/World/Inventory/ContainerManagerModel.h"
#include <Minecraft/InvUtil.h>

ChestStealer::ChestStealer()
    : Module("ChestStealer", "Steal all items from opened chests", Category::PLAYER) {
    registerSetting(new SliderSetting<int>("Delay", "Base delay between items (ms)", &mDelayValue, 0, 0, 2000));
    registerSetting(new BoolSetting("Randomize", "Randomise delay", &mRandomizeDelay, false));
    registerSetting(new SliderSetting<int>("RandMin", "Min random delay (ms)", &mRandomMin, 50, 10, 500));
    registerSetting(new SliderSetting<int>("RandMax", "Max random delay (ms)", &mRandomMax, 120, 20, 1000));
}

void ChestStealer::onEnable() {
}

void ChestStealer::onDisable() {}

void ChestStealer::onNormalTick(LocalPlayer* player) {}

// Handle container open / close packets to track state
void ChestStealer::onReceivePacket(Packet* pkt, bool* cancel) {

}
int startingEmptySlot = -1;

int getFirstEmptySlot() {
    auto player = GI::getLocalPlayer();
    for(int i = 0; i < 36; i++) {
        if(startingEmptySlot != -1 && i < startingEmptySlot)
            continue;

        ItemStack* stack = player->supplies->container->getItem(i);
        if(!InvUtil::isVaildItem(stack)) {
            startingEmptySlot = i + 1;
            return i;
        }
    }
    startingEmptySlot = -1;
    return -1;
}

void ChestStealer::onChestScreen(ContainerScreenController* csc) {
    auto player = GI::getLocalPlayer();
    if(!player)
        return;

        static uint64_t lastSteal = 0;
    static bool isStealing = false;
    uint64_t now = TimerUtil::getCurrentMs();


    ContainerManagerModel* chest = player->getContainerManagerModel();
    if(!chest) {
        return;
    }


    std::vector<int> itemz = {};
    for(int i = 0; i < 54; i++) {
        ItemStack* stack = chest->getSlot(i);
        if(stack && stack->mItem)
            itemz.push_back(i);
    }
    if(itemz.empty()) {
        if(lastSteal + 200 < now) {
            isStealing = false;
            csc->_tryExit();
        }
        return;
    }
    static uint64_t delay = getDelayMs();

    if(!isStealing) {
        isStealing = true;
    }

    if(lastSteal + mDelayValue < now) {
        for(const int i : itemz) {
            csc->handleAutoPlace("container_items", i);
            lastSteal = now;
            delay = getDelayMs();  
            break;
        }
    }
    if(shouldDelay())
        return;
}

// ----------  helpers  ----------

void ChestStealer::takeItem(int chestSlot, ItemStack* item) {
    /*auto player =GI::getLocalPlayer();

    int from = chestSlot;
    int to = getFirstEmptySlot();
    ItemStack* item2 = player->supplies->container->getItem(to);
    auto item1 = item;

    InventoryAction action = InventoryAction(from, item1, ItemStack::getEmptyItem());
    action.mSource.mType = InventorySourceType::ContainerInventory;
    action.mSource.mContainerId = static_cast<int>(ContainerID::Chest);

    InventoryAction action2 = InventoryAction(to, ItemStack::getEmptyItem(), item1);
    action2.mSource.mType = InventorySourceType::ContainerInventory;
    action2.mSource.mContainerId = static_cast<int>(ContainerID::Inventory);

    auto pkt = MinecraftPacket::createPacket<InventoryTransactionPacket>();

    auto cit = std::make_unique<ComplexInventoryTransaction>();
    cit->data.addAction(action);
    cit->data.addAction(action2);

    pkt->mTransaction = std::move(cit);

   GI::getPacketSender()->sendToServer(pkt.get());*/
}

uint64_t ChestStealer::getDelayMs() const {
    if(!mRandomizeDelay) return static_cast<uint64_t>(mDelayValue);
    // Ensure min <= max
    int lo = std::min(mRandomMin, mRandomMax);
    int hi = std::max(mRandomMin, mRandomMax);
    return static_cast<uint64_t>(Math::randomInt(lo, hi));
}

bool ChestStealer::shouldDelay() {
    if(mDelayValue == 0 && !mRandomizeDelay) return false;
    mLastItemTaken = TimerUtil::getCurrentMs();
    return true;
} 