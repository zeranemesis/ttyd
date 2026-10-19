#include "evt/evt_msg.h"
#include "evt/evt_cmd.h"
#include "driver/msgdrv.h"
#include "driver/camdrv.h"
#include "driver/dispdrv.h"
#include "manager/evtmgr.h"
#include "manager/evtmgr_cmd.h"
#include "demo/DEMOPad.h"
#include "system.h"
#include "font8x8.h"
#include <dolphin/gx.h>
#include <dolphin/mtx.h>
#include <stdio.h>
#include <string.h>

static char s_dispSpeaker[64] = "";
static char s_dispMsg[1024] = "";
static BOOL s_msgActive = FALSE;
static GXTexObj s_fontTexObj;
static u8 s_fontTexData[128 * 128];
static BOOL s_fontTexReady = FALSE;

static const GXColor c_shadowColor = { 0, 0, 0, 0x80 };
static const GXColor c_borderColor = { 0x24, 0x18, 0x10, 0xFF };
static const GXColor c_bgColor     = { 0xFD, 0xFA, 0xEE, 0xF8 };
static const GXColor c_innerColor  = { 0xF0, 0xEB, 0xD4, 0x80 };
static const GXColor c_bannerRed   = { 0xD0, 0x28, 0x28, 0xFF };
static const GXColor c_msgWhite    = { 0xFF, 0xFF, 0xFF, 0xFF };
static const GXColor c_textDark    = { 0x1A, 0x12, 0x0C, 0xFF };
static const GXColor c_promptBlue  = { 0x20, 0x70, 0xD0, 0xFF };

void evt_msg_init(void) {
    // Retail init
}

USER_FUNC(evt_msg_pri) {
    event->msgPriority = evtGetValue(event, event->args[0]);
    return EVT_RETURN_DONE;
}

static void initFontTex(void) {
    int c, tileX, tileY, r, bit, px, py, bx, by, blockIdx, offset;
    u8 rowBits;

    if (s_fontTexReady) return;
    memset(s_fontTexData, 0, sizeof(s_fontTexData));
    for (c = 0; c < 128; c++) {
        tileX = (c % 16) * 8;
        tileY = (c / 16) * 8;
        for (r = 0; r < 8; r++) {
            rowBits = s_font8x8[c][r];
            for (bit = 0; bit < 8; bit++) {
                if ((rowBits >> (7 - bit)) & 1) {
                    px = tileX + bit;
                    py = tileY + r;
                    // GameCube GX_TF_I8: 8x4 pixel tiles (32 bytes per tile)
                    bx = px / 8;
                    by = py / 4;
                    blockIdx = by * 16 + bx;
                    offset = blockIdx * 32 + (py % 4) * 8 + (px % 8);
                    s_fontTexData[offset] = 0xFF;
                }
            }
        }
    }
    GXInitTexObj(&s_fontTexObj, s_fontTexData, 128, 128, GX_TF_I8, GX_CLAMP, GX_CLAMP, GX_FALSE);
    GXInitTexObjLOD(&s_fontTexObj, GX_NEAR, GX_NEAR, 0.0f, 0.0f, 0.0f, GX_FALSE, GX_FALSE, GX_ANISO_1);
    s_fontTexReady = TRUE;
}

static void drawSolidQuad(f32 x1, f32 y1, f32 x2, f32 y2, GXColor color) {
    GXSetNumChans(0);
    GXSetNumTexGens(0);
    GXSetNumTevStages(1);
    GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD_NULL, GX_TEXMAP_NULL, GX_COLOR_NULL);
    GXSetTevColorOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
    GXSetTevAlphaOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
    GXSetTevColorIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_ZERO, GX_CC_ZERO, GX_CC_C0);
    GXSetTevAlphaIn(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_A0);
    GXSetTevColor(GX_TEVREG0, color);

    GXClearVtxDesc();
    GXSetVtxDesc(GX_VA_POS, GX_DIRECT);
    GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_POS, GX_POS_XYZ, GX_F32, 0);

    GXBegin(GX_QUADS, GX_VTXFMT0, 4);
    GXPosition3f32(x1, y1, 0.0f);
    GXPosition3f32(x2, y1, 0.0f);
    GXPosition3f32(x2, y2, 0.0f);
    GXPosition3f32(x1, y2, 0.0f);
    GXEnd();
}

