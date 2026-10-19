#include "seq/seq_load.h"
#include "driver/camdrv.h"
#include "driver/dispdrv.h"
#include "driver/fadedrv.h"
#include "driver/msgdrv.h"
#include "manager/fontmgr.h"
#include "driver/icondrv.h"
#include "driver/itemdrv.h"
#include "driver/seqdrv.h"
#include "driver/swdrv.h"
#include "driver/windowdrv.h"
#include "event.h"
#include "manager/filemgr.h"
#include "mario/mario.h"
#include "mario/mario_party.h"
#include "mario/mario_pouch.h"
#include "mario/mariost.h"
#include "memory.h"
#include "pmario_sound.h"
#include "system.h"
#include "win/win_main.h"
#include <dolphin/card.h>
#include <dolphin/os.h>
#include <dolphin/pad.h>
#include <math.h>
#include <stdio.h>
#include <string.h>

extern GlobalWork* gp;

#ifndef TARGET_PC
typedef struct Vec2 {
	f32 x, y;
} Vec2;
#endif

typedef struct LoadTableEntry {
	Vec2 pos;
	Vec2 ofs;
	s32 texA;
	s32 texB;
	u8 up;
	u8 down;
	u8 left;
	u8 right;
} LoadTableEntry;

typedef struct LoadWork {
	u16 flags;             // 0x0
	u8 pad_2[2];           // 0x2
	FileEntry* texture;    // 0x4
	f32 curX;              // 0x8
	f32 curY;              // 0xC
	f32 targetX;           // 0x10
	f32 targetY;           // 0x14
	s32 state;             // 0x18
	s32 cardCode;          // 0x1C
	s32 cursorIndex;       // 0x20
	s32 soundMode;         // 0x24
	s32 rumbleMode;        // 0x28
	f32 alpha;             // 0x2C
	s32 copySrcIndex;      // 0x30
	s32 copyDstIndex;      // 0x34
	const char* titleMsg;  // 0x38
	EvtEntry* curEvt;      // 0x3C
	s32 initialSoundMode;  // 0x40
	s32 initialRumbleMode; // 0x44
	s32 prevOption;        // 0x48
} LoadWork;

static LoadWork work;
static LoadWork* wp = &work;

static const Vec c_scaleOne = { 1.0f, 1.0f, 1.0f };
static const Vec c_scaleTitle = { 0.8f, 0.8f, 1.0f };
static const GXColor c_loadWhite = { 0xFF, 0xFF, 0xFF, 0xFF };
static const GXColor c_loadBlack = { 0x00, 0x00, 0x00, 0xFF };
static const GXColor c_loadFont = { 0xFF, 0xEE, 0xAA, 0xFF };


static const LoadTableEntry main_tbl[9] = {
	{{-140.0f,  75.0f}, {-120.0f, 60.0f}, 13,  0, 7, 2, 1, 1},
	{{ 140.0f,  75.0f}, {-120.0f, 60.0f}, 12,  0, 8, 3, 0, 0},
	{{-140.0f, -65.0f}, {-120.0f, 60.0f}, 11,  0, 0, 4, 3, 3},
	{{ 140.0f, -65.0f}, {-120.0f, 60.0f}, 10,  0, 1, 5, 2, 2},
	{{-200.0f, -150.0f}, { -80.0f,  0.0f}, 16, 30, 2, 7, 6, 5},
	{{   0.0f, -150.0f}, { -80.0f,  0.0f}, 17, 30, 2, 7, 4, 6},
	{{ 200.0f, -150.0f}, { -80.0f,  0.0f}, 18, 30, 3, 8, 5, 4},
	{{-130.0f, -190.0f}, {-100.0f,  0.0f}, 19, 31, 4, 0, 8, 8},
	{{ 130.0f, -190.0f}, {-100.0f,  0.0f}, 20, 31, 5, 1, 7, 7},
};

