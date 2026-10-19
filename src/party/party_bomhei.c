#include "dolphin/types.h"
#include "mario/mario_party.h"
#include "party.h"

#ifndef TARGET_PC
extern const f32 lbl_8042DE60;
extern const f32 lbl_8042DFA0;
#define BOMHEI_HEIGHT lbl_8042DE60
#else
#define BOMHEI_HEIGHT 28.0f
#endif

void bomhei_init(PartyEntry* party) {
    party->field_33 = 3;
    party->field_32 = 0;
    party->height = BOMHEI_HEIGHT;
    party->field_F4 = BOMHEI_HEIGHT;
    *((u8*)party + 0x17A) = 100;
    *((u32*)party + 0x17C / 4) = -1;
    *((u32*)party + 0x180 / 4) = -1;
    *((u32*)party + 0x184 / 4) = -1;
}

s32 bomheiGetStatus(void) {
    PartyEntry* party = partyGetPtr(marioGetPartyId());
    s32 status = 2;

    if (party == NULL ||
        (party->currentMemberId == PARTY_MEMBER_BOBBERY && (party->flags & 0x100) == 0)) {
        return 0;
    }
    if (party->useMotionId == 0x3C) {
        status = 1;
    }
    return status;
}

extern s32 marioChkWallAround(void* arg, s32 mode, f32 unused1, f32 unused2, f32 value);

s32 fn_800C34F4(PartyEntry* party, void* arg, f32 unused1, f32 unused2, f32 value) {
    if (party->flags & 0x1000) {
        value = lbl_8042DFA0;
    }
    return marioChkWallAround(arg, 1, unused1, unused2, value);
}
