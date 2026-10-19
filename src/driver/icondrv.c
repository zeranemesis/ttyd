#include "driver/icondrv.h"
#include "driver/camdrv.h"
#include "driver/dispdrv.h"
#include "driver/mapdrv.h"
#include "driver/shadowdrv.h"
#include "evt/evt_cmd.h"
#include "manager/dvdmgr.h"
#include "manager/evtmgr_cmd.h"
#include "mario/mariost.h"
#include "memory.h"
#include "system.h"
#include "sdk/texPalette.h"
#include <string.h>

#include <stdio.h>


#ifdef TARGET_PC
extern GlobalWork* gp;
#define wp gp
#else
extern GlobalWork* wp_80422810;
#define wp wp_80422810
#endif
#ifndef GSW
#define GSW(id) ((id) - 170000000)
#endif

//.sbss
TEXPalette* icon_tpl;
IconBinary* icon_bin;
BOOL icon_tpl_ok;
BOOL icon_bin_ok;
static IconWork work[2];

//.sdata2, exported (referenced by party RELs)
const f32 dat_80419778 = 18.5f;
const f32 dat_8041977c = 2.0f;
const f32 dat_80419780 = 45.0f;
const f32 dat_80419784 = 6.2831855f;
const f32 dat_80419788 = 360.0f;
const f32 dat_8041978c = 50.0f;
const f32 dat_80419790 = 0.25f;
const f32 float_10_80419794 = 0.3f;
const f32 float_8_80419798 = 12.0f;
const f32 float_0_8041979c = 120.0f;
const f32 float_20_804197a0 = 0.15f;
const f32 float_16_804197a4 = 20.0f;
const f32 float_neg1_804197a8 = 2.2f;
const f32 float_1_804197ac = -2.0f;
const f32 float_0p5_804197b0 = 0.2f;
const f32 float_0p6_804197b4 = 135.0f;
const f32 float_0p8_804197b8 = 315.0f;
const f32 float_0p9_804197bc = 225.0f;
const f32 float_deg2rad_804197c0 = 270.0f;
const f32 lbl_8042E154 = 4.0f;
const f32 float_200_804197c8 = 5.0f;

//float constants defined in driver/cam_road.c's .sdata2
extern f32 float_0p005_80419548;
extern f32 float_0_8041954c;
extern const f32 float_0p004_80419550;
extern const f32 float_0p008_80419554;
extern const f32 float_1_80419558;
extern const f32 float_2_8041955c;
extern const f32 float_3_80419560;
extern const f32 float_10_80419564;
extern const f32 float_3p1416_80419568;
extern f32 float_0p5_8041956c;
extern const f32 float_0p5_80419570;
extern const f32 float_6p2832_80419574;
extern const f32 float_3p1416_80419578;
extern const f32 float_neg3p1416_8041957c;
extern const f32 float_rad2deg_80419580;
extern const f32 float_360_80419584;
extern const f32 float_180_80419588;
extern const f32 float_0p01_8041958c;
extern const f32 float_0p3_80419590;

//GXColor constants defined in driver/cam_road.c's .sdata2 (0xFFFFFFFF = white)
extern const f32 lbl_8042C328;

//strings defined in driver/cam_road.c's .sdata2
extern const char float_90_80419594[4]; // "aji"

//.sdata digit icon tables
extern u16 lbl_804218C4[10];
extern u16 lbl_804218D8[10];

//.sdata arrays defined in driver/shadowdrv.c
extern u16 lbl_80421880[6];
extern u16 lbl_8042188C[6];
extern u8 lbl_804218A0[32];
extern u8 lbl_804218C0[4];

//.rodata constants
extern const Vec lbl_802CB398;
extern const Vec lbl_802CB3A4;
extern const Vec lbl_802CB3B0;
extern const Vec lbl_802CB3BC;
extern const f64 double_to_int_mask_802c12c0;

//.rodata format strings
extern const char vec3_802c12d0[]; // "%s/icon.tpl"
extern const char vec3_802c12dc[]; // "%s/icon.bin"

//external functions
extern void shadowEntry(f32 x, f32 y, f32 z, f32 radius);

extern const f64 double_to_int_802c12c8;         // 0x802CB3D0, used by the

// local prototypes
void _callback_tpl(s32 error, DVDFileInfo* info);
void _callback_bin(s32 error, DVDFileInfo* info);
void iconDisp(CameraId camId, void* param);
void iconGX(Mtx mtx, IconEntry* entry);
void iconNumberDisp(Mtx mtx, u16 flags, u16 iconId, GXColor* color);

#define iconGetWork() (wp->inBattle ? &work[1] : &work[0])