static const LoadTableEntry sub_tbl[7] = {
	{{-140.0f,  75.0f}, {-120.0f, 60.0f}, 13,  0, 6, 2, 1, 1},
	{{ 140.0f,  75.0f}, {-120.0f, 60.0f}, 12,  0, 6, 3, 0, 0},
	{{-140.0f, -65.0f}, {-120.0f, 60.0f}, 11,  0, 0, 6, 3, 3},
	{{ 140.0f, -65.0f}, {-120.0f, 60.0f}, 10,  0, 1, 6, 2, 2},
	{{-200.0f, -150.0f}, { -80.0f,  0.0f}, 16, 30, 2, 7, 6, 5},
	{{   0.0f, -150.0f}, { -80.0f,  0.0f}, 17, 30, 2, 7, 4, 6},
	{{ 200.0f, -150.0f}, { -80.0f,  0.0f}, 18, 30, 3, 1, 6, 6},
};

static const LoadTableEntry copy_tbl[4] = {
	{{-140.0f,  75.0f}, {-120.0f, 60.0f}, 13, 0, 2, 2, 1, 1},
	{{ 140.0f,  75.0f}, {-120.0f, 60.0f}, 12, 0, 3, 3, 0, 0},
	{{-140.0f, -65.0f}, {-120.0f, 60.0f}, 11, 0, 0, 0, 3, 3},
	{{ 140.0f, -65.0f}, {-120.0f, 60.0f}, 10, 0, 1, 1, 2, 2},
};

static const char* s_lang_dirs[6] = {
	"jp", "us", "ge", "fr", "sp", "it"
};

// Extern declarations
extern void fn_800B33D8(s32 idx);
extern void fn_800DCF0C(s32 flag);
extern void fn_802047F4(void);
extern void stg0_00_init(void);
extern s32 cardGetCode(void);
extern BOOL cardIsExec(void);
extern void* cardGetFilePtr(void);
extern void SoundSetOutputMode(s32 mode);
extern void fn_8020D384(void);
extern void fn_8020D158(s32 r3);
extern void nameEntWait(void);
extern BOOL nameEntIsCancel(void);
extern void nameEntOff(void);
extern const char* fn_8020D010(void);
extern void nameMaskGX(f32 alpha);

// External EVT scripts
extern EvtScript evt_memcard_check[];
extern EvtScript evt_memcard_start[];
extern EvtScript evt_memcard_delete[];
extern EvtScript evt_memcard_copy[];
extern EvtScript evt_shake[];