static void drawText(f32 x, f32 y, f32 scale, GXColor color, const char* str) {
    f32 curX, curY, charW, charH;
    u8 b, b2;
    char ch;
    int c, tileX, tileY;
    f32 u0, u1, v0, v1;

    initFontTex();

    GXSetNumChans(0);
    GXSetNumTexGens(1);
    GXSetNumTevStages(1);
    GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR_NULL);
    GXSetTevColorOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
    GXSetTevAlphaOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
    GXSetTevColorIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_ZERO, GX_CC_ZERO, GX_CC_C0);
    GXSetTevAlphaIn(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_A0, GX_CA_TEXA, GX_CA_ZERO);
    GXSetTevColor(GX_TEVREG0, color);

    GXLoadTexObj(&s_fontTexObj, GX_TEXMAP0);
    GXSetTexCoordGen2(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, GX_IDENTITY, GX_FALSE, GX_PTIDENTITY);

    GXClearVtxDesc();
    GXSetVtxDesc(GX_VA_POS, GX_DIRECT);
    GXSetVtxDesc(GX_VA_TEX0, GX_DIRECT);
    GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_POS, GX_POS_XYZ, GX_F32, 0);
    GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_TEX0, GX_TEX_ST, GX_F32, 0);

    curX = x;
    curY = y;
    charW = 8.0f * scale;
    charH = 8.0f * scale;

    while (*str) {
        b = (u8)*str++;
        ch = (char)b;

        // UTF-8 multi-byte decoding
        if (b == 0xC3 && *str) {
            b2 = (u8)*str++;
            if (b2 >= 0xA8 && b2 <= 0xAB) ch = 'e';
            else if (b2 >= 0x88 && b2 <= 0x8B) ch = 'E';
            else if (b2 >= 0xA0 && b2 <= 0xA5) ch = 'a';
            else if (b2 == 0xA7) ch = 'c';
            else if (b2 == 0x87) ch = 'C';
            else if (b2 >= 0xB9 && b2 <= 0xBC) ch = 'u';
            else if (b2 >= 0xAC && b2 <= 0xAF) ch = 'i';
            else if (b2 >= 0xB2 && b2 <= 0xB6) ch = 'o';
            else ch = ' ';
        } else if (b >= 128) {
            // ISO-8859-1 single-byte decoding
            if (b == 0xE9 || b == 0xE8 || b == 0xEA || b == 0xEB) ch = 'e';
            else if (b == 0xC9 || b == 0xC8 || b == 0xCA || b == 0xCB) ch = 'E';
            else if (b == 0xE0 || b == 0xE2 || b == 0xE4) ch = 'a';
            else if (b == 0xE7) ch = 'c';
            else if (b == 0xC7) ch = 'C';
            else if (b == 0xF9 || b == 0xFB || b == 0xFC) ch = 'u';
            else if (b == 0xEE || b == 0xEF) ch = 'i';
            else if (b == 0xF4 || b == 0xF6) ch = 'o';
            else ch = ' ';
        }

        if (ch == '\n') {
            curX = x;
            curY -= (charH + 6.0f);
            continue;
        }

        // Automatic line wrapping inside dialogue box
        if (curX > 275.0f && ch == ' ') {
            curX = x;
            curY -= (charH + 6.0f);
            continue;
        }

        if ((u8)ch < 32 || (u8)ch >= 128) {
            ch = ' ';
        }

        c = (u8)ch;
        tileX = (c % 16) * 8;
        tileY = (c / 16) * 8;
        u0 = (f32)tileX / 128.0f;
        u1 = (f32)(tileX + 8) / 128.0f;
        v0 = (f32)tileY / 128.0f;
        v1 = (f32)(tileY + 8) / 128.0f;

        GXBegin(GX_QUADS, GX_VTXFMT0, 4);
        GXPosition3f32(curX, curY + charH, 0.0f);
        GXTexCoord2f32(u0, v0);

        GXPosition3f32(curX + charW, curY + charH, 0.0f);
        GXTexCoord2f32(u1, v0);

        GXPosition3f32(curX + charW, curY, 0.0f);
        GXTexCoord2f32(u1, v1);

        GXPosition3f32(curX, curY, 0.0f);
        GXTexCoord2f32(u0, v1);
        GXEnd();

        curX += charW + 1.0f;
    }
}