s32 iconNumberDispGx3D(Mtx mtx, s32 number, BOOL small, GXColor* color) {
    GXColor* colorp;
    s32* digitp;
    s32 i;
    s32 count;
    s32 pos;
    s32 digits[4];
    s32 rem;
    Mtx trans;
    GXColor colorCopy;
    f32 spacing;

    spacing = !small ? float_10_80419564 : float_3p1416_80419568;
    pos = 0;

    rem = number % 100000;
    digits[4] = rem / 10000;
    rem %= 10000;
    digits[3] = rem / 1000;
    rem %= 1000;
    digits[2] = rem / 100;
    rem %= 100;
    digits[1] = rem / 10;
    digits[0] = rem % 10;

    if (number / 100000) {
        count = 5;
    } else if (digits[4]) {
        count = 5;
    } else if (digits[3]) {
        count = 4;
    } else if (digits[2]) {
        count = 3;
    } else if (digits[1]) {
        count = 2;
    } else {
        count = 1;
    }

    digitp = digits;
    colorp = &colorCopy;
    for (i = 0; i < count; digitp++, i++) {
        PSMTXTrans(trans, (f32)(-i) * spacing, float_0p5_8041956c, float_0p5_8041956c);
        PSMTXConcat(mtx, trans, trans);
        colorCopy = *color;
        iconNumberDisp(trans, 0, !small ? lbl_804218C4[*digitp] : lbl_804218D8[*digitp], colorp);
        pos = (s32)((f32)pos + spacing);
    }
    return pos;
}
s32 iconNumberDispGx(Mtx mtx, s32 number, BOOL small, GXColor* color) {
    GXColor* colorp;
    s32* digitp;
    s32 i;
    s32 count;
    s32 pos;
    s32 digits[5];
    s32 rem;
    Mtx trans;
    GXColor colorCopy;
    f32 spacing;

    spacing = !small ? float_0p5_80419570 : float_6p2832_80419574;
    pos = 0;

    rem = number % 100000;
    digits[4] = rem / 10000;
    rem %= 10000;
    digits[3] = rem / 1000;
    rem %= 1000;
    digits[2] = rem / 100;
    rem %= 100;
    digits[1] = rem / 10;
    digits[0] = rem % 10;

    if (number / 100000) {
        count = 5;
    } else if (digits[4]) {
        count = 5;
    } else if (digits[3]) {
        count = 4;
    } else if (digits[2]) {
        count = 3;
    } else if (digits[1]) {
        count = 2;
    } else {
        count = 1;
    }

    digitp = digits;
    colorp = &colorCopy;
    for (i = 0; i < count; digitp++, i++) {
        PSMTXTrans(trans, (f32)(-i) * spacing, float_0p5_8041956c, float_0p5_8041956c);
        PSMTXConcat(mtx, trans, trans);
        colorCopy = *color;
        iconNumberDisp(trans, 0x10, !small ? lbl_804218C4[*digitp] : lbl_804218D8[*digitp], colorp);
        pos = (s32)((f32)pos + spacing);
    }
    return pos;
}

void iconSetAlpha(const char* name, u8 alpha) {
    IconEntry* entry = iconNameToPtr(name); // inline
    if (entry) {
        GXColor color = entry->color;
        color.a = alpha;
        entry->color = color;
    }
}

void iconSetScale(const char* name, f32 scale) {
    IconEntry* entry = iconNameToPtr(name); // inline
    if (entry) {
        entry->scale = scale;
    }
}

void iconFlagOff(const char* name, s16 mask) {
    IconEntry* entry = iconNameToPtr(name); // inline
    if (entry) {
        entry->flags &= ~mask;
    }
}

void iconFlagOn(const char* name, s16 flags) {
    IconEntry* entry = iconNameToPtr(name); // inline
    if (entry) {
        entry->flags |= flags;
    }
}

void iconSetPos(const char* name, f32 x, f32 y, f32 z) {
    IconWork* workptr = iconGetWork();
    IconEntry* entry = iconNameToPtr(name); // inline

    if (entry) {
        Vec temp = lbl_802CB3BC;
        temp.x = x;
        temp.y = y;
        temp.z = z;
        entry->position = temp;
    }
}

IconEntry* iconNameToPtr(const char* name) {
    IconWork* workptr = iconGetWork();
    IconEntry* entry = workptr->entries;
    int i;

    for (i = 0; i < workptr->count; i++, entry++) {
        if (entry->flags & 1 && !strcmp(entry->name, name)) {
            return entry;
        }
    }
    return NULL;
}