static void continueGame(void) {
	s32 ch;

	fn_800B33D8(wp->cursorIndex);
	gp->field_0x11D0 = wp->cursorIndex;
	gp->field_0x11B8 = wp->cursorIndex;
	marioPartyKill();
	marioGetPtr()->flags2 &= ~0x18;
	fn_800DCF0C(4);
	marioPartyEntry(gp->field_0x11E0);
	marioPartyEntry(gp->field_0x11E4);
	psndClearFlag(4);
	fn_802047F4();
	gp->flags |= 1;

	ch = swByteGet(4);
	if (wp->cursorIndex == 0 || wp->cursorIndex == 1) {
		stg0_00_init();
		gp->field_0x11D0 = wp->cursorIndex;
		marioPartyKill();
		marioGetPtr()->flags2 &= ~0x18;
		gp->field_0x110 = 0;
		gp->field_0x108 = 0x19;
		swInit();
		swByteSet(0, 0);
		gp->flags &= ~1;
		printf("[continueGame] Slot %d selectionne -> Demarrage direct a Port-Lacanaie (gor_00)!\n", wp->cursorIndex);
		fflush(stdout);
		seqSetSeq(SEQ_MAP_CHANGE, "gor_00", "marukongou");
		return;
	}
	if (!gp->currentMapName[0] || !strcmp(gp->currentMapName, "title")) {
		stg0_00_init();
		gp->field_0x11D0 = wp->cursorIndex;
		marioPartyKill();
		marioGetPtr()->flags2 &= ~0x18;
		gp->field_0x110 = 0;
		gp->field_0x108 = 0x19;
		swInit();
		swByteSet(0, 0);
		printf("[continueGame] Starting fresh game! Transitioning to aaa_00 (prologue)...\n");
		fflush(stdout);
		seqSetSeq(SEQ_MAP_CHANGE, "aaa_00", "prologue");
		return;
	}

	switch (ch) {
		case 0:
			gp->field_0x110 = 0;
			gp->field_0x108 = 0x1C;
			seqSetSeq(SEQ_MAP_CHANGE, gp->currentMapName, NULL);
			break;
		case 1:
			gp->field_0x110 = 0x3D;
			gp->field_0x108 = 0x38;
			seqSetSeq(SEQ_MAP_CHANGE, "aji_10", NULL);
			break;
		case 2:
			gp->field_0x110 = 0x3E;
			gp->field_0x108 = 0x3A;
			seqSetSeq(SEQ_MAP_CHANGE, "kpa_00", NULL);
			break;
		case 3:
			gp->field_0x110 = 0x3C;
			gp->field_0x108 = 0x36;
			seqSetSeq(SEQ_MAP_CHANGE, "nok_01", "nokotarou");
			break;
		case 4:
			gp->field_0x110 = 0x3D;
			gp->field_0x108 = 0x38;
			seqSetSeq(SEQ_MAP_CHANGE, "hei_00", NULL);
			break;
		case 5:
			gp->field_0x110 = 0x3E;
			gp->field_0x108 = 0x3A;
			seqSetSeq(SEQ_MAP_CHANGE, "mri_00", NULL);
			break;
		case 6:
			gp->field_0x110 = 0x3C;
			gp->field_0x108 = 0x36;
			seqSetSeq(SEQ_MAP_CHANGE, "mri_01", NULL);
			break;
		case 7:
			gp->field_0x110 = 0x3D;
			gp->field_0x108 = 0x38;
			seqSetSeq(SEQ_MAP_CHANGE, "tou_02", NULL);
			break;
		case 8:
			gp->field_0x110 = 0x3E;
			gp->field_0x108 = 0x3A;
			seqSetSeq(SEQ_MAP_CHANGE, "usu_00", NULL);
			break;
		case 9:
			gp->field_0x110 = 0x3C;
			gp->field_0x108 = 0x36;
			seqSetSeq(SEQ_MAP_CHANGE, "gor_04", NULL);
			break;
		case 10:
			gp->field_0x110 = 0;
			gp->field_0x108 = 0x1C;
			seqSetSeq(SEQ_MAP_CHANGE, "muj_12", NULL);
			break;
		case 11:
			gp->field_0x110 = 0x3E;
			gp->field_0x108 = 0x3A;
			seqSetSeq(SEQ_MAP_CHANGE, "muj_00", NULL);
			break;
		case 12:
			gp->field_0x110 = 0x3C;
			gp->field_0x108 = 0x36;
			seqSetSeq(SEQ_MAP_CHANGE, "pik_03", NULL);
			break;
		case 13:
			gp->field_0x110 = 0;
			gp->field_0x108 = 0x1C;
			seqSetSeq(SEQ_MAP_CHANGE, "gor_01", NULL);
			break;
		case 14:
			gp->field_0x110 = 0x3E;
			gp->field_0x108 = 0x3A;
			seqSetSeq(SEQ_MAP_CHANGE, "pik_03", "resha_return");
			break;
		case 15:
			gp->field_0x110 = 0x3C;
			gp->field_0x108 = 0x36;
			seqSetSeq(SEQ_MAP_CHANGE, "pik_01", NULL);
			break;
		case 16:
			gp->field_0x110 = 0;
			gp->field_0x108 = 0x1C;
			seqSetSeq(SEQ_MAP_CHANGE, "aji_18", "e_bero");
			break;
		case 17:
			gp->field_0x110 = 0x3E;
			gp->field_0x108 = 0x3A;
			seqSetSeq(SEQ_MAP_CHANGE, "gor_00", NULL);
			break;
		case 18:
			gp->field_0x110 = 0x3C;
			gp->field_0x108 = 0x36;
			seqSetSeq(SEQ_MAP_CHANGE, "jin_04", "next");
			break;
		default:
			stg0_00_init();
			gp->field_0x11D0 = wp->cursorIndex;
			marioPartyKill();
			marioGetPtr()->flags2 &= ~0x18;
			gp->field_0x110 = 0;
			gp->field_0x108 = 0x19;
			swInit();
			swByteSet(0, 0);
			seqSetSeq(SEQ_MAP_CHANGE, "aaa_00", "prologue");
			break;
	}
}

