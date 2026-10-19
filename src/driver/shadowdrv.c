#include "driver/shadowdrv.h"
#include <string.h>

//.bss
CharShadowWork char_shadow_work;
ProjShadowWork proj_shadow_work;
DepthShadowWork depth_shadow_work;

//.sbss
GXTexObj rampTex8;
GXTexObj rampTex16;

//.sdata
u16 ShadowSizeTbl[4] = {0x190, 0x100, 0x80, 0x40};
DepthShadowWork* lbl_80421928 = &depth_shadow_work;
ProjShadowWork* lbl_80421924 = &proj_shadow_work;
CharShadowWork* lbl_80421920 = &char_shadow_work;

ShadowConfig* shadowGetProjShadowConfig(void) {
    return &shadowConfig[lbl_80421924->unk8];
}

GXColor shadowGetProjShadowColor(void) {
    return lbl_80421924->unk100;
}

Mtx* shadowGetProjShadowProjTexMtx(void) {
    return &lbl_80421924->projTexMtx;
}

GXTexObj* shadowGetProjShadowTexObj(void) {
    if (lbl_80421924->unk0 & 1) {
        return smartTexObj(&lbl_80421924->texObj, (void**)lbl_80421924->unkFC);
    }
    return NULL;
}

s32 shadowProjShadowDrawCountUp(void) {
    return lbl_80421924->unk4++;
}

ShadowConfig* shadowGetDepthShadowConfig(void) {
    return &shadowConfig[lbl_80421928->unk8];
}

GXColor shadowGetDepthShadowColor(void) {
    GXColor color;
    color.r = ((255 - lbl_80421928->unk100.r) * lbl_80421928->unk100.a) / 255;
    color.g = ((255 - lbl_80421928->unk100.g) * lbl_80421928->unk100.a) / 255;
    color.b = ((255 - lbl_80421928->unk100.b) * lbl_80421928->unk100.a) / 255;
    color.a = 0;
    return color;
}

Mtx* shadowGetDepthShadowProjTexMtx(void) {
    return &lbl_80421928->projTexMtx;
}

Mtx* shadowGetDepthShadowDepthMtx(void) {
    return &lbl_80421928->depthMtx;
}

GXTexObj* shadowGetDepthShadowTexObj(void) {
    if (lbl_80421928->unk0 & 1) {
        return smartTexObj(&lbl_80421928->texObj, (void**)lbl_80421928->unkFC);
    }
    return NULL;
}

s32 shadowDepthShadowDrawCountUp(void) {
    return lbl_80421928->unk4++;
}

GXTexObj* shadowGetRamp16TexObj(void) {
    return &rampTex16;
}

GXTexObj* shadowGetRamp8TexObj(void) {
    return &rampTex8;
}

s32 shadowGetCharShadowProjection(void) {
    return lbl_80421920->unk10C;
}

GXColor shadowGetCharShadowColor(void) {
    return lbl_80421920->unk100;
}

Mtx* shadowGetCharShadowProjTexMtx(void) {
    return &lbl_80421920->projTexMtx;
}

GXTexObj* shadowGetCharShadowTexObj(void) {
    if (lbl_80421920->unk0 & 1) {
        return smartTexObj(&lbl_80421920->texObj, (void**)lbl_80421920->unkFC);
    }
    return NULL;
}

void shadowFlagOn(s32 id, u16 flag) {
    lbl_80421920->entries[id].flags |= flag;
}

void shadowSetCollision(s32 id, BOOL col) {
    u16* flags = &lbl_80421920->entries[id].flags;
    if (col) {
        *flags |= 1;
    } else {
        *flags &= ~1;
    }
}

void shadowSetCamId(s32 id, u16 camId) {
    ShadowEntry* entry = &lbl_80421920->entries[id];
    entry->camId = camId;
}

void shadowSetSize(s32 id, f32 size) {
    ShadowEntry* entry = &lbl_80421920->entries[id];
    entry->size = size;
}

