#include "battle/battle.h"
#include "evt/evt_cmd.h"

#pragma dont_inline on
void* get_ptr(void) {
    return (u8*)_battleWorkPointer + 0x1F4C;
}
#pragma dont_inline reset

USER_FUNC(star_stone_appear) {
    u8* work = (u8*)get_ptr();
    *(s32*)(work + 0x510) = 1;
    *(s32*)(work + 0x518) = 1;
    return EVT_RETURN_DONE;
}
USER_FUNC(star_appear) {
    u8* work = (u8*)get_ptr();
    s32 offset = 0;
    do {
        u8* star = work + 0x510 + offset;
        *(s32*)(star + 0) = 1;
        *(s32*)(star + 8) = 0;
        offset += 0x64;
    } while (offset < 7 * 0x64);
    return EVT_RETURN_DONE;
}
extern void statusWinOpen(void);
USER_FUNC(status_on) { statusWinOpen(); return EVT_RETURN_DONE; }
USER_FUNC(start_game) { u8* work=(u8*)get_ptr(); *(s32*)(work+4)=1; return EVT_RETURN_DONE; }
s32 object_get_num(void) {
    u8* work=(u8*)get_ptr(); s32 offset=0,count=0;
    do { if (*(s32*)(work+0x358+offset)==2) count++; offset+=0x2C; } while(offset<10*0x2C);
    return count;
}
void object_entry(void) {
    u8* work=(u8*)get_ptr(); s32 offset=0;
    do { s32* state=(s32*)(work+0x358+offset); if(*state==0){*state=1;break;} offset+=0x2C; } while(offset<10*0x2C);
}
void disp_3D_alpha(void) {}
void disp_2D(void) {}
