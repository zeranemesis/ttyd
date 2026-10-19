#pragma once

#include <dolphin/gx.h>
#include <dolphin/mtx.h>
#include "memory.h"

typedef struct ShadowConfig {
    u8 field_0x0[0x48];
} ShadowConfig;

extern ShadowConfig shadowConfig[];

typedef struct ShadowEntry {
    u16 flags;              // 0x0
    u8 pad2[1];             // 0x2
    u8 type;                // 0x3
    u8 pad4[0x10 - 0x4];    // 0x4
    f32 size;               // 0x10
    u8 pad14[0x1A - 0x14];  // 0x14
    u16 camId;              // 0x1A
    f32 decayX;             // 0x1C
    f32 decayY;             // 0x20
    f32 decayZ;             // 0x24
    u8 mode;                // 0x28
    u8 pad29[0x5C - 0x29];  // 0x29
} ShadowEntry;

typedef struct DepthShadowWork {
    s32 unk0;               // 0x0
    s32 unk4;               // 0x4
    s32 unk8;               // 0x8
    u8 unkC[0x7C - 0xC];    // 0xC
    Mtx projTexMtx;         // 0x7C
    Mtx depthMtx;           // 0xAC
    GXTexObj texObj;        // 0xDC
    smartEntry* unkFC;      // 0xFC
    GXColor unk100;         // 0x100
} DepthShadowWork;

typedef struct ProjShadowWork {
    s32 unk0;               // 0x0
    s32 unk4;               // 0x4
    s32 unk8;               // 0x8
    u8 unkC[0x7C - 0xC];    // 0xC
    Mtx projTexMtx;         // 0x7C
    u8 unkAC[0xDC - 0xAC];  // 0xAC
    GXTexObj texObj;        // 0xDC
    smartEntry* unkFC;      // 0xFC
    GXColor unk100;         // 0x100
} ProjShadowWork;

typedef struct CharShadowWork {
    s32 unk0;               // 0x0
    s32 unk4;               // 0x4
    s32 unk8;               // 0x8
    u8 field_0xC[0x7C - 0xC];   // 0xC
    Mtx projTexMtx;         // 0x7C
    u8 field_0xAC[0xDC - 0xAC]; // 0xAC
    GXTexObj texObj;        // 0xDC
    smartEntry* unkFC;      // 0xFC
    GXColor unk100;         // 0x100
    s32 unk104;             // 0x104
    ShadowEntry* entries;   // 0x108
    s32 unk10C;             // 0x10C
    s32 unk110;             // 0x110
    f32 unk114;             // 0x114
    s32 unk118;             // 0x118
} CharShadowWork;

void shadowInit(void);
void shadowMain(void);

void shadowSetType(s32 id, u8 type);
void U_shadowSetMode(s32 id, u8 mode);
void shadowSetVolumeDecay(s32 id, f32 x, f32 y, f32 z);
void shadowSetSize(s32 id, f32 size);
void shadowSetCamId(s32 id, u16 camId);
void shadowSetCollision(s32 id, BOOL col);
void shadowFlagOn(s32 id, u16 flag);
GXTexObj* shadowGetCharShadowTexObj(void);
Mtx* shadowGetCharShadowProjTexMtx(void);
GXColor shadowGetCharShadowColor(void);
s32 shadowGetCharShadowProjection(void);
GXTexObj* shadowGetRamp8TexObj(void);
GXTexObj* shadowGetRamp16TexObj(void);
s32 shadowDepthShadowDrawCountUp(void);
GXTexObj* shadowGetDepthShadowTexObj(void);
Mtx* shadowGetDepthShadowDepthMtx(void);
Mtx* shadowGetDepthShadowProjTexMtx(void);
GXColor shadowGetDepthShadowColor(void);
ShadowConfig* shadowGetDepthShadowConfig(void);
s32 shadowProjShadowDrawCountUp(void);
GXTexObj* shadowGetProjShadowTexObj(void);
Mtx* shadowGetProjShadowProjTexMtx(void);
GXColor shadowGetProjShadowColor(void);
ShadowConfig* shadowGetProjShadowConfig(void);