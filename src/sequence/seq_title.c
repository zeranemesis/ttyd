#include "seq/seq_title.h"
#include "driver/animdrv.h"
#include "driver/camdrv.h"
#include "driver/dispdrv.h"
#include "driver/fadedrv.h"
#include "manager/fontmgr.h"
#include "mario/mario_pouch.h"
#include "mario/mariost.h"
#include "pmario_sound.h"
#include "sound.h"
#include "system.h"
#include "win/win_main.h"
#include <dolphin/pad.h>
#include <math.h>
#include <musyx/musyx.h>
#include <string.h>

extern SoundWork sound;
extern const f32 float_90_804193cc;
extern const f32 float_270_804193d0;
extern const f32 lbl_8042C01C;
extern PaperSoundBGM psbgm[2];
#ifdef TARGET_PC
extern GlobalWork* gp;
#else
extern GlobalWork* wp_80422810;
#define gp wp_80422810
#endif

TitleWork work2;
#ifndef TARGET_PC
#define wp2 lbl_804217E0
#endif
TitleWork* wp2 = &work2;

#ifndef PAD_BUTTON_Z
#define PAD_BUTTON_Z PAD_TRIGGER_Z
#endif

#ifdef TARGET_PC
static const GXColor c_whiteColor = { 0xFF, 0xFF, 0xFF, 0xFF };
#else
extern const GXColor lbl_80436028;
#define c_whiteColor lbl_80436028
#endif
static const GXColor c_blackColor = { 0x00, 0x00, 0x00, 0xFF };
static const GXColor c_fontColor = { 0xFA, 0xFA, 0xFA, 0xFF };


static const char* build_date = "Sep 26 2004";
static const char* build_time = "15:37:03";

//local prototypes
static void fn_80008A54(u8 alpha, f32 x, f32 y);
void fn_80008EFC(CameraId camId, void* param);
void disp1(CameraId camId, void* param);
void L_titleInit(SequenceWork* work);
void fn_800093F0(SequenceWork* work);

const char* DbgBtlSel_GetMsgDataPtr(void) {
	return NULL;
}

void seq_titleInit(SequenceWork* work) {
	L_titleInit(work);
	gp->flags &= ~2;
	gp->flags &= ~0x40;
	gp->flags &= ~0x20;
	gp->flags &= ~0x1000;
}

void seq_titleExit(SequenceWork* work) {
	fileFree(wp2->texture);
	if (seqGetNextSeq() != SEQ_LOAD) {
		psndStopAllFadeOut();
	}
}

void seq_titleMain(SequenceWork* work) {
	fn_800093F0(work);
}

s32 N_getDebugMode(void) {
	return wp2->debug;
}

/* Candidate exported accessor, mirroring the existing debug getter.
 * Confirm code generation against the retail G8MP01 function.
 */
s32 getDebugMode(void) {
    return N_getDebugMode();
}

