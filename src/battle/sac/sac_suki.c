#include "battle/battle.h"
#include "evt/evt_cmd.h"
#include "manager/evtmgr_cmd.h"

extern BattleWork* _battleWorkPointer;

#pragma dont_inline on
void* get_ptr_80253884(void) {
	return (u8*)_battleWorkPointer + 0x1F4C;
}
#pragma dont_inline reset

void disp_3D_alpha_80253890(void) {
}

void disp_3D_80253894(void) {
}

static s32 star_stone_appear(void) {
	u8* workPtr = get_ptr_80253884();
	*(s32*)(workPtr + 0x22C) = 1;
	return 2;
}

static USER_FUNC(wait_game_end) {
	u8* workPtr = get_ptr_80253884();
	s32* args = event->args;
	if (*(s32*)workPtr == 5) {
		evtSetValue(event, args[0], *(s32*)(workPtr + 0xC) + 1);
		return 2;
	}
	return 0;
}

static s32 start_game(void) {
	u8* workPtr = get_ptr_80253884();
	*(s32*)workPtr = 1;
	return 2;
}

s32 wait_star_stone_attack_end(void) {
	return *(s32*)((u8*)get_ptr_80253884() + 0x22C) == 8 ? 2 : 0;
}

s32 wait_star_stone_up_end(void) {
	return *(s32*)((u8*)get_ptr_80253884() + 0x22C) == 4 ? 2 : 0;
}