void pcDrawSolidQuad(f32 x1, f32 y1, f32 x2, f32 y2, GXColor color) {
    drawSolidQuad(x1, y1, x2, y2, color);
}

void pcDrawText(f32 x, f32 y, f32 scale, GXColor color, const char* str) {
    drawText(x, y, scale, color, str);
}

static void msgBoxDisp(CameraId camId, void* param) {
    CameraEntry* cam;
    static int s_blink = 0;

    if (!s_msgActive) return;

    cam = camGetPtr(CAMERA_2D);

    GXSetAlphaUpdate(GX_TRUE);
    GXSetCullMode(GX_CULL_NONE);
    GXSetZCompLoc(GX_FALSE);
    GXSetAlphaCompare(GX_ALWAYS, 0, GX_AOP_AND, GX_ALWAYS, 0);
    GXSetBlendMode(GX_BM_BLEND, GX_BL_SRCALPHA, GX_BL_INVSRCALPHA, GX_LO_CLEAR);
    GXSetZMode(GX_FALSE, GX_ALWAYS, GX_FALSE);

    GXLoadPosMtxImm(cam->view, GX_PNMTX0);
    GXSetCurrentMtx(GX_PNMTX0);

    // 1. Drop shadow of dialogue box
    drawSolidQuad(-256.0f + 5.0f, -115.0f - 5.0f, 256.0f + 5.0f, -225.0f - 5.0f, c_shadowColor);

    // 2. Dark brown/charcoal border
    drawSolidQuad(-256.0f, -115.0f, 256.0f, -225.0f, c_borderColor);

    // 3. Creamy paper background
    drawSolidQuad(-252.0f, -119.0f, 252.0f, -221.0f, c_bgColor);

    // 4. Subtle inner line border
    drawSolidQuad(-248.0f, -123.0f, 248.0f, -217.0f, c_innerColor);

    // 5. Speaker banner
    if (s_dispSpeaker[0]) {
        drawSolidQuad(-245.0f + 3.0f, -95.0f - 3.0f, -110.0f + 3.0f, -118.0f - 3.0f, c_shadowColor);
        drawSolidQuad(-245.0f, -95.0f, -110.0f, -118.0f, c_borderColor);
        drawSolidQuad(-242.0f, -98.0f, -113.0f, -115.0f, c_bannerRed);
        drawText(-235.0f, -112.0f, 1.4f, c_msgWhite, s_dispSpeaker);
    }

    // 6. Dialogue text (dark charcoal text)
    drawText(-235.0f, -145.0f, 1.5f, c_textDark, s_dispMsg);

    // 7. Flashing button prompt [ A ]
    if ((s_blink++ % 40) < 25) {
        drawText(200.0f, -212.0f, 1.3f, c_promptBlue, "[ A ]");
    }
}

static void clean_msg_tags(char* dst, const char* src, size_t maxLen) {
    size_t d = 0;
    int inTag = 0;
    while (*src && d + 1 < maxLen) {
        if (*src == '<') {
            inTag = 1;
        } else if (*src == '>') {
            inTag = 0;
        } else if (!inTag) {
            dst[d++] = *src;
        }
        src++;
    }
    dst[d] = '\0';
}