static void fn_80008A54(u8 alpha, f32 x, f32 y) {
	CameraEntry* cam;
	GXTexObj texObj0, texObj1;
	Mtx transMtx, scaleMtx;
	GXColor color;
	f32 h, w;

	cam = camGetCurPtr();
	TEXGetGXTexObjFromPalette(*((TEXPalette**)wp2->texture->data), &texObj0, gp->language + 13);
	GXLoadTexObj(&texObj0, GX_TEXMAP0);
	TEXGetGXTexObjFromPalette(*((TEXPalette**)wp2->texture->data), &texObj1, 6);
	GXLoadTexObj(&texObj1, GX_TEXMAP1);

	GXSetNumTevStages(2);
	GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR_NULL);
	GXSetTevColorOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
	GXSetTevAlphaOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
	GXSetTevColorIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_ZERO, GX_CC_ZERO, GX_CC_TEXC);
	GXSetTevAlphaIn(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_TEXA);

	GXSetTevOrder(GX_TEVSTAGE1, GX_TEXCOORD1, GX_TEXMAP1, GX_COLOR_NULL);
	GXSetTevColorOp(GX_TEVSTAGE1, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
	GXSetTevAlphaOp(GX_TEVSTAGE1, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
	GXSetTevColorIn(GX_TEVSTAGE1, GX_CC_ZERO, GX_CC_CPREV, GX_CC_TEXC, GX_CC_ZERO);
	GXSetTevAlphaIn(GX_TEVSTAGE1, GX_CA_ZERO, GX_CA_APREV, GX_CA_A0, GX_CA_ZERO);

	color = c_whiteColor;
	color.a = alpha;
	GXSetTevColor(GX_TEVREG0, color);
	GXSetNumChans(0);
	GXSetNumTexGens(2);
	GXSetTexCoordGen2(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, GX_IDENTITY, GX_FALSE, GX_PTIDENTITY);
	GXSetTexCoordGen2(GX_TEXCOORD1, GX_TG_MTX2x4, GX_TG_TEX0, GX_TEXMTX0, GX_FALSE, GX_PTIDENTITY);

	MTXScale(scaleMtx, float_90_804193cc, float_90_804193cc, float_90_804193cc);
	MTXTrans(transMtx, (f32)wp2->field_0x2C / float_270_804193d0, lbl_8042C01C, lbl_8042C01C);
	MTXConcat(scaleMtx, transMtx, transMtx);
	GXLoadTexMtxImm(transMtx, GX_TEXMTX0, GX_MTX2x4);

	GXSetBlendMode(GX_BM_BLEND, GX_BL_SRCALPHA, GX_BL_INVSRCALPHA, GX_LO_CLEAR);
	GXSetZCompLoc(GX_TRUE);
	GXSetAlphaCompare(GX_ALWAYS, 0, GX_AOP_AND, GX_ALWAYS, 0);
	GXSetZMode(GX_FALSE, GX_LEQUAL, GX_FALSE);
	GXSetCullMode(GX_CULL_NONE);
	GXClearVtxDesc();
	GXSetVtxDesc(GX_VA_POS, GX_DIRECT);
	GXSetVtxDesc(GX_VA_TEX0, GX_DIRECT);
	GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_POS, GX_POS_XYZ, GX_F32, 0);
	GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_TEX0, GX_TEX_ST, GX_F32, 0);

	MTXTrans(transMtx, x, y, 0.0f);
	h = (f32)GXGetTexObjHeight(&texObj0);
	w = (f32)GXGetTexObjWidth(&texObj0);
	MTXScale(scaleMtx, w, h, float_90_804193cc);
	MTXConcat(transMtx, scaleMtx, transMtx);
	MTXConcat(cam->view, transMtx, transMtx);
	GXLoadPosMtxImm(transMtx, GX_PNMTX0);
	GXSetCurrentMtx(GX_PNMTX0);

	GXBegin(GX_QUADS, GX_VTXFMT0, 4);
	GXPosition3f32(-0.5f, -0.5f, 0.0f);
	GXTexCoord2f32(0.0f, 1.0f);
	GXPosition3f32(0.5f, -0.5f, 0.0f);
	GXTexCoord2f32(1.0f, 1.0f);
	GXPosition3f32(0.5f, 0.5f, 0.0f);
	GXTexCoord2f32(1.0f, 0.0f);
	GXPosition3f32(-0.5f, 0.5f, 0.0f);
	GXTexCoord2f32(0.0f, 0.0f);
	GXEnd();

	GXSetTevSwapModeTable(GX_TEV_SWAP1, GX_CH_RED, GX_CH_GREEN, GX_CH_BLUE, GX_CH_ALPHA);
}