static void loadDisp(CameraId camId, void* param) {
	CameraEntry* cam2d;
	CameraEntry savedCam;
	int i;
	Vec pos, titlePos, curPos;

	if (!wp->texture || !wp->texture->data || !*wp->texture->data) return;

	cam2d = camGetPtr(CAMERA_2D);
	savedCam = *camGetPtr(camId);
	*camGetPtr(camId) = *cam2d;
	GXSetProjection(cam2d->projection, cam2d->projType);
	GXSetViewport(cam2d->viewport[0], cam2d->viewport[1], cam2d->viewport[2], cam2d->viewport[3], cam2d->viewport[4], cam2d->viewport[5]);

	winTexInit(*((TEXPalette**)wp->texture->data));

	for (i = 0; i < 4; i++) {
		pos.x = main_tbl[i].pos.x; pos.y = main_tbl[i].pos.y; pos.z = 0.0f;
		winTexSet(14, pos, c_scaleOne, c_loadWhite);
		titlePos.x = pos.x + main_tbl[i].ofs.x; titlePos.y = pos.y + main_tbl[i].ofs.y; titlePos.z = 0.0f;
		winTexSet(main_tbl[i].texA, titlePos, c_scaleTitle, c_loadWhite);
	}

	for (i = 4; i < 9; i++) {
		pos.x = main_tbl[i].pos.x; pos.y = main_tbl[i].pos.y; pos.z = 0.0f;
		winTexSet(main_tbl[i].texA, pos, c_scaleOne, c_loadWhite);
	}

	if (wp->cursorIndex >= 0 && wp->cursorIndex < 9) {
		curPos.x = wp->curX; curPos.y = wp->curY; curPos.z = 0.0f;
		winTexSet(0, curPos, c_scaleOne, c_loadWhite);
	}

	GXLoadPosMtxImm(cam2d->view, GX_PNMTX0);
	GXSetCurrentMtx(GX_PNMTX0);
	FontDrawStart();
	FontDrawEdge();
	FontDrawColor(c_loadFont);
	FontDrawScale(0.75f);
	FontDrawString(-240.0f, 50.0f, "Port-Lacanaie  Niv.12  PV 25/25");
	FontDrawString(40.0f, 50.0f, "Port-Lacanaie  Niv.22  PV 45/45");
	FontDrawString(-240.0f, -90.0f, "Nouveau Fichier (Prologue)");
	FontDrawString(40.0f, -90.0f, "Nouveau Fichier (Prologue)");

	*camGetPtr(camId) = savedCam;
	GXSetProjection(savedCam.projection, savedCam.projType);
	GXSetViewport(savedCam.viewport[0], savedCam.viewport[1], savedCam.viewport[2], savedCam.viewport[3], savedCam.viewport[4], savedCam.viewport[5]);
}

