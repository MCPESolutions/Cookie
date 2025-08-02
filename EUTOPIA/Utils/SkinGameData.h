#pragma once
#include <string>
#include <unordered_map>
#include <vector>
#include "../SDK/Core/mce.h"
#include "../SDK/World/Actor/Components/ActorUniqueIDComponent.h"
#include "../SDK/World/Level/Level.h"

namespace std {
template <>
struct hash<mce::UUID> {
    size_t operator()(const mce::UUID& uuid) const noexcept {
        return std::hash<uint64_t>()(uuid.mLow) ^ (std::hash<uint64_t>()(uuid.mHigh) << 1);
    }
};
}  // namespace std

struct PlayerEntry {
    std::string name;
    mce::PlayerSkin* skin;
    std::string xuid;
    ActorUniqueIDComponent id;
    std::vector<unsigned char> croppedHead;  
    std::vector<unsigned char> fullSkinData;  // 新增：完整皮肤数据
    int skinWidth;  // 新增：皮肤宽度
    int skinHeight; // 新增：皮肤高度
    mce::UUID uuid;

    PlayerEntry(std::string _name, mce::PlayerSkin* _skin, std::string _xuid,
                ActorUniqueIDComponent _id, std::vector<unsigned char> _crop, 
                std::vector<unsigned char> _fullSkin, int _width, int _height, mce::UUID _uuid)
        : name(_name), skin(_skin), xuid(_xuid), id(_id), croppedHead(_crop), 
          fullSkinData(_fullSkin), skinWidth(_width), skinHeight(_height), uuid(_uuid) {};
    PlayerEntry() {};
};

class SkinGameData {
   public:
    static inline std::unordered_map<std::string, PlayerEntry> playerlist;
    static inline std::unordered_map<std::string, std::vector<unsigned char>> skinmap;
    static inline std::unordered_map<std::string, std::vector<unsigned char>> fullskinmap; // 新增：完整皮肤映射

    static void updatePlayerList(std::unordered_map<mce::UUID, PlayerListEntry>* list);

    static bool hasPlayerSkin(const std::string& playerName) {
        auto it = skinmap.find(playerName);
        return it != skinmap.end() && !it->second.empty();
    }

    static const std::vector<unsigned char>& getPlayerSkin(const std::string& playerName) {
        static std::vector<unsigned char> empty;
        auto it = skinmap.find(playerName);
        return (it != skinmap.end()) ? it->second : empty;
    }

    // 新增：获取完整皮肤数据
    static bool hasPlayerFullSkin(const std::string& playerName) {
        auto it = fullskinmap.find(playerName);
        return it != fullskinmap.end() && !it->second.empty();
    }

    static const std::vector<unsigned char>& getPlayerFullSkin(const std::string& playerName) {
        static std::vector<unsigned char> empty;
        auto it = fullskinmap.find(playerName);
        return (it != fullskinmap.end()) ? it->second : empty;
    }

    // 新增：获取皮肤尺寸
    static Vec2<int> getPlayerSkinSize(const std::string& playerName) {
        auto it = playerlist.find(playerName);
        if(it != playerlist.end()) {
            return Vec2<int>(it->second.skinWidth, it->second.skinHeight);
        }
        return Vec2<int>(64, 64); // 默认尺寸
    }
};