USER_FUNC(evt_msg_print) {
    s32* args = event->args;
    s32 style = evtGetValue(event, args[0]);
    const char* tag = evtGetString(event, args[1]);
    s32 pos = evtGetValue(event, args[2]);
    const char* speaker = evtGetString(event, args[3]);
    const char* rawMsg;
    char cleanMsg[1024];
    const char* dispSpeaker;
    s32 frames;
    u32 trg;

    if (isFirstCall) {
        rawMsg = tag ? msgSearch(tag) : "";
        clean_msg_tags(cleanMsg, rawMsg ? rawMsg : tag, sizeof(cleanMsg));

        dispSpeaker = speaker ? speaker : "Narration";
        if (speaker && !strcmp(speaker, "L_5")) dispSpeaker = "Luigi";
        if (speaker && !strcmp(speaker, "L_8")) dispSpeaker = "Luigi";
        if (speaker && !strcmp(speaker, "L_9")) dispSpeaker = "Luigi";
        if (speaker && !strcmp(speaker, "L_11")) dispSpeaker = "Luigi";
        if (speaker && !strcmp(speaker, "L_6")) dispSpeaker = "Luigi";
        if (speaker && !strcmp(speaker, "L_7")) dispSpeaker = "Luigi";
        if (speaker && !strcmp(speaker, "M_L_3")) dispSpeaker = "Luigi";
        if (speaker && !strcmp(speaker, "M_S_5")) dispSpeaker = "Parakarry";

        strncpy(s_dispSpeaker, dispSpeaker, sizeof(s_dispSpeaker) - 1);
        s_dispSpeaker[sizeof(s_dispSpeaker) - 1] = '\0';

        strncpy(s_dispMsg, cleanMsg, sizeof(s_dispMsg) - 1);
        s_dispMsg[sizeof(s_dispMsg) - 1] = '\0';

        s_msgActive = TRUE;

#ifdef TARGET_PC
        printf("\n======================================================\n");
        printf("  [DIALOGUE - %s] %s :\n  %s\n", tag ? tag : "???", dispSpeaker, cleanMsg);
        printf("======================================================\n\n");
        fflush(stdout);
#endif

        event->lwData[0] = 0; // frame timer
        dispEntry(CAMERA_2D, 1.0f, 0, msgBoxDisp, NULL);
        return EVT_RETURN_BLOCK;
    }

    event->lwData[0]++;
    frames = event->lwData[0];

    // Keep dialogue visible every frame
    dispEntry(CAMERA_2D, 1.0f, 0, msgBoxDisp, NULL);

    // Check button A trigger to advance dialog
    trg = DemoPad[0].buttonDown;

    // After at least 30 frames (~0.5s), allow pressing A to advance
    if (frames > 30 && (trg & PAD_BUTTON_A)) {
        s_msgActive = FALSE;
        return EVT_RETURN_DONE;
    }

    // Auto-advance after 180 frames (~3.0 seconds) so story cutscenes never get stuck
    if (frames >= 180) {
        s_msgActive = FALSE;
        return EVT_RETURN_DONE;
    }

    return EVT_RETURN_BLOCK;
}

USER_FUNC(evt_msg_print_add) {
    return evt_msg_print(event, isFirstCall);
}

USER_FUNC(evt_msg_print_insert) {
    return evt_msg_print(event, isFirstCall);
}

USER_FUNC(evt_msg_close) {
    extern void fn_800826E0(s32);
    fn_800826E0(evtGetValue(event, event->args[0]));
    return EVT_RETURN_DONE;
}

extern void msgWindow_Repeat(s32 id);
extern void msgWindow_Continue(s32 id);

USER_FUNC(evt_msg_repeat) {
    if (isFirstCall) {
        msgWindow_Repeat(event->printWindowId);
    }
    if (windowCheckID(event->printWindowId)) {
        return EVT_RETURN_BLOCK;
    }
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_msg_continue) {
    if (isFirstCall) {
        msgWindow_Continue(event->printWindowId);
    }
    if (windowCheckID(event->printWindowId)) {
        return EVT_RETURN_BLOCK;
    }
    return EVT_RETURN_DONE;
}