void iconGX(Mtx mtx, IconEntry* entry) // C-AND-ROUTINE-BUILT // icondrv.s:0x800199BC
{
    GXTexObj texObj;
    Mtx scale;
    Mtx mtx2;
    CameraEntry* camera;
    GXColor color;
    Vec v;
    f32 x0;
    f32 y0;
    f32 x1;
    f32 y1;

    v = lbl_802CB3B0;
    v.x = mtx[0][3];
    v.y = mtx[1][3];
    v.z = mtx[2][3];
    PSMTXScale(scale, (entry->flags & 0x0004) ? float_3p1416_80419578 : float_neg3p1416_8041957c,
                      (entry->flags & 0x0008) ? float_3p1416_80419578 : float_neg3p1416_8041957c,
                      float_neg3p1416_8041957c);
    PSMTXConcat(mtx, scale, mtx2);

    camera = camGetPtr(CAMERA_2D);
    if (camGetCurPtr() != camera && (entry->flags & 0x0010)) {
        PSMTXScale(scale, float_neg3p1416_8041957c, float_3p1416_80419578, float_neg3p1416_8041957c);
        PSMTXConcat(mtx2, scale, mtx2);
    }
    GXLoadPosMtxImm(mtx2, 0);
    GXSetCurrentMtx(0);

    {
        u16 tplId = entry->tplId;
        if (!icon_tpl_ok || !icon_bin_ok) {
            GXInitTexObj(&texObj, lbl_804218C0, 1, 1, 0, 0, 0, 0);
        }
        else {
            TEXGetGXTexObjFromPalette(icon_tpl, &texObj, tplId);
            GXInitTexObjLOD(&texObj, GX_LINEAR, GX_LINEAR, float_0p5_8041956c,
                            float_0p5_8041956c, float_0p5_8041956c,
                            GX_FALSE, GX_FALSE, GX_ANISO_1);
        }
    }
    GXLoadTexObj(&texObj, GX_TEXMAP0);

    if (!(entry->flags & 0x0010)) {
        mapSetMaterialLight(4, v);
    }
    else {
        GXSetNumChans(1);
        GXSetChanMatColor(GX_COLOR0A0, *(GXColor*)&float_1_80419558);
        GXSetChanCtrl(GX_COLOR0A0, GX_FALSE, GX_SRC_REG, GX_SRC_REG, GX_LIGHT_NULL,
                      GX_DF_NONE, GX_AF_NONE);
    }

    if (entry->flags & 0x0020) {
        GXSetNumTevStages(3);
        GXSetTevSwapModeTable(GX_TEV_SWAP1, GX_CH_RED, GX_CH_RED, GX_CH_RED, GX_CH_ALPHA);
        GXSetTevSwapModeTable(GX_TEV_SWAP2, GX_CH_GREEN, GX_CH_GREEN, GX_CH_GREEN, GX_CH_ALPHA);
        GXSetTevSwapModeTable(GX_TEV_SWAP3, GX_CH_BLUE, GX_CH_BLUE, GX_CH_BLUE, GX_CH_ALPHA);
        GXSetTevSwapMode(GX_TEVSTAGE0, GX_TEV_SWAP0, GX_TEV_SWAP1);
        GXSetTevSwapMode(GX_TEVSTAGE1, GX_TEV_SWAP0, GX_TEV_SWAP2);
        GXSetTevSwapMode(GX_TEVSTAGE2, GX_TEV_SWAP0, GX_TEV_SWAP3);
        GXSetTevColor(GX_TEVREG0, *(GXColor*)&float_2_8041955c);
        GXSetTevColor(GX_TEVREG1, entry->color);
        GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR_NULL);
        GXSetTevColorOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
        GXSetTevAlphaOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
        GXSetTevColorIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_C0, GX_CC_TEXC, GX_CC_ZERO);
        GXSetTevAlphaIn(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_TEXA);
        GXSetTevOrder(GX_TEVSTAGE1, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR_NULL);
        GXSetTevColorOp(GX_TEVSTAGE1, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
        GXSetTevAlphaOp(GX_TEVSTAGE1, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
        GXSetTevColorIn(GX_TEVSTAGE1, GX_CC_ZERO, GX_CC_C0, GX_CC_TEXC, GX_CC_CPREV);
        GXSetTevAlphaIn(GX_TEVSTAGE1, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_TEXA);
        GXSetTevOrder(GX_TEVSTAGE2, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR_NULL);
        GXSetTevColorOp(GX_TEVSTAGE2, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
        GXSetTevAlphaOp(GX_TEVSTAGE2, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
        GXSetTevColorIn(GX_TEVSTAGE2, GX_CC_ZERO, GX_CC_C0, GX_CC_TEXC, GX_CC_CPREV);
        GXSetTevAlphaIn(GX_TEVSTAGE2, GX_CA_ZERO, GX_CA_A1, GX_CA_TEXA, GX_CA_ZERO);
        GXSetNumTexGens(1);
        GXSetTexCoordGen2(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, GX_IDENTITY, GX_FALSE, GX_PTIDENTITY);
    }
    else {
        GXSetNumTevStages(2);
        GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR0A0);
        GXSetTevOp(GX_TEVSTAGE0, GX_MODULATE);
        GXSetTevSwapMode(GX_TEVSTAGE0, GX_TEV_SWAP0, GX_TEV_SWAP0);
        GXSetTevColor(GX_TEVREG0, entry->color);
        GXSetTevOrder(GX_TEVSTAGE1, GX_TEXCOORD_NULL, GX_TEXMAP_NULL, GX_COLOR_NULL);
        GXSetTevColorOp(GX_TEVSTAGE1, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
        GXSetTevAlphaOp(GX_TEVSTAGE1, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
        GXSetTevColorIn(GX_TEVSTAGE1, GX_CC_ZERO, GX_CC_C0, GX_CC_CPREV, GX_CC_ZERO);
        GXSetTevAlphaIn(GX_TEVSTAGE1, GX_CA_ZERO, GX_CA_A0, GX_CA_APREV, GX_CA_ZERO);
        if (!(entry->flags & 0x0010) && !(entry->flags & 0x0040) && !(entry->flags & 0x0080) &&
            (mapGetWork()->entries[0].flags & 2)) {
            GXColor blend;

            mapGetBlend(&blend);
            GXSetTevColor(GX_TEVREG1, blend);
            GXSetNumTevStages(3);
            GXSetTevOrder(GX_TEVSTAGE2, GX_TEXCOORD_NULL, GX_TEXMAP_NULL, GX_COLOR_NULL);
            GXSetTevColorOp(GX_TEVSTAGE2, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
            GXSetTevAlphaOp(GX_TEVSTAGE2, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
            GXSetTevColorIn(GX_TEVSTAGE2, GX_CC_ZERO, GX_CC_CPREV, GX_CC_C1, GX_CC_ZERO);
            GXSetTevAlphaIn(GX_TEVSTAGE2, GX_CA_ZERO, GX_CA_APREV, GX_CA_A1, GX_CA_ZERO);
        }
        GXSetNumTexGens(1);
        GXSetTexCoordGen2(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, GX_IDENTITY, GX_FALSE, GX_PTIDENTITY);
    }

    if (!(entry->flags & 0x0010)) {
        mapSetMaterialFog();
    }
    else {
        GXSetFog(GX_FOG_NONE, float_0p5_8041956c, float_0p5_8041956c,
                 float_0p5_8041956c, float_0p5_8041956c,
                 *(GXColor*)&float_3_80419560);
    }

    if (entry->color.a < 0xFF) {
        GXSetBlendMode(GX_BM_BLEND, GX_BL_SRCALPHA, GX_BL_INVSRCALPHA, GX_LO_CLEAR);
        GXSetZCompLoc(GX_TRUE);
        GXSetAlphaCompare(GX_ALWAYS, 0, GX_AOP_AND, GX_ALWAYS, 0);
        GXSetZMode(GX_TRUE, GX_LEQUAL, GX_FALSE);
    }
    else {
        GXSetBlendMode(GX_BM_NONE, GX_BL_SRCALPHA, GX_BL_INVSRCALPHA, GX_LO_CLEAR);
        GXSetZCompLoc(GX_FALSE);
        GXSetAlphaCompare(GX_GEQUAL, 0x80, GX_AOP_OR, GX_NEVER, 0);
        GXSetZMode(GX_TRUE, GX_LEQUAL, GX_TRUE);
    }

    if (entry->flags & 0x0010) {
        GXSetZMode(GX_FALSE, GX_LEQUAL, GX_FALSE);
    }

    if (entry->flags & 0x0400) {
        GXSetCullMode(GX_CULL_BACK);
    }
    else if (entry->flags & 0x0800) {
        GXSetCullMode(GX_CULL_FRONT);
    }
    else {
        GXSetCullMode(GX_CULL_NONE);
    }

    GXClearVtxDesc();
    GXSetVtxDesc(GX_VA_POS, GX_DIRECT);
    GXSetVtxDesc(GX_VA_TEX0, GX_DIRECT);
    GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_POS, GX_POS_XYZ, GX_F32, 0);
    GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_TEX0, GX_TEX_ST, GX_F32, 0);

    {
        f32 h;

        if (!entry->width && !entry->height) {
            x1 = (f32)GXGetTexObjWidth(&texObj);
            h = (f32)GXGetTexObjHeight(&texObj);
        }
        else {
            x1 = (f32)entry->width;
            h = (f32)entry->height;
        }
        x0 = -x1 * float_rad2deg_80419580;
        y0 = h;
        y1 = -h;
        if (!(entry->flags & 0x0010)) {
            x0 *= float_360_80419584;
            y0 *= float_360_80419584;
            x1 *= float_360_80419584;
            y1 *= float_360_80419584;
            if (*(u16*)&entry->iconId == 403) {
                x0 *= float_180_80419588;
                y0 *= float_180_80419588;
                x1 *= float_180_80419588;
                y1 *= float_180_80419588;
            }
            else if (*(u16*)&entry->iconId == 405) {
                x0 *= float_0p01_8041958c;
                y0 *= float_0p01_8041958c;
                x1 *= float_0p01_8041958c;
                y1 *= float_0p01_8041958c;
            }
        }

        GXBegin(GX_QUADS, GX_VTXFMT0, 4);
        GXPosition3f32(x0, y0, 0.0f);
        GXTexCoord2f32(0.0f, 0.0f);
        GXPosition3f32(x0 + x1, y0, 0.0f);
        GXTexCoord2f32(1.0f, 0.0f);
        GXPosition3f32(x0 + x1, y0 + y1, 0.0f);
        GXTexCoord2f32(1.0f, 1.0f);
        GXPosition3f32(x0, y0 + y1, 0.0f);
        GXTexCoord2f32(0.0f, 1.0f);

        if (entry->flags & 0x0020) {
            GXSetTevSwapMode(GX_TEVSTAGE0, GX_TEV_SWAP0, GX_TEV_SWAP0);
            GXSetTevSwapMode(GX_TEVSTAGE1, GX_TEV_SWAP0, GX_TEV_SWAP0);
            GXSetTevSwapMode(GX_TEVSTAGE2, GX_TEV_SWAP0, GX_TEV_SWAP0);
        }
    }
}

void iconGetWidthHeight(u16* width, u16* height, u16 iconId) {
    IconBinary* bin;
    IconBinaryEntry* data;
    TEXDescriptor* descriptor;
    u16 idx;

    bin = icon_bin;
    idx = iconId;
    if (!icon_tpl_ok || !icon_bin_ok) {
        data = NULL;
    }
    else {
        switch (iconId) {
            case 73:
                idx = lbl_80421880[wp->language];
                break;
            case 103:
                idx = lbl_8042188C[wp->language];
                break;
        }
        data = (IconBinaryEntry*)((u8*)bin + bin->offset[idx]);
    }
    *width = data->width;
    *height = data->height;
    if (!*width && !*height) {
        descriptor = TEXGet(icon_tpl, iconId);
        *width = descriptor->textureHeader->width;
        *height = descriptor->textureHeader->height;
    }
}

void iconGetTexObj(GXTexObj* texObj, u16 iconId) {
    IconBinary* bin;
    IconBinaryEntry* data;
    s32 i;
    s32 frame;
    s32 total;
    u16 tplId;
    if (!icon_tpl_ok || !icon_bin_ok) {
        GXInitTexObj(texObj, lbl_804218A0, 1, 1, GX_TF_I4, GX_CLAMP, GX_CLAMP, 0);
        return;
    }
    bin = icon_bin;
    if (!icon_tpl_ok || !icon_bin_ok) {
        data = NULL;
    } else {
        switch (iconId) {
            case 73:
                iconId = lbl_80421880[wp->language];
                break;
            case 103:
                iconId = lbl_8042188C[wp->language];
                break;
        }
        data = (IconBinaryEntry*)((u8*)bin + bin->offset[iconId]);
    }
    tplId = data->frames[0].tplId;
    if (data->numIcons > 1) {
        total = 0;
        for (i = 0; i < data->numIcons; i++) {
            total += data->frames[i].delay;
        }
        frame = (s32)wp->retraceCount % total;
        for (i = 0; i < data->numIcons; i++) {
            if (frame < data->frames[i].delay) {
                tplId = data->frames[i].tplId;
                break;
            }
            frame -= data->frames[i].delay;
        }
    }
    TEXGetGXTexObjFromPalette(icon_tpl, texObj, tplId);
    GXInitTexObjLOD(texObj, GX_LINEAR, GX_LINEAR, float_0p5_8041956c,
                    float_0p5_8041956c, float_0p5_8041956c, GX_FALSE, GX_FALSE,
                    GX_ANISO_1);
}

void iconNumberDisp(Mtx mtx, u16 flags, u16 iconId, GXColor* color) {
    CameraEntry* camera;
    IconEntry entry;
    IconEntry* entryp;
    Mtx mtx2;
    IconBinaryEntry* data;
    IconBinary* bin;
    u16 id;
    s32 i, frame, total;

    camera = camGetCurPtr();
    entryp = &entry;
    if (!icon_tpl_ok || !icon_bin_ok) {
        return;
    }
    *(u32*)&entry.color = *(u32*)color;
    entry.flags = flags;
    id = (u16)iconId;
    entry.iconId = iconId;
    bin = icon_bin;
    if (!icon_tpl_ok || !icon_bin_ok) {
        data = NULL;
    } else {
        switch (id) {
            case 73:
                id = lbl_80421880[wp->language];
                break;
            case 103:
                id = lbl_8042188C[wp->language];
                break;
        }
        data = (IconBinaryEntry*)((u8*)bin + bin->offset[id]);
    }
    entry.width = data->width;
    entry.height = data->height;
    entry.tplId = data->frames[0].tplId;
    entry.frameDelay = data->frames[0].delay;
    if (data->numIcons > 1) {
        total = 0;
        for (i = 0; i < data->numIcons; i++) {
            total += data->frames[i].delay;
        }
        frame = (s32)wp->retraceCount % total;
        for (i = 0; i < data->numIcons; i++) {
            if (frame < data->frames[i].delay) {
                entry.tplId = data->frames[i].tplId;
                break;
            }
            frame -= data->frames[i].delay;
        }
    }
    PSMTXConcat(camera->view, mtx, mtx2);
    iconGX(mtx2, entryp);
}
void iconDispGx(Vec* position, s16 flags, s32 iconId, f32 scale) {
    iconDispGxAlpha(position, flags, iconId, 0xFF, scale);
}

void iconDispGx2(Mtx mtx, s16 flags, u16 iconId) {
    CameraEntry* camera;
    IconEntry entry;
    IconEntry* entryp;
    Mtx mtx2;
    IconBinaryEntry* data;
    IconBinary* bin;
    u16 id;
    s32 i, frame, total;

    camera = camGetCurPtr();
    entryp = &entry;
    if (!icon_tpl_ok || !icon_bin_ok) {
        return;
    }
    entry.flags = flags;
    *(u32*)&entry.color = *(u32*)&float_0p008_80419554;
    id = (u16)iconId;
    entry.iconId = iconId;
    bin = icon_bin;
    if (!icon_tpl_ok || !icon_bin_ok) {
        data = NULL;
    } else {
        switch (id) {
            case 73:
                id = lbl_80421880[wp->language];
                break;
            case 103:
                id = lbl_8042188C[wp->language];
                break;
        }
        data = (IconBinaryEntry*)((u8*)bin + bin->offset[id]);
    }
    entry.width = data->width;
    entry.height = data->height;
    entry.tplId = data->frames[0].tplId;
    entry.frameDelay = data->frames[0].delay;
    if (data->numIcons > 1) {
        total = 0;
        for (i = 0; i < data->numIcons; i++) {
            total += data->frames[i].delay;
        }
        frame = (s32)wp->retraceCount % total;
        for (i = 0; i < data->numIcons; i++) {
            if (frame < data->frames[i].delay) {
                entry.tplId = data->frames[i].tplId;
                break;
            }
            frame -= data->frames[i].delay;
        }
    }
    PSMTXConcat(camera->view, mtx, mtx2);
    iconGX(mtx2, entryp);
}

void iconDispGxAlpha(Vec* position, s16 flags, s32 iconId, u8 alpha, f32 scale) {
    CameraEntry* camera;
    IconEntry entry;
    IconEntry* entryp;
    GXColor color;
    Mtx trans, scaleMtx, rot, mtx;
    IconBinaryEntry* data;
    IconBinary* bin;
    u16 id;
    s32 i, frame, total;

    camera = camGetCurPtr();
    entryp = &entry;
    if (!(icon_tpl_ok && icon_bin_ok)) {
        return;
    }
    PSMTXTrans(trans, position->x, position->y, position->z);
    PSMTXScale(scaleMtx, scale, scale, scale);
    PSMTXRotRad(rot, 'y', float_0p3_80419590 * -camera->viewYaw);
    PSMTXConcat(trans, rot, mtx);
    PSMTXConcat(mtx, scaleMtx, mtx);
    PSMTXConcat(camera->view, mtx, mtx);
    *(u32*)&color = *(u32*)&float_0p004_80419550;
    color.a = alpha;
    id = (u16)iconId;
    entry.flags = flags;
    entry.color = color;
    entry.iconId = iconId;
    bin = icon_bin;
    if (!icon_tpl_ok || !icon_bin_ok) {
        data = NULL;
    } else {
        switch (id) {
            case 73:
                id = lbl_80421880[wp->language];
                break;
            case 103:
                id = lbl_8042188C[wp->language];
                break;
        }
        data = (IconBinaryEntry*)((u8*)bin + bin->offset[id]);
    }
    entry.width = data->width;
    entry.height = data->height;
    entry.tplId = data->frames[0].tplId;
    entry.frameDelay = data->frames[0].delay;
    if (data->numIcons > 1) {
        total = 0;
        for (i = 0; i < data->numIcons; i++) {
            total += data->frames[i].delay;
        }
        frame = (s32)wp->retraceCount % total;
        for (i = 0; i < data->numIcons; i++) {
            if (frame < data->frames[i].delay) {
                entry.tplId = data->frames[i].tplId;
                break;
            }
            frame -= data->frames[i].delay;
        }
    }
    iconGX(mtx, entryp);
}

void iconDisp(CameraId camId, void* param) {
    IconEntry* entry = param; // cast to correct type
    CameraEntry* camera;
    Mtx trans, scale, rot, mtx;

    camera = camGetPtr(camId);
    PSMTXTrans(trans, entry->position.x, entry->position.y, entry->position.z);
    PSMTXScale(scale, entry->scale, entry->scale, entry->scale);
    PSMTXRotRad(rot, 'y', float_0p3_80419590 * -camera->viewYaw);
    PSMTXConcat(trans, rot, mtx);
    PSMTXConcat(mtx, scale, mtx);
    PSMTXConcat(camera->view, mtx, mtx);
    iconGX(mtx, entry);
}

void iconChange(const char* name, s16 iconId) {
    IconWork* workptr = iconGetWork();
    IconBinary* bin;
    IconBinaryEntry* data;
    IconEntry* entry;
    u16 idx;

    entry = iconNameToPtr(name); // inlined by MWCC, matches the target loop
    entry->iconId = iconId;
    bin = icon_bin;
    idx = *(u16*)&entry->iconId;
    if (!icon_tpl_ok || !icon_bin_ok) {
        data = NULL;
    }
    else {
        switch (*(u16*)&entry->iconId) {
            case 73:
                idx = lbl_80421880[wp->language];
                break;
            case 103:
                idx = lbl_8042188C[wp->language];
                break;
        }
        data = (IconBinaryEntry*)((u8*)bin + bin->offset[idx]);
    }
    entry->width = data->width;
    entry->height = data->height;
    entry->numIcons = data->numIcons;
    entry->currIcon = 0;
    entry->tplId = data->frames[0].tplId;
    entry->frameDelay = data->frames[0].delay;
    if (data->type & 1) {
        entry->flags |= 0x1000;
    }
    entry->flags |= 1;
}

void iconDelete(const char* name) {
    IconWork* workptr = iconGetWork();
    IconEntry* entry = workptr->entries;
    int i;

    for (i = 0; i < workptr->count; i++, entry++) {
        if (entry->flags & 1 && !strcmp(entry->name, name)) {
            entry->flags &= ~1; // no longer in use
            break;
        }
    }
}

void iconEntry2D(const char* name, s16 iconId) {
    IconWork* workptr = iconGetWork();
    IconBinary* bin;
    IconBinaryEntry* data;
    IconEntry* entry;
    u16 idx;
    int i;

    entry = workptr->entries;
    for (i = 0; i < workptr->count; i++, entry++) {
        if (entry->flags & 1 && !strcmp(entry->name, name)) {
            break;
        }
    }

    entry = workptr->entries;
    for (i = 0; i < workptr->count; i++, entry++) {
        if (!(entry->flags & 1)) {
            break;
        }
    }

    memset(entry, 0, sizeof(IconEntry));
    // volatile on BOTH sides pins the word-copy to the target's LLSSLS schedule:
    // lwz r5,0(r6) / lwz r0,4(r6) / stw r5,0x4(r31) / stw r0,0x8(r31) /
    // lwz r0,0x8(r6) / stw r0,0xc(r31).  A plain struct copy instead rotates the
    // r0/r5 temps and hoists the z load early (lwz r0,0x8 right after storing x).
    *(volatile Vec*)&entry->position = *(const volatile Vec*)&lbl_802CB3A4;
    entry->scale = float_neg3p1416_8041957c; // 1.0f
    *(u32*)&entry->color = *(u32*)&float_0_8041954c; // {0xFF, 0xFF, 0xFF, 0xFF}
    strcpy(entry->name, name);
    entry->iconId = iconId;
    bin = icon_bin;
    idx = *(u16*)&entry->iconId;
    if (!icon_tpl_ok || !icon_bin_ok) {
        data = NULL;
    }
    else {
        switch (*(u16*)&entry->iconId) {
            case 73:
                idx = lbl_80421880[wp->language];
                break;
            case 103:
                idx = lbl_8042188C[wp->language];
                break;
        }
        data = (IconBinaryEntry*)((u8*)bin + bin->offset[idx]);
    }
    if (data) {
        entry->width = data->width;
        entry->height = data->height;
        entry->numIcons = data->numIcons;
        entry->currIcon = 0;
        entry->tplId = data->frames[0].tplId;
        entry->frameDelay = data->frames[0].delay;
        if (data->type & 1) {
            entry->flags |= 0x1000;
        }
    }
    else {
        entry->flags |= 0x2000;
    }
    entry->flags |= 0x11;
}

void iconEntry(const char* name, s16 iconId) {
    IconWork* workptr = iconGetWork();
    IconBinary* bin;
    IconBinaryEntry* data;
    IconEntry* entry;
    u16 idx;
    int i;

    entry = workptr->entries;
    for (i = 0; i < workptr->count; i++, entry++) {
        if (entry->flags & 1 && !strcmp(entry->name, name)) {
            break;
        }
    }

    entry = workptr->entries;
    for (i = 0; i < workptr->count; i++, entry++) {
        if (!(entry->flags & 1)) {
            break;
        }
    }

    memset(entry, 0, sizeof(IconEntry));
    // see iconEntry2D: volatile both sides reproduces the target r5/r0 word-copy
    *(volatile Vec*)&entry->position = *(const volatile Vec*)&lbl_802CB398;
    entry->scale = float_neg3p1416_8041957c; // 1.0f
    *(u32*)&entry->color = *(u32*)&float_0p005_80419548; // {0xFF, 0xFF, 0xFF, 0xFF}
    strcpy(entry->name, name);
    entry->iconId = iconId;
    bin = icon_bin;
    idx = *(u16*)&entry->iconId;
    if (!icon_tpl_ok || !icon_bin_ok) {
        data = NULL;
    }
    else {
        switch (*(u16*)&entry->iconId) {
            case 73:
                idx = lbl_80421880[wp->language];
                break;
            case 103:
                idx = lbl_8042188C[wp->language];
                break;
        }
        data = (IconBinaryEntry*)((u8*)bin + bin->offset[idx]);
    }
    if (data) {
        entry->width = data->width;
        entry->height = data->height;
        entry->numIcons = data->numIcons;
        entry->currIcon = 0;
        entry->tplId = data->frames[0].tplId;
        entry->frameDelay = data->frames[0].delay;
        if (data->type & 1) {
            entry->flags |= 0x1000;
        }
    }
    else {
        entry->flags |= 0x2000;
    }
    entry->flags |= 0x1;
    if (mapGetWork()->entries[0].flags & 2) {
        // written in this (De Morgan) form so that the &= ~0x40 block stays
        // the fallthrough and the |= 0x40 block is the out-of-line branch
        // target, exactly like the target's block layout
        if (!strncmp(wp->currentMapName, float_90_80419594, 3) && evtGetValue(0, GSW(0)) == 0x178) {
            entry->flags &= ~0x40;
        }
        else {
            entry->flags |= 0x40;
        }
    }
}

void iconMain(void) {
    IconWork* workptr;
    IconBinary* bin;
    IconBinaryEntry* data;
    int i;
    IconEntry* entry;
    u16 idx;
    u8 alpha;

    workptr = iconGetWork();
    if (!icon_tpl_ok || !icon_bin_ok) {
        return;
    }
    entry = workptr->entries;
    for (i = 0; i < workptr->count; i++, entry++) {
        if (entry->flags & 1) {
            if (entry->flags & 0x2000) {
                entry->flags &= ~0x2000;
                bin = icon_bin;
                idx = *(u16*)&entry->iconId;
                if (!icon_tpl_ok || !icon_bin_ok) {
                    data = NULL;
                }
                else {
                    switch (*(u16*)&entry->iconId) {
                        case 73:
                            idx = lbl_80421880[wp->language];
                            break;
                        case 103:
                            idx = lbl_8042188C[wp->language];
                            break;
                    }
                    data = (IconBinaryEntry*)((u8*)bin + bin->offset[idx]);
                }
                entry->width = data->width;
                entry->height = data->height;
                entry->numIcons = data->numIcons;
                entry->currIcon = 0;
                entry->tplId = data->frames[0].tplId;
                entry->frameDelay = data->frames[0].delay;
                if (data->type & 1) {
                    entry->flags |= 0x1000;
                }
            }
            if (entry->flags & 0x1000) {
                if (entry->frameDelay) {
                    entry->frameDelay--;
                }
                if (!entry->frameDelay) {
                    entry->currIcon++;
                    if (entry->currIcon >= entry->numIcons) {
                        entry->currIcon = 0;
                    }
                    bin = icon_bin;
                    idx = *(u16*)&entry->iconId;
                    if (!icon_tpl_ok || !icon_bin_ok) {
                        data = NULL;
                    }
                    else {
                        switch (*(u16*)&entry->iconId) {
                            case 73:
                                idx = lbl_80421880[wp->language];
                                break;
                            case 103:
                                idx = lbl_8042188C[wp->language];
                                break;
                        }
                        data = (IconBinaryEntry*)((u8*)bin + bin->offset[idx]);
                    }
                    entry->tplId = data->frames[entry->currIcon].tplId;
                    entry->frameDelay = data->frames[entry->currIcon].delay;
                }
            }
            if (!(entry->flags & 0x2)) {
                if (entry->flags & 0x10) {
                    dispEntry(CAMERA_2D, lbl_8042C328, 1, iconDisp, entry);
                }
                else if (entry->flags & 0x100) {
                    shadowEntry(entry->position.x, entry->position.y, entry->position.z, float_10_80419564);
                    dispEntry(CAMERA_3D_EFFECTA, dispCalcZ(&entry->position), 1, iconDisp, entry);
                }
                else if (entry->flags & 0x200) {
                    shadowEntry(entry->position.x, entry->position.y, entry->position.z, float_10_80419564);
                    dispEntry(CAMERA_3D_EFFECTB, dispCalcZ(&entry->position), 1, iconDisp, entry);
                }
                else {
                    shadowEntry(entry->position.x, entry->position.y, entry->position.z, float_10_80419564);
                    // via the local (instead of comparing the loaded byte
                    // directly) MWCC keeps this if/else as
                    // `cmpwi/beq -> mode 1`, matching the target exactly
                    alpha = entry->color.a;
                    if (alpha != 0xFF) {
                        dispEntry(CAMERA_3D, dispCalcZ(&entry->position), 2, iconDisp, entry);
                    }
                    else {
                        dispEntry(CAMERA_3D, dispCalcZ(&entry->position), 1, iconDisp, entry);
                    }
                }
            }
        }
    }
}

void iconReInit(void) {
    IconWork* workptr = iconGetWork();
    memset(workptr->entries, 0, sizeof(IconEntry) * workptr->count);
}

void iconTexSetup(void) {
    ; // stubbed in retail
}

void iconInit(void) {
    char path[128];
    DVDEntry* entry;
    u32 size;
    IconWork* w;

    work[0].count = 32;
    work[0].entries = __memAlloc(HEAP_DEFAULT, sizeof(IconEntry) * work[0].count);
    memset(work[0].entries, 0, sizeof(IconEntry) * work[0].count);

    w = &work[1];
    w->count = 32;
    w->entries = __memAlloc(HEAP_DEFAULT, sizeof(IconEntry) * w->count);
    memset(w->entries, 0, sizeof(IconEntry) * w->count);
    icon_tpl = NULL;
    icon_bin = NULL;
    icon_tpl_ok = FALSE;
    icon_bin_ok = FALSE;

    sprintf(path, vec3_802c12d0, getMarioStDvdRoot());
    entry = DVDMgrOpen(path, 2, 0);
    if (entry) {
        size = OSRoundUp32B(DVDMgrGetLength(entry));
        icon_tpl = __memAlloc(HEAP_DEFAULT, size);
        entry->info.cb.userData = entry; // this file's userData is a pointer to the entry
        DVDMgrReadAsync(entry, icon_tpl, size, 0, _callback_tpl);
    }

    sprintf(path, vec3_802c12dc, getMarioStDvdRoot());
    entry = DVDMgrOpen(path, 2, 0);
    if (entry) {
        size = OSRoundUp32B(DVDMgrGetLength(entry));
        icon_bin = __memAlloc(HEAP_DEFAULT, size);
        entry->info.cb.userData = entry; // this file's userData is a pointer to the entry
        DVDMgrReadAsync(entry, icon_bin, size, 0, _callback_bin);
    }
}

void _callback_bin(s32 error, DVDFileInfo* info) {
    DVDMgrClose(info->cb.userData);
    icon_bin_ok = TRUE;
}

void _callback_tpl(s32 error, DVDFileInfo* info) {
    DVDEntry* entry = info->cb.userData;
    UnpackTexPalette(icon_tpl);
    DVDMgrClose(entry);
    icon_tpl_ok = TRUE;
}
