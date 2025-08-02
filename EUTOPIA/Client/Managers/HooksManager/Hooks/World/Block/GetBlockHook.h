#pragma once
#include <iostream>
#include <string>

#include "../../FuncHook.h"

class GetBlockHook : public FuncHook {
   private:
    using func_t = Block*(__thiscall*)(BlockSource*, const BlockPos&);
    static inline func_t oFunc;

    static Block* BlockSourceGetBlockCallback(BlockSource* _this, const BlockPos& blockPos) {
        Block* result = oFunc(_this, blockPos);

            int blockId = result->blockLegcy->blockid;
        std::string blockName = result->blockLegcy->mtranslateName;
  
        return result;
    }

   public:
    GetBlockHook() {
        OriginFunc = (void*)&oFunc;
        func = (void*)&BlockSourceGetBlockCallback;
    }
};
