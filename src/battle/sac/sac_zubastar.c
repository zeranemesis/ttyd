#include "battle/battle.h"
#include "evt/evt_cmd.h"
#include <dolphin/gx.h>
#include <string.h>
#pragma dont_inline on
void* GetZubaStarPtr(void) { return (u8*)_battleWorkPointer + 0x1F4C; }
#pragma dont_inline reset
USER_FUNC(star_stone_appear) { u8* work=(u8*)GetZubaStarPtr(); *(s32*)(work+0x58C)=1; return EVT_RETURN_DONE; }
USER_FUNC(wait_star_stone_up) { return *(s32*)((u8*)GetZubaStarPtr()+0x58C)<4 ? EVT_RETURN_DONE : 0; }
USER_FUNC(zubastar_init) { memset(GetZubaStarPtr(),0,0x5E0); return EVT_RETURN_DONE; }
s32 weaponGetPower_ZubaStar(BattleUnit* attacker, BattleWeapon* weapon, BattleUnit* target) { u8* work=(u8*)GetZubaStarPtr(); u32 index=*(u32*)(work+0x10); return *(s32*)((u8*)weapon+index*4+0x20); }
void zubastar_capture(void) {
    u8* work=(u8*)GetZubaStarPtr();
    GXSetTexCopySrc(0xD0,0,0x190,0x1E0);
    GXSetTexCopyDst(0x190,0x1E0,GX_TF_RGB565,GX_FALSE);
    GXCopyTex(*(*(GXTexObj***)(work+0x588)),GX_FALSE);
    GXPixModeSync();
}
void zubaStarDispStar(void) {}