void fn_80008EFC(CameraId camId, void* param) {
	CameraEntry* cam2d;
	CameraEntry savedCam;
	f32 savedProj[7];
	f32 savedVp[6];
	s32 alpha;
	u32 logoTexIds[6] = { 0x13, 0x13, 0x14, 0x15, 0x16, 0x17 };

	cam2d = camGetPtr(CAMERA_2D);
	savedCam = *camGetPtr(camId);
	*camGetPtr(camId) = *cam2d;
	GXGetProjectionv(savedProj);
	GXGetViewportv(savedVp);
	GXSetProjection(cam2d->projection, cam2d->projType);
	GXSetViewport(cam2d->viewport[0], cam2d->viewport[1], cam2d->viewport[2], cam2d->viewport[3], cam2d->viewport[4], cam2d->viewport[5]);

	winTexInit(*((TEXPalette**)wp2->texture->data));
	winTexSet_a(
		logoTexIds[gp->language],
		(Vec){ wp2->field_0xC, wp2->field_0x10 - 20.0f, 0.0f },
		(Vec){ 1.0f, 1.0f, 1.0f },
		c_whiteColor
	);
	winTexSet(
		gp->language + 7,
		(Vec){ wp2->field_0x14, wp2->field_0x18, 0.0f },
		(Vec){ 1.0f, 1.0f, 1.0f },
		c_whiteColor
	);

	wp2->field_0x2C++;
	alpha = (s32)(wp2->field_0x1C * (459.0f * (f32)sin((6.2831855f * (f32)wp2->field_0x2C) / 120.0f)));
	if (alpha < 0) alpha = 0;
	if (alpha > 255) alpha = 255;
	fn_80008A54((u8)alpha, 0.0f, -122.0f);

	if (wp2->debug < 0 && (keyGetButton(0) & PAD_BUTTON_Z)) {
		FontDrawStart();
		FontDrawEdge();
		FontDrawColor(c_fontColor);
		FontDrawScale(0.8f);
		FontDrawString(60.0f, 220.0f, build_date);
		FontDrawString(200.0f, 220.0f, build_time);
	}

	*camGetPtr(camId) = savedCam;
	GXSetProjectionv(savedProj);
	GXSetViewport(savedVp[0], savedVp[1], savedVp[2], savedVp[3], savedVp[4], savedVp[5]);
}

void disp1(CameraId camId, void* param) {
	CameraEntry* cam2d;
	CameraEntry savedCam;
	f32 savedProj[7];
	f32 savedVp[6];

	cam2d = camGetPtr(CAMERA_2D);
	savedCam = *camGetPtr(camId);
	*camGetPtr(camId) = *cam2d;
	GXGetProjectionv(savedProj);
	GXGetViewportv(savedVp);
	GXSetProjection(cam2d->projection, cam2d->projType);
	GXSetViewport(cam2d->viewport[0], cam2d->viewport[1], cam2d->viewport[2], cam2d->viewport[3], cam2d->viewport[4], cam2d->viewport[5]);

	winTexInit(*((TEXPalette**)wp2->texture->data));
	winTexSet(
		5,
		(Vec){ 0.0f, 0.0f, 0.0f },
		(Vec){ 1.0f, 1.0f, 1.0f },
		(GXColor){ 0xFF, 0xFF, 0xFF, 0xFF }
	);

	*camGetPtr(camId) = savedCam;
	GXSetProjectionv(savedProj);
	GXSetViewport(savedVp[0], savedVp[1], savedVp[2], savedVp[3], savedVp[4], savedVp[5]);
}

void L_titleInit(SequenceWork* work) {
	memset(wp2, 0, sizeof(TitleWork));
	wp2->texture = fileAllocf(4, "%s/mariost.tpl", getMarioStDvdRoot());
	wp2->field_0xC = 0.0f;
	wp2->field_0x10 = 1000.0f;
	wp2->field_0x14 = 0.0f;
	wp2->field_0x18 = -1000.0f;
	wp2->field_0x1C = 0.0f;
	wp2->debug = 0;
	if (seqGetPrevSeq() == SEQ_LOAD) {
		psndSFXAllOff();
	}
	else if (seqGetPrevSeq() != SEQ_LOGO) {
		if (seqGetPrevSeq() == SEQ_MAP_CHANGE) {
			psndBGMOff_f_d(512, 3000, 0);
			psndBGMOff_f_d(513, 3000, 0);
			psndSFXAllOff();
		}
		else if (seqGetPrevSeq() == SEQ_GAME_OVER) {
			psndStopAllFadeOut();
		}
	}
	if (seqGetPrevSeq() != SEQ_LOAD) {
		if (seqGetPrevSeq() == SEQ_GAME_OVER) {
			if (OSGetSoundMode()) {
				SoundSetOutputMode(SND_OUTPUTMODE_STEREO);
			}
			else {
				SoundSetOutputMode(SND_OUTPUTMODE_MONO);
			}
			psndBGMOn(0x200, "BGM_DIGEST1");
			if (psbgm[0].streamId != -1) {
				sound.streams[psbgm[0].streamId].flags |= 0x8000;
			}
		}
		else {
			psndBGMOn(0x200, "BGM_TITLE1");
			if (psbgm[0].streamId != -1) {
				sound.streams[psbgm[0].streamId].flags |= 0x8000;
			}
		}
	}
	camGetPtr(CAMERA_2D)->flags &= ~0x100;
	camGetPtr(CAMERA_2D)->flags |= 0x400;
}