void shadowSetVolumeDecay(s32 id, f32 x, f32 y, f32 z) {
    ShadowEntry* entry = &lbl_80421920->entries[id];
    entry->decayX = x;
    entry->decayY = y;
    entry->decayZ = z;
}

void U_shadowSetMode(s32 id, u8 mode) {
    ShadowEntry* entry = &lbl_80421920->entries[id];
    entry->mode = mode;
}

void shadowSetType(s32 id, u8 type) {
    ShadowEntry* entry = &lbl_80421920->entries[id];
    entry->type = type;
}

void shadowMain(void) {

}

void shadowInit(void) { //almost 1:1, minor regalloc
    u8* ramp8;
    u16* ramp16;
    u32 i;
    u32 bufferSize;
    CharShadowWork* wp;

    lbl_80421920->unk0 = 0;
    lbl_80421920->unk4 = 0;
    lbl_80421920->unk8 = 1;
    lbl_80421920->unk100 = (GXColor){0, 0, 0, 0x6E};
    lbl_80421920->unk10C = 1;
    lbl_80421920->unk110 = 2;
    lbl_80421920->unk114 = 20.0f;
    lbl_80421920->unk118 = 1;
    lbl_80421920->unk104 = 0;
    lbl_80421920->entries = __memAlloc(HEAP_DEFAULT, sizeof(ShadowEntry) * 64);
    memset(lbl_80421920->entries, 0, sizeof(ShadowEntry) * 64);
    wp = lbl_80421920;
    wp->unkFC = smartAlloc(GXGetTexBufferSize(ShadowSizeTbl[0], ShadowSizeTbl[0], 2, 0, 0), 0);
    lbl_80421928->unk0 = 2;
    lbl_80421928->unk4 = 0;
    lbl_80421928->unk8 = 0;
    lbl_80421928->unk100 = (GXColor){0, 0, 0, 0x40};

    bufferSize = GXGetTexBufferSize(0x10, 0x10, GX_TF_I8, GX_FALSE, 0);
    ramp8 = __memAlloc(HEAP_DEFAULT, GXGetTexBufferSize(0x10, 0x10, GX_TF_I8, GX_FALSE, 0));
    for (i = 0; i < 256; i++) {
        ramp8[(((i & 0x3) * 8) + (((i & 0xC) * 16) + (((i & 0x80) / 4) + ((i & 0x70) / 16))))] = i;
    }
    GXInitTexObj(&rampTex8, ramp8, 0x10, 0x10, GX_TF_I8, GX_CLAMP, GX_REPEAT, GX_FALSE);
    GXInitTexObjLOD(&rampTex8, GX_NEAR, GX_NEAR, 0.0f, 0.0f, 0.0f, GX_FALSE, GX_FALSE, GX_ANISO_1);
    DCFlushRange(ramp8, bufferSize);

    bufferSize = GXGetTexBufferSize(0x100, 0x100, GX_TF_IA8, GX_FALSE, 0);
    ramp16 = __memAlloc(HEAP_DEFAULT, GXGetTexBufferSize(0x100, 0x100, GX_TF_IA8, GX_FALSE, 0));
    for (i = 0; i < 0x10000; i++) {
        ramp16[(((i & 0x3) * 4) + (((i & 0xFC) * 256) + (((i & 0xFC00) / 64) + ((i & 0x300) / 256))))] = i;
    }
    GXInitTexObj(&rampTex16, ramp16, 0x100, 0x100, GX_TF_IA8, GX_CLAMP, GX_REPEAT, GX_FALSE);
    GXInitTexObjLOD(&rampTex16, GX_NEAR, GX_NEAR, 0.0f, 0.0f, 0.0f, GX_FALSE, GX_FALSE, GX_ANISO_1);
    DCFlushRange(ramp16, bufferSize);

    lbl_80421924->unk0 = 2;
    lbl_80421924->unk4 = 0;
    lbl_80421924->unk8 = 0;
    lbl_80421924->unk100 = (GXColor){0, 0, 0, 0x40};
}
