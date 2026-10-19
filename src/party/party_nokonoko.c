#include "dolphin/types.h"

typedef struct PartyEntry {
    s32 field_0;
    s32 field_4;
    s32 field_8;
} PartyEntry;

#define M2C_FIELD(expr, type_ptr, offset) (*(type_ptr)((s8 *)(expr) + (offset)))

extern f32 float_0p5_8041976c;
extern f32 lbl_8042E190;
extern s32 lbl_8042B418;
extern void *partyGetPtr();
extern void marioGetPartyId();
extern void partyGoodbyeInit(PartyEntry *);
extern s32 partyGoodbyeMain(PartyEntry *);
extern void partyChgRunMode(PartyEntry *, s32);

void nokotaro_hold_cancel(void *arg0) {
    M2C_FIELD(arg0, s8 *, 0x39) = 0x14;
}

void nokonoko_init(void *arg0) {
    f32 temp_f1;
    f32 temp_f2;
    M2C_FIELD(arg0, s8 *, 0x33) = 2;
    temp_f2 = float_0p5_8041976c;
    M2C_FIELD(arg0, s8 *, 0x32) = 0;
    temp_f1 = lbl_8042E190;
    M2C_FIELD(arg0, f32 *, 0xF0) = temp_f2;
    M2C_FIELD(arg0, f32 *, 0xF4) = temp_f1;
    lbl_8042B418 = -1;
}

void nokonoko_bye(PartyEntry *party) {
    if (party->field_8 & 8) {
        party->field_8 &= ~8;
        partyGoodbyeInit(party);
    }
    if (partyGoodbyeMain(party) != 0) {
        partyChgRunMode(party, 0xE);
    }
}

s32 nokonoko_holdItem(void) {
    void *party;
	s32 type;
    marioGetPartyId();
    party = partyGetPtr();
    if (party == NULL) return 0;
	type = M2C_FIELD(party, u8 *, 0x31);
	if (type != 2 || !(M2C_FIELD(party, s32 *, 0) & 0x100)) return 0;
    return M2C_FIELD(party, s32 *, 0x174) != 0;
}

s32 nokonokoGetStatus(void *arg0) {
    u8 status;
    s32 result;
    if (arg0 == NULL) return 0;
    if (M2C_FIELD(arg0, s8 *, 0x31) != 2 || !(M2C_FIELD(arg0, s32 *, 0) & 0x100)) return 0;
    status = M2C_FIELD(arg0, u8 *, 0x39);
    result = 7;
    if (status <= 2) return 1;
    if (status >= 0xA && status <= 0xD) return 2;
    if (status >= 0x14 && status <= 0x15) return 3;
    if (status >= 0x32 && status <= 0x47) return 4;
    if (status >= 0x50 && status <= 0x64) return 5;
    if (status >= 0x6E && status <= 0x72) result = 6;
    return result;
}
