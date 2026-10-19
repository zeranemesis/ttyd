#include "party.h"

extern void partyMoveMain(PartyEntry* party, s32 hosei);
extern f32 angleABf(f32 x1, f32 z1, f32 x2, f32 z2);
extern void psndSFXOn_3D(s32 sfxId, Vec* pos);
extern const f32 dat_804196ac;
extern const f32 float_480_804196fc;
extern const f32 float_64_804196ec;
extern s32 lbl_8042B408[2];
extern s32 lbl_8042B410[2];
extern u8 footmarkBuf[];

u32 partyGetFootmarkId(PartyEntry* party) { return (party->flags >> 2) & 1; }
s32 partyGetFootmarkWriteId(s32 slot) { return lbl_8042B410[slot]; }
s32 partyGetFootmarkReadId(s32 slot) { return lbl_8042B408[slot]; }
void* partyGetFootmarkPtr(s32 slot) { return footmarkBuf + slot * 0x500 + lbl_8042B408[slot] * 0x10; }
void partyNextReadId(s32 slot) { if (lbl_8042B408[slot] != lbl_8042B410[slot]) { lbl_8042B408[slot]++; if (lbl_8042B408[slot] >= 0x50) lbl_8042B408[slot] = 0; } }
void partyGetReadId(s32 slot, f32* x, f32* y, f32* z) { f32* p = (f32*)partyGetFootmarkPtr(slot); *x = p[1]; *y = p[2]; *z = p[3]; }
void partyGetReadId2(s32 slot, Vec* out) { f32* p = (f32*)partyGetFootmarkPtr(slot); out->x = p[1]; out->y = p[2]; out->z = p[3]; }
void partySetDir(PartyEntry* party, f32 x, f32 z) { party->forceMoveTargetX = angleABf(party->position.x, party->position.z, x, z); }
void partyMove(PartyEntry* party) { partyMoveMain(party, 1); }
void partyMoveNoHosei(PartyEntry* party) { partyMoveMain(party, 0); }
void partyClearJumpPara(PartyEntry* party) { party->flags &= 0xFFFFFF8F; *(f32*)((u8*)party + 0x114) = dat_804196ac; *(f32*)((u8*)party + 0x118) = dat_804196ac; *(f32*)((u8*)party + 0x11C) = dat_804196ac; }
void partyChgMot(PartyEntry* party, s32 motId) {
    if (party->field_36[0] == 3) {
        party->flags &= 0xFFFFFF8F;
        party->flags &= 0xFCFFFFFF;
        party->flags &= 0xFBFFFFFF;
    }
    party->field_36[0] = motId;
    party->field_8 |= 2;
}
void N_partyPlaySfxPartyLanding1(PartyEntry* party) { psndSFXOn_3D(0x928, &party->position); }
void unk_800c9398(PartyEntry* party, f32 scale) {
    f32 factor;
    f32 adjustedScale = scale * *(f32*)(*(u32*)((u8*)party + 0x160) + 0x228);
    if (party->flags & 0x80) {
        factor = float_480_804196fc;
    } else {
        factor = float_64_804196ec;
    }
    *(f32*)((u8*)party + 0x108) = adjustedScale * factor;
    *(f32*)((u8*)party + 0x104) = *(f32*)((u8*)party + 0x108);
}
#define CLEAR_FOOTMARK(entry) do { \
    (entry)[0] = 0; \
    *(u32*)((entry) + 4) = x; \
    *(u32*)((entry) + 8) = y; \
    *(u32*)((entry) + 12) = z; \
    (entry) += 0x10; \
} while (0)

void partyClearFootmark(void) {
    MarioWork* mario = marioGetPtr();
    u32 x = *(u32*)((u8*)mario + 0x8C);
    u32 y = *(u32*)((u8*)mario + 0x90);
    u32 z = *(u32*)((u8*)mario + 0x94);
    u8* entry = footmarkBuf;
    s32 i;

    for (i = 0; i < 10; i++) {
        CLEAR_FOOTMARK(entry);
        CLEAR_FOOTMARK(entry);
        CLEAR_FOOTMARK(entry);
        CLEAR_FOOTMARK(entry);
        CLEAR_FOOTMARK(entry);
        CLEAR_FOOTMARK(entry);
        CLEAR_FOOTMARK(entry);
        CLEAR_FOOTMARK(entry);
    }
    lbl_8042B410[0] = 0;
    lbl_8042B408[0] = 0;
}

void partyChgMoveMode(PartyEntry* party, u8 mode) {
    s32 oldMode = *(u8*)((u8*)party + 0x3B);
    if (oldMode != 6) {
        if ((oldMode < 6) && (oldMode < 4)) {
        }
    } else {
        party->flags &= 0xFFFF7FFF;
    }
    *(u8*)((u8*)party + 0x3B) = mode;
    party->field_8 |= 4;
}

#undef CLEAR_FOOTMARK
