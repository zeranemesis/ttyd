#include "battle/battle.h"
#include "evt/evt_cmd.h"
#include "manager/evtmgr_cmd.h"

extern BattleWork* _battleWorkPointer;

typedef struct DekaWork {
    u8 pad_0[0x30];
    s32 gameState;
    u8 pad_34[0x10];
    s32 power;
    u8 pad_48[0x3A8];
    s32 starStoneState;
} DekaWork;

#pragma dont_inline on
static DekaWork* get_ptr(void) {
    return (DekaWork*)((u8*)_battleWorkPointer + 0x1F4C);
}
#pragma dont_inline reset

USER_FUNC(get_result) {
    evtSetValue(event, event->args[0], get_ptr()->power);
    return 2;
}

static USER_FUNC(star_stone_appear) {
    s32 state = 1;
    DekaWork* work = get_ptr();
    work->starStoneState = state;
    return 2;
}

static USER_FUNC(wait_game_end) {
	return get_ptr()->gameState == 5 ? 2 : 0;
}

static USER_FUNC(start_game) {
    s32 state = 1;
    DekaWork* work = get_ptr();
    work->gameState = state;
    return 2;
}

USER_FUNC(wait_star_stone_take_on) {
	return get_ptr()->starStoneState == 4 ? 2 : 0;
}

USER_FUNC(weaponGetPower_Deka) {
    return get_ptr()->power + 1;
}
