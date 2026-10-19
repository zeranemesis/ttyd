#include "battle/sac/sac_bakugame.h"
#include "battle/battle.h"

extern BattleWork* _battleWorkPointer;

#pragma dont_inline on
void* GetBakuGamePtr(void) {
	return (u8*)_battleWorkPointer + 0x1F4C;
}
#pragma dont_inline reset

static s32 star_stone_appear(void) {
	s32 state = 1;
	u8* work = GetBakuGamePtr();
	*(s32*)(work + 0x204) = state;
	return 2;
}

s32 bakuGameAudienceCanThrowPos(s32 position) {
	s32 base = 0;
	s32 count = 3;
	do {
		if (position >= base + 8 && position <= base + 11) {
			return 1;
		}
		base += 20;
	} while (--count != 0);
	return 0;
}
