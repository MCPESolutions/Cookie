#pragma once
#include "../FuncHook.h"

class SetColorHook : public FuncHook {
private:
	using func_t = mce::Color* (__fastcall*)(__int64, mce::Color*, __int64);
	static inline func_t oFunc;

	static  mce::Color* SetColorCallback(__int64 a1, mce::Color* color, __int64 a3) {
		__int64 actor = *(uintptr_t*)(a3 + 0x38);
        static ModelOverlayColor* hurtColorMod = ModuleManager::getModule<ModelOverlayColor>();
		return oFunc(a1, color, a3); // The Original Color is MC_Color(1.f, 0.f, 0.f, 0.6f)
	}
public:
	SetColorHook() {
		OriginFunc = (void*)&oFunc;
		func = (void*)&SetColorCallback;
	}
	// search for "minecraft:totem_particle" lel
};