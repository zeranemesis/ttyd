#include "win/win_main.h"
#include "data/item_data.h"
#include "driver/camdrv.h"
#include "driver/dispdrv.h"
#include "driver/seqdrv.h"
#include "evt/evt_cmd.h"
#include "manager/evtmgr.h"
#include "mario/mario.h"
#include "memory.h"
#include "party.h"
#include "system.h"
#include <dolphin/pad.h>
#include <demo/DEMOPad.h>
#include <string.h>

extern ItemData* itemDataTable;
extern int sprintf(char* str, const char* format, ...);
#ifndef TARGET_PC
extern const f32 lbl_80430AEC;
extern const GXColor lbl_80430AE0;
#endif

//.data
EVT_BEGIN(evt_diary_read)
EVT_END()

//.sbss
extern WindowWork* lbl_8042B550;
#define wp lbl_8042B550
extern TEXPalette* lbl_8042B554;
#define winTexTpl lbl_8042B554
static s32 cnt;

// local prototypes
void winDisp(CameraId camId, void* param);
void winDispKoopa(CameraId camId, void* param);
s32 winIconInit(void);
void winIconGrayInit(void);

void winLectureKeyMask(void) {
    if (wp->flags & 0x1000) {
        if (!(wp->flags & 0x800)) {
            wp->buttonTrg = 0;
            wp->buttonRep = 0;
            wp->dirTrg = 0;
            wp->dirRep = 0;
            return;
        }
        if (wp->lectureStep < 5) {
            if (wp->lectureStep < 2) {
                if (wp->lectureStep >= 0) {
                    wp->buttonTrg = 0;
                    wp->buttonRep = 0;
                    wp->dirTrg = 0;
                    wp->dirRep = 0;
                    if (keyGetButtonTrg(0) & PAD_BUTTON_A) {
                        wp->lectureStep++;
                    }
                }
            } else {
                wp->buttonTrg = 0;
                wp->buttonRep = 0;
                wp->dirTrg &= DEMO_STICK_RIGHT;
                wp->dirRep &= DEMO_STICK_RIGHT;
                if ((s32)wp->dirTrg != 0 || (s32)wp->dirRep != 0) {
                    wp->lectureStep++;
                }
            }
        } else {
            if (wp->lectureStep < 9) {
                wp->buttonTrg &= PAD_BUTTON_A;
                wp->buttonRep &= PAD_BUTTON_A;
                wp->dirTrg = 0;
                wp->dirRep = 0;
                if ((s32)wp->buttonTrg != 0 || (s32)wp->buttonRep != 0) {
                    wp->lectureStep++;
                }
            } else if (wp->lectureStep == 9) {
                wp->buttonTrg &= 0x1000;
                wp->buttonRep &= 0x1000;
                wp->dirTrg = 0;
                wp->dirRep = 0;
                if ((s32)wp->buttonTrg != 0 || (s32)wp->buttonRep != 0) {
                    wp->lectureStep++;
                }
            }
        }
    }
}

#ifdef TARGET_PC
extern u32 evt_lecture_msg[];
#else
extern u32 lbl_80384A5C[];
#define evt_lecture_msg lbl_80384A5C
#endif
void winLectureOn(void) {
    wp->flags |= 0x1000;
    wp->flags |= 0x800;
    wp->lectureStep = 0;
    wp->lectureEvent = evtEntry(evt_lecture_msg, 0, 0);
}

BOOL winLectureCheck(void) {
    if (wp->flags & 0x1000) {
        if (evtCheckID(wp->lectureEvent->eventId)) {
            return TRUE;
        }
    }
    wp->flags &= ~0x1000;
    return FALSE;
}