void fn_800093F0(SequenceWork* work) {
	switch (wp2->field_0x8) {
		case 0:
			if (psndBGMStartCheck(0)
				&& animGroupBaseAsync("OFF_d_maku_left_right", 2, NULL)
				&& animGroupBaseAsync("OFF_d_maku_center", 2, NULL))
			{
				if (seqGetPrevSeq() == SEQ_LOAD) {
					fadeEntry(0x40, 0, c_blackColor);
					fadeEntry(0x1F, 0, c_whiteColor);
					wp2->field_0x10 = 120.0f;
					wp2->field_0x18 = -170.0f;
					wp2->field_0x24 = 0;
				}
				else if (seqGetPrevSeq() == SEQ_GAME_OVER) {
					fadeEntry(0x15, 0, c_whiteColor);
					fadeEntry(0x1D, 0, c_whiteColor);
					wp2->field_0x24 = 78;
				}
				else {
					fadeEntry(0x15, 0, c_whiteColor);
					fadeEntry(0x22, 0, c_whiteColor);
					fadeSetNarrowFast();
					wp2->field_0x24 = 23;
				}
				wp2->field_0x8++;
			}
			break;

		case 1:
			wp2->field_0x24--;
			if (wp2->field_0x24 == 0) {
				if (psbgm[0].streamId != -1) {
					sound.streams[psbgm[0].streamId].flags &= ~0x8000;
				}
			}
			if (fadeIsFinish() || wp2->field_0x24 <= 0) {
				if (seqGetPrevSeq() != SEQ_LOAD) {
					fadeEntry(0x14, 500, c_whiteColor);
					wp2->field_0x8++;
				}
				else {
					wp2->field_0x10 = 120.0f;
					wp2->field_0x18 = -170.0f;
					wp2->field_0x20 = 0;
					wp2->field_0x2C = 0;
					wp2->field_0x8 = 3;
				}
			}
			break;

		case 2:
			wp2->field_0xC = 0.0f;
			wp2->field_0x10 = intplGetValue(10, wp2->field_0x20, gp->framerate, 300.0f, 120.0f);
			wp2->field_0x24--;
			if (wp2->field_0x24 == 0) {
				if (psbgm[0].streamId != -1) {
					sound.streams[psbgm[0].streamId].flags &= ~0x8000;
				}
				if (seqGetPrevSeq() == SEQ_GAME_OVER) {
					psndSFXOn((const char*)0xAB8);
				}
			}
			wp2->field_0x14 = 0.0f;
			wp2->field_0x18 = intplGetValue(11, wp2->field_0x20, gp->framerate, -300.0f, -170.0f);
			wp2->field_0x20++;
			if (wp2->field_0x20 > gp->framerate) {
				fileAsyncf(4, NULL, "%s/demo.tpl", getMarioStDvdRoot());
				animGroupBaseAsync("c_peach", 0, NULL);
				animGroupBaseAsync("c_nazo", 0, NULL);
				animGroupBaseAsync("MOBJ_GoldenTreasureBox", 0, NULL);
				animGroupBaseAsync("MOBJ_BigTreasureBox", 0, NULL);
				animGroupBaseAsync("c_roten", 0, NULL);
				animGroupBaseAsync("c_kuribo_n", 0, NULL);
				animGroupBaseAsync("c_tyusan", 0, NULL);
				wp2->field_0x2C = 0;
				wp2->field_0x8++;
			}
			break;

		case 3:
			wp2->field_0x20++;
			if (wp2->field_0x20 > gp->framerate) {
				wp2->field_0x1C += 0.1f;
				if (wp2->field_0x1C > 1.0f) {
					wp2->field_0x1C = 1.0f;
				}
				if (keyGetButtonTrg(0) & (PAD_BUTTON_START | PAD_BUTTON_A)) {
					psndSFXOn((const char*)8);
					wp2->field_0x8 = 10;
				}
				else {
					wp2->field_0x28++;
					if (wp2->field_0x28 > gp->framerate * 19) {
						wp2->field_0x28 = 0;
					}
				}
			}
			break;

		case 10:
			fadeEntry(0x3F, 0, c_blackColor);
			wp2->field_0x24 = 0;
			wp2->field_0x8++;
			// fallthrough
		case 11:
			if (fadeIsFinish() || ++wp2->field_0x24 > 30) {
				fadeEntry(0x1A, 0, c_whiteColor);
				wp2->field_0x24 = 0;
				wp2->field_0x8++;
			}
			break;

		case 12:
			if (fadeIsFinish() || ++wp2->field_0x24 > 30) {
				fadeReset(4);
				seqSetSeq(SEQ_LOAD, NULL, NULL);
			}
			break;

		case 20:
			fadeEntry(0x21, 0, c_whiteColor);
			fadeEntry(FADE_OUT_BLACK, 1000, c_blackColor);
			psndBGMOff_f_d(0x200, 1000, 0);
			wp2->field_0x20 = 0;
			wp2->field_0x8++;
			break;

		case 21:
			wp2->field_0x10 = intplGetValue(11, wp2->field_0x20, gp->framerate, 120.0f, 400.0f);
			wp2->field_0x18 = intplGetValue(11, wp2->field_0x20, gp->framerate, -170.0f, -400.0f);
			wp2->field_0x20++;
			if (wp2->field_0x20 > gp->framerate) {
				wp2->field_0x20 = gp->framerate;
			}
			wp2->field_0x1C -= 0.1f;
			if (wp2->field_0x1C < 0.0f) {
				wp2->field_0x1C = 0.0f;
			}
			if (fadeIsFinish()) {
				fadeReset(1);
				if (pouchGetHP() <= 0) {
					pouchSetHP(1);
				}
				gp->areaFadeInType = 10;
				gp->areaFadeInDuration = 0;
				seqSetSeq(SEQ_MAP_CHANGE, "dmo_00", NULL);
			}
			break;
	}

	if (wp2->field_0x8 != 12 && work->state < 14) {
		dispEntry(CAMERA_3D, 0, 0.0f, disp1, NULL);
		dispEntry(CAMERA_3D_EFFECTB, 0, 0.0f, fn_80008EFC, NULL);
		switch (wp2->debug) {
			case 0:
				if (keyGetButtonTrg(0) & ~PAD_TRIGGER_L) wp2->debug = 0;
				else if (keyGetButtonTrg(0) & PAD_TRIGGER_L) wp2->debug++;
				break;
			case 1:
				if (keyGetButtonTrg(0) & ~PAD_TRIGGER_R) wp2->debug = 0;
				else if (keyGetButtonTrg(0) & PAD_TRIGGER_R) wp2->debug++;
				break;
			case 2:
				if (keyGetButtonTrg(0) & ~PAD_TRIGGER_L) wp2->debug = 0;
				else if (keyGetButtonTrg(0) & PAD_TRIGGER_L) wp2->debug++;
				break;
			case 3:
				if (keyGetButtonTrg(0) & ~PAD_TRIGGER_R) wp2->debug = 0;
				else if (keyGetButtonTrg(0) & PAD_TRIGGER_R) wp2->debug++;
				break;
			case 4:
				if (keyGetButtonTrg(0) & ~PAD_BUTTON_X) wp2->debug = 0;
				else if (keyGetButtonTrg(0) & PAD_BUTTON_X) wp2->debug++;
				break;
			case 5:
				if (keyGetButtonTrg(0) & ~PAD_BUTTON_Y) wp2->debug = 0;
				else if (keyGetButtonTrg(0) & PAD_BUTTON_Y) wp2->debug = -1;
				break;
		}
	}
}
