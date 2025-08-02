#pragma once
#include <iostream>
#include <string>

#include "../../../../../../SDK/Render/Tessellator.h"
#include "../../../../../../SDK/World/Level/BlockTessellator.h"
#include "../../FuncHook.h"

class TessellateBlockInWorldHook : public FuncHook {
   private:
    // ���º���ǩ����ƥ���°汾
    using func_t = __int64(__fastcall*)(BlockTessellator*, int, Tessellator*, Block*, const BlockPos&, void*);
    static inline func_t oFunc;

    static __int64 TessellateBlockInWorldCallBack(BlockTessellator* _this,
                                                  int a2,  // �����Ĳ���
                                                  Tessellator* tessellator, Block* block,
                                                  const BlockPos& pos,  // BlockPos (ѹ��Ϊint)
                                                  void* airAndSimpleBlocks) {
        // ����ԭ����
        __int64 result = oFunc(_this, a2, tessellator, block, pos, airAndSimpleBlocks);

        // �����İ�ȫ���
        if(!tessellator || !block || !_this) {
            return result;
        }

        auto blok = block;
        Vec3<int> Pos = pos;


        return result;
    }

   public:
    TessellateBlockInWorldHook() {
        OriginFunc = (void*)&oFunc;
        func = (void*)&TessellateBlockInWorldCallBack;
    }
};