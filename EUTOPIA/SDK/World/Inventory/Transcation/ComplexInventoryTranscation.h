#pragma once
#include <cstdint>
#include "InventoryTranscation.h"
class ComplexInventoryTransaction {
public:
    // ComplexInventoryTransaction inner types define
    enum class Type : int {
        NormalTransaction = 0x0,
        InventoryMismatch = 0x1,
        ItemUseTransaction = 0x2,
        ItemUseOnEntityTransaction = 0x3,
        ItemReleaseTransaction = 0x4,
    };

    uintptr_t** vtable; // this+0x0
    Type                 type;
    InventoryTransaction data;

    ComplexInventoryTransaction()
    {
        static uintptr_t vtable_addr =  (uintptr_t)MemoryUtil::getVtableFromSig("48 8D 05 ? ? ? ? 48 8B F9 48 89 01 8B DA 48 83 C1 ? E8 ? ? ? ? F6 C3 ? 74 ? BA ? ? ? ? 48 8B CF E8 ? ? ? ? 48 8B 5C 24 ? 48 8B C7 48 83 C4 ? 5F C3 CC CC CC CC CC CC CC CC CC CC CC CC CC CC 48 89 5C 24 ? 48 89 74 24 ? 57 41 56");
        vtable = reinterpret_cast<uintptr_t**>(vtable_addr);
        type = Type::NormalTransaction;
        data = InventoryTransaction();
        data.mActions = std::unordered_map<InventorySource, std::vector<InventoryAction>>();
        data.mItems = std::vector<InventoryTransactionItemGroup>();
    };

    Type getTransacType() {
        return type;
    }
};