#include "battle/battle.h"
#include "evt/evt_cmd.h"
#include "manager/evtmgr_cmd.h"

extern BattleWork* _battleWorkPointer;

#pragma dont_inline on
static void* get_ptr(void) {
	return (u8*)_battleWorkPointer + 0x1F4C;
}
#pragma dont_inline reset

static s32 star_stone_appear(void) {
	u8* workPtr = get_ptr();
	*(s32*)(workPtr + 0x3B4) = 1;
	return 2;
}

static s32 start_game(void) {
	u8* workPtr = get_ptr();
	*(s32*)(workPtr + 4) = 1;
	return 2;
}

static USER_FUNC(wait_game_end) {
	s32* args = event->args;
	u8* workPtr = get_ptr();
	if (*(s32*)(workPtr + 4) == 7) {
		evtSetValue(event, args[0], (s32)*(f32*)(workPtr + 0x14));
		evtSetValue(event, args[1], (s32)*(f32*)(workPtr + 0x1C));
		return 2;
	}
	return 0;
}
