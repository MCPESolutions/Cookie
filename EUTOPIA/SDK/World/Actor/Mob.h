#pragma once
#include"Actor.h"
class Mob : public Actor { // Mob Class 1.21.60
public:
	void setSprinting(bool shouldSprint) {
		MemoryUtil::callVirtualFunc<void, bool>(140,this, shouldSprint);
	}
};