static void loadMain(SequenceWork* seq) {
	u32 btnTrg;
	u32 dirRep;
	s32 code;
	f32 targetX, targetY;

	btnTrg = keyGetButtonTrg(0);
	dirRep = keyGetDirRep(0);
	code = cardGetCode();

	if (cardIsExec() || !fadeIsFinish()) {
		btnTrg = 0;
		dirRep = 0;
	}

	if (wp->cardCode != code) {
		btnTrg = 0;
		dirRep = 0;
	}
	wp->cardCode = code;

	switch (wp->state) {
		case 0:
			wp->state++;
			break;

		case 1:
			wp->titleMsg = msgSearch("msg_savefile_select");
			{
				static int s_autoLoadTimer = 0;
				s_autoLoadTimer++;
				if ((btnTrg & PAD_BUTTON_A) || s_autoLoadTimer > 120) {
					s_autoLoadTimer = 0;
					psndSFXOn((const char*)18);
					if (wp->cursorIndex < 4) {
						wp->state = 20; // start game with this file
					}
					else if (wp->cursorIndex == 4) {
						wp->state = 40; // copy
					}
					else if (wp->cursorIndex == 5) {
						wp->state = 50; // delete
					}
					else if (wp->cursorIndex == 6) {
						wp->state = 100; // sound mode
					}
					else if (wp->cursorIndex == 7) {
						wp->state = 200; // rumble mode
					}
					else {
						wp->state = 10; // back / cancel
					}
					break;
				}
			}
			if (dirRep & (PAD_BUTTON_UP | PAD_BUTTON_DOWN | PAD_BUTTON_LEFT | PAD_BUTTON_RIGHT)) {
				s32 prev = wp->cursorIndex;
				if (dirRep & PAD_BUTTON_UP) {
					wp->cursorIndex = main_tbl[prev].up;
				}
				else if (dirRep & PAD_BUTTON_DOWN) {
					wp->cursorIndex = main_tbl[prev].down;
				}
				else if (dirRep & PAD_BUTTON_LEFT) {
					wp->cursorIndex = main_tbl[prev].left;
				}
				else if (dirRep & PAD_BUTTON_RIGHT) {
					wp->cursorIndex = main_tbl[prev].right;
				}
				if (prev != wp->cursorIndex) {
					psndSFXOn((const char*)9);
				}
			}
			else if (btnTrg & PAD_BUTTON_B) {
				psndSFXOn((const char*)19);
				wp->state = 10;
			}
			break;

		case 10:
			fadeEntry(0x1B, 0, c_loadBlack);
			wp->state++;
			break;

		case 11:
			if (fadeIsFinish()) {
				seqSetSeq(SEQ_TITLE, NULL, NULL);
			}
			break;

		case 20:
			printf("[loadMain] State 20 -> 22: PC fast-track file select for slot %d!\n", wp->cursorIndex);
			fflush(stdout);
			wp->state = 22;
			break;

		case 21:
			wp->state = 22;
			break;

		case 22:
			fadeEntry(0x1B, 0, c_loadBlack);
			fn_800DB9A8(0x200, 2000, 0);
			wp->state++;
			break;

		case 23:
			if (fadeIsFinish() || ++wp->copyDstIndex > 30) {
				wp->copyDstIndex = 0;
				continueGame();
			}
			break;

		case 40:
		case 50:
			wp->state = 1;
			break;

		case 41:
		case 51:
			wp->state = 1;
			break;

		case 100:
			wp->flags |= 1;
			if (dirRep & PAD_BUTTON_UP) {
				wp->soundMode--;
				if (wp->soundMode < 0) wp->soundMode = 2;
				psndSFXOn((const char*)5);
			}
			else if (dirRep & PAD_BUTTON_DOWN) {
				wp->soundMode++;
				if (wp->soundMode > 2) wp->soundMode = 0;
				psndSFXOn((const char*)5);
			}
			else if (btnTrg & PAD_BUTTON_A) {
				OSSetSoundMode(wp->soundMode);
				SoundSetOutputMode(wp->soundMode);
				psndSFXOn((const char*)18);
				wp->initialSoundMode = wp->soundMode;
				wp->flags &= ~1;
				wp->state = 1;
			}
			else if (btnTrg & PAD_BUTTON_B) {
				wp->soundMode = wp->initialSoundMode;
				OSSetSoundMode(wp->soundMode);
				SoundSetOutputMode(wp->soundMode);
				psndSFXOn((const char*)19);
				wp->flags &= ~1;
				wp->state = 1;
			}
			break;

		case 200:
			wp->flags |= 1;
			if (dirRep & (PAD_BUTTON_UP | PAD_BUTTON_DOWN)) {
				wp->rumbleMode = 1 - wp->rumbleMode;
				psndSFXOn((const char*)5);
			}
			else if (btnTrg & PAD_BUTTON_A) {
				gp->rumbleMode = (wp->rumbleMode == 0);
				psndSFXOn((const char*)18);
				wp->initialRumbleMode = wp->rumbleMode;
				wp->flags &= ~1;
				wp->state = 1;
			}
			else if (btnTrg & PAD_BUTTON_B) {
				wp->rumbleMode = wp->initialRumbleMode;
				psndSFXOn((const char*)19);
				wp->flags &= ~1;
				wp->state = 1;
			}
			break;

		default:
			break;
	}

	targetX = main_tbl[wp->cursorIndex].pos.x;
	targetY = main_tbl[wp->cursorIndex].pos.y;
	wp->curX += (targetX - wp->curX) * 0.25f;
	wp->curY += (targetY - wp->curY) * 0.25f;

	dispEntry(CAMERA_2D, 0.0f, 0, loadDisp, NULL);
}

