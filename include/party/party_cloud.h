#pragma once

#include <dolphin/mtx.h>

typedef struct PartyCloud {
    f32 unk0; //0x0
    f32 unk4; //0x4
    f32 unk8; //0x8
    f32 unkC; //0xC
    f32 unk10; //0x10
    s32 unk14; //0x14
    f32 unk18; //0x18
    s32 unk1C; //0x1C
    s32 unk20; //0x20
    s32 unk24; //0x24
    s32 unk28; //0x28
    s32 unk2C; //0x2C
    u8 field_30[0x34 - 0x30]; // 0x30
    u32 unk34; //0x34
    s16 unk38; //0x38
    u8 field_3A[0x48 - 0x3A]; // 0x3A
} PartyCloud;

void cloudGetAt(Vec* position);
void cloudResetAt();
u8 cloudGetStatus();
f32 cloudGetBreathDir(void);
s32 cloudGetHitBreathout(s32 param_1);
void getHitBreatheout2(struct PartyEntry* party);
void cloud_exit(struct PartyEntry* party);
void cloud_use(struct PartyEntry* party);
void fn_801898E0(struct PartyEntry* party);
void fn_80189924(struct PartyEntry* party);
void unk_8018206c(struct PartyEntry* party);
void unk_801820d0(struct PartyEntry* party);
void fn_80189A58(struct PartyEntry* party);
void fn_80189A88(struct PartyEntry* party);
