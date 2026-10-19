#include "battle/battle.h"

#pragma dont_inline on
void* GetScissorPtr(void) {
    return (u8*)_battleWorkPointer + 0x1F4C;
}
#pragma dont_inline reset

s32 star_stone_appear(void) {
    u8* scissor = (u8*)GetScissorPtr();
    *(s32*)(scissor + 0x11C) = 1;
    return 2;
}

s32 wait_star_standby(void) {
    u8* scissor = (u8*)GetScissorPtr();
    return *(s32*)(scissor + 0x11C) < 6 ? 2 : 0;
}