void winIconGrayInit(void) {
    winIconInit();
    GXSetTevSwapModeTable(GX_TEV_SWAP1, GX_CH_RED, GX_CH_RED, GX_CH_RED, GX_CH_ALPHA);
    GXSetTevSwapModeTable(GX_TEV_SWAP2, GX_CH_GREEN, GX_CH_GREEN, GX_CH_GREEN, GX_CH_ALPHA);
    GXSetTevSwapModeTable(GX_TEV_SWAP3, GX_CH_BLUE, GX_CH_BLUE, GX_CH_BLUE, GX_CH_ALPHA);
    GXSetTevColor(GX_TEVREG2, (GXColor){ 85, 85, 85, 255 });
    GXSetNumTevStages(4);
    GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR_NULL);
    GXSetTevColorOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
    GXSetTevAlphaOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
    GXSetTevColorIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_C2, GX_CC_TEXC, GX_CC_ZERO);
    GXSetTevAlphaIn(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_TEXA);
    GXSetTevSwapMode(GX_TEVSTAGE0, GX_TEV_SWAP0, GX_TEV_SWAP1);
    GXSetTevOrder(GX_TEVSTAGE1, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR_NULL);
    GXSetTevColorOp(GX_TEVSTAGE1, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
    GXSetTevAlphaOp(GX_TEVSTAGE1, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
    GXSetTevColorIn(GX_TEVSTAGE1, GX_CC_ZERO, GX_CC_C2, GX_CC_TEXC, GX_CC_CPREV);
    GXSetTevAlphaIn(GX_TEVSTAGE1, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_APREV);
    GXSetTevSwapMode(GX_TEVSTAGE1, GX_TEV_SWAP0, GX_TEV_SWAP2);
    GXSetTevOrder(GX_TEVSTAGE2, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR_NULL);
    GXSetTevColorOp(GX_TEVSTAGE2, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
    GXSetTevAlphaOp(GX_TEVSTAGE2, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
    GXSetTevColorIn(GX_TEVSTAGE2, GX_CC_ZERO, GX_CC_C2, GX_CC_TEXC, GX_CC_CPREV);
    GXSetTevAlphaIn(GX_TEVSTAGE2, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_APREV);
    GXSetTevSwapMode(GX_TEVSTAGE2, GX_TEV_SWAP0, GX_TEV_SWAP3);
    GXSetTevOrder(GX_TEVSTAGE3, GX_TEXCOORD_NULL, GX_TEXMAP_NULL, GX_COLOR_NULL);
    GXSetTevColorOp(GX_TEVSTAGE3, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
    GXSetTevAlphaOp(GX_TEVSTAGE3, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
    GXSetTevColorIn(GX_TEVSTAGE3, GX_CC_ZERO, GX_CC_C0, GX_CC_CPREV, GX_CC_ZERO);
    GXSetTevAlphaIn(GX_TEVSTAGE3, GX_CA_ZERO, GX_CA_A0, GX_CA_APREV, GX_CA_ZERO);
    GXSetTevSwapMode(GX_TEVSTAGE3, GX_TEV_SWAP0, GX_TEV_SWAP0);
}

s32 winIconInit(void) {
    return 0;
    winTexTpl = NULL;
    GXSetCullMode(GX_CULL_NONE);
    GXSetFog(GX_FOG_NONE, 0.0f, 0.0f, 0.0f, 0.0f, (GXColor){ 0xFF, 0xFF, 0xFF, 0xFF });
    GXSetNumChans(0);
    GXSetChanCtrl(GX_COLOR0A0, GX_FALSE, GX_SRC_REG, GX_SRC_REG, 0, GX_DF_NONE, GX_AF_NONE);
    GXSetNumTevStages(1);
    GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR_NULL);
    GXSetTevColorOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
    GXSetTevAlphaOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
    GXSetTevColorIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_C0, GX_CC_TEXC, GX_CC_ZERO);
    GXSetTevAlphaIn(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_A0, GX_CA_TEXA, GX_CA_ZERO);
    GXSetTevSwapMode(GX_TEVSTAGE0, GX_TEV_SWAP0, GX_TEV_SWAP0);
    GXSetNumTexGens(1u);
    GXSetTexCoordGen(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, 0x3C);
    GXClearVtxDesc();
    GXSetVtxDesc(GX_VA_POS, GX_DIRECT);
    GXSetVtxDesc(GX_VA_TEX0, GX_DIRECT);
    GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_POS, GX_POS_XYZ, GX_F32, 0);
    GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_TEX0, GX_TEX_ST, GX_F32, 0);
}

USER_FUNC(unk_80175eb0) {
    s32 *args = event->args;
    s32 val = evtGetValue(event, args[0]);
    if (isFirstCall) {
        wp->flags &= ~0x800;
        if (wp->lectureStep < val) {
            return 0;
        }
        goto end;
    }
end:
    wp->flags |= 0x800;
    return 2;
}

USER_FUNC(unk_80175e98) {
    wp->flags &= ~0x800;
    return 2;
}

WindowWork* winGetPtr(void) {
    return wp;
}

void winInit(void) {
    wp = __memAlloc(HEAP_DEFAULT, sizeof(WindowWork));
    memset(wp, 0, sizeof(WindowWork));
}

void winReInit(void) {
    memset(wp, 0, sizeof(WindowWork));
}

BOOL winCheck(void) {
    s32 state;

    if (wp == NULL) {
        return FALSE;
    }

    if (wp->flags & 0x1000) {
        return FALSE;
    }

    state = wp->menuState;
    if (state != 0 && state < 200) {
        return TRUE;
    }
    return FALSE;
}

void winOpenEnable(void) {
    if (wp) {
        wp->flags &= ~1;
    }
}

void winOpenDisable(void) {
    if (wp) {
        wp->flags |= 1;
    }
}

void winMain(void) {
    MarioWork* mario;

    mario = marioGetPtr();
    if (!(wp->flags & 1) && seqGetSeq() == SEQ_GAME && !seqCheckSeq()) {
        wp->buttonTrg = keyGetButtonTrg(0);
        wp->buttonRep = keyGetButtonRep(0);
        wp->dirTrg = keyGetDirTrg(0);
        wp->dirRep = keyGetDirRep(0);

        if (keyGetButtonTrg(0) & PAD_BUTTON_LEFT) {
            wp->dirTrg |= DEMO_STICK_LEFT;
        }
        if (keyGetButtonTrg(0) & PAD_BUTTON_RIGHT) {
            wp->dirTrg |= DEMO_STICK_RIGHT;
        }
        if (keyGetButtonTrg(0) & PAD_BUTTON_UP) {
            wp->dirTrg |= DEMO_STICK_UP;
        }
        if (keyGetButtonTrg(0) & PAD_BUTTON_DOWN) {
            wp->dirTrg |= DEMO_STICK_DOWN;
        }

        if (keyGetButtonRep(0) & PAD_BUTTON_LEFT) {
            wp->dirRep |= DEMO_STICK_LEFT;
        }
        if (keyGetButtonRep(0) & PAD_BUTTON_RIGHT) {
            wp->dirRep |= DEMO_STICK_RIGHT;
        }
        if (keyGetButtonRep(0) & PAD_BUTTON_UP) {
            wp->dirRep |= DEMO_STICK_UP;
        }
        if (keyGetButtonRep(0) & PAD_BUTTON_DOWN) {
            wp->dirRep |= DEMO_STICK_DOWN;
        }

        winLectureKeyMask();

        switch (wp->menuState) {
            case 0:
                break;

			case 201:
                if (!evtCheckID(wp->currentEvent->eventId)) {
                    wp->menuState = 3;
                }
                break;

            case 300:
                wp->currentEvent = evtEntryType(&evt_diary_read, 0, 0, 0);
                wp->menuState += 1;
                break;

            case 301:
                if (!evtCheckID(wp->currentEvent->eventId)) {
                    wp->menuState = 3;
                }
                break;

            case 400:
                evtYuuWindow();
                wp->menuState += 1;
                break;

            case 401:
                if (evtYuuWindowEndCheck()) {
                    wp->menuState = 3;
                }
                break;
        }

        if (wp->menuState == 2) {
            s8 characterId = marioGetPtr()->characterId;
            if (characterId < 1) {
                if (characterId < 0) {
                    return;
                }
                if (!(wp->flags & 0x1000)) {
                    dispEntry(CAMERA_2D, 250.0f, 0, winDisp, NULL);
                } else {
                    dispEntry(CAMERA_2D, 500.0f, 0, winDisp, NULL);
                }
                return;
            }
            if (characterId < 3) {
                dispEntry(CAMERA_2D, 250.0f, 0, winDispKoopa, NULL);
            }
        }
    }
}

void winDisp(CameraId camId, void* param) {
}

void winDispKoopa(CameraId camId, void* param) {
}

USER_FUNC(itemUseFunc) {
    PartyEntry* party;
    MarioWork* mario;
    ItemData* item;

    item = &itemDataTable[event->lwData[10]];
    if (isFirstCall) {
        cnt = 0;
    }
    /*switch (cnt) {

    }*/
}

void winTexInit(TEXPalette* texture) {
    winTexTpl = texture;
    GXSetCullMode(GX_CULL_NONE);
#ifdef TARGET_PC
    GXSetFog(GX_FOG_NONE, 0.0f, 0.0f, 0.0f, 0.0f, (GXColor){ 0xFF, 0xFF, 0xFF, 0xFF });
#else
    GXSetFog(GX_FOG_NONE, lbl_80430AEC, lbl_80430AEC, lbl_80430AEC, lbl_80430AEC, lbl_80430AE0);
#endif
    GXSetNumChans(0);
    GXSetChanCtrl(GX_COLOR0A0, GX_FALSE, GX_SRC_REG, GX_SRC_REG, 0, GX_DF_NONE, GX_AF_NONE);
    GXSetNumTevStages(1);
    GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR_NULL);
    GXSetTevColorOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
    GXSetTevAlphaOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
    GXSetTevColorIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_C0, GX_CC_TEXC, GX_CC_ZERO);
    GXSetTevAlphaIn(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_A0, GX_CA_TEXA, GX_CA_ZERO);
    GXSetTevSwapMode(GX_TEVSTAGE0, GX_TEV_SWAP0, GX_TEV_SWAP0);
    GXSetNumTexGens(1u);
    GXSetTexCoordGen(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, 0x3C);
    GXClearVtxDesc();
    GXSetVtxDesc(GX_VA_POS, GX_DIRECT);
    GXSetVtxDesc(GX_VA_TEX0, GX_DIRECT);
    GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_POS, GX_POS_XYZ, GX_F32, 0);
    GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_TEX0, GX_TEX_ST, GX_F32, 0);
}

void winTexSet(u32 textureId, Vec translate, Vec scale, GXColor color) {
}