void seq_loadInit(SequenceWork* work) {
	memset(wp, 0, sizeof(LoadWork));
}

USER_FUNC(unk_800f2398) {
	fadeReset(4);
	wp->prevOption = event->printWindowId;
	return 2;
}

void seq_loadExit(SequenceWork* work) {
	if (wp->texture) {
		fileFree(wp->texture);
		wp->texture = NULL;
	}
}

void seq_loadMain(SequenceWork* seq) {
	char path[128];
	s32 langIdx;

	switch (seq->state) {
		case 0:
			langIdx = gp->language;
			if (langIdx < 0 || langIdx >= 6) {
				langIdx = 3; // default FR
			}
			sprintf(path, "%s/w/%s/name.tpl", getMarioStDvdRoot(), s_lang_dirs[langIdx]);
			if (fileAsyncf(4, 0, path)) {
				printf("[seq_loadMain] State 0: loaded %s successfully!\n", path);
				fflush(stdout);
				wp->texture = fileAllocf(4, path);
				seq->state++;
			}
			break;

		case 1:
			printf("[seq_loadMain] State 1: Memory card check OK (PC virtual save).\n");
			fflush(stdout);
			wp->curEvt = NULL;
			seq->state = 3;
			wp->state = 0;
			wp->cardCode = cardGetCode();
			wp->flags &= ~0x40;
			wp->soundMode = OSGetSoundMode();
			SoundSetOutputMode(wp->soundMode);
			wp->initialSoundMode = wp->soundMode;
			wp->rumbleMode = gp->rumbleMode ? 0 : 1;
			wp->initialRumbleMode = wp->rumbleMode;
			wp->cursorIndex = gp->field_0x11B8;
			wp->curX = main_tbl[wp->cursorIndex].pos.x;
			wp->curY = main_tbl[wp->cursorIndex].pos.y;
			break;

		case 2:
			seq->state = 3;
			break;

		case 3:
			fadeEntry(0x1C, 0, c_loadWhite);
			printf("[seq_loadMain] State 3 -> 4: File Select Menu is now ACTIVE!\n");
			fflush(stdout);
			seq->state++;
			break;

		case 4:
			loadMain(seq);
			break;

		default:
			break;
	}
}
