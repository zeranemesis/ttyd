#include "mario/mariost.h"

#include "battle/battle.h"

#include "driver/animdrv.h"
#include "driver/arcdrv.h"
#include "driver/bgdrv.h"
#include "driver/camdrv.h"
#include "driver/casedrv.h"
#include "driver/dispdrv.h"
#include "driver/effdrv.h"
#include "driver/envdrv.h"
#include "driver/extdrv.h"
#include "driver/fadedrv.h"
#include "driver/hitdrv.h"
#include "driver/icondrv.h"
#include "driver/imgdrv.h"
#include "driver/itemdrv.h"
#include "driver/lightdrv.h"
#include "driver/mapdrv.h"
#include "driver/mobjdrv.h"
#include "driver/msgdrv.h"
#include "driver/npcdrv.h"
#include "driver/offscreendrv.h"
#include "driver/seqdrv.h"
#include "driver/shadowdrv.h"
#include "driver/swdrv.h"
#include "driver/windowdrv.h"

#include "evt/evt_badgeshop.h"
#include "evt/evt_johoya.h"
#include "evt/evt_yuugijou.h"

#include "mario/mario.h"

#include "manager/arammgr.h"
#include "manager/cardmgr.h"
#include "manager/dvdmgr.h"
#include "manager/evtmgr.h"
#include "manager/filemgr.h"
#include "manager/fontmgr.h"
#include "manager/winmgr.h"

#include "sdk/DEMOInit.h"

#include "win/win_main.h"

#ifndef TARGET_PC
#define printf(...) ((void)0)
#define fflush(...) ((void)0)
#define gcDvdCheckThread fn_800703BC
#define DvdCheckThreadOn DvdCheckTreadOn
#endif

#include "countdown.h"
#include "error_handler.h"
#include "memory.h"
#include "nameent.h"
#include "pmario_sound.h"
#include "romfont.h"
#include "sound.h"
#include "statuswindow.h"
#include "system.h"

#include <dolphin/vi.h>
#include <string.h>

extern void DEMOPadInit(void);
extern BOOL g_bFirstSmartAlloc;

//.bss
GlobalWork marioSt;
OSThread DvdCheckThread;
static u8 stack[0x1000];

//.sdata
static OSTime off_time[4] = {0, 0, 0, 0};
static BOOL off_trg[4] = {FALSE, FALSE, FALSE, FALSE};
#ifndef TARGET_PC
#define gp wp_80422810
#endif
GlobalWork* gp = &marioSt;

//.sbss
OSTime none_key;
#ifndef TARGET_PC
#define _mariostSystemLevel work_8042B260
#endif
s32 _mariostSystemLevel;
BOOL DvdCheckThreadOn; //DvdCheckTreadOn

//.data
GXRenderModeObj GXNtsc480ProgMarioSt = {
	VI_TVMODE_NTSC_PROG, //viTVmode
	0x260, //fbWidth
	0x1E0, //efbHeight
	0x1E0, //xfbHeight
	0x1E, //viXOrigin
	0x0, //viYOrigin
	0x294, //viWidth
	0x1E0, //viHeight
	VI_XFBMODE_SF, //xFBmode
	0, //field_rendering
	0, //aa
	6, 6, 6, 6, 6, 6, //pix 0
	6, 6, 6, 6, 6, 6, //pix 1
	6, 6, 6, 6, 6, 6, //pix 2
	6, 6, 6, 6, 6, 6, //pix 3
	8, 8, 10, 12, 10, 8, 8, //vfilter
};

GXRenderModeObj GXNtsc480IntDfMarioSt = {
	VI_TVMODE_NTSC_INT, //viTVmode
	0x260, //fbWidth
	0x1E0, //efbHeight
	0x1E0, //xfbHeight
	0x1E, //viXOrigin
	0x0, //viYOrigin
	0x294, //viWidth
	0x1E0, //viHeight
	VI_XFBMODE_DF, //xFBmode
	0, //field_rendering
	0, //aa
	6, 6, 6, 6, 6, 6, //pix 0
	6, 6, 6, 6, 6, 6, //pix 1
	6, 6, 6, 6, 6, 6, //pix 2
	6, 6, 6, 6, 6, 6, //pix 3
	8, 8, 10, 12, 10, 8, 8, //vfilter
};

GXRenderModeObj sRMObjHReso_prog = {
	VI_TVMODE_NTSC_PROG, //viTVmode
	0x260, //fbWidth
	0x1C0, //efbHeight
	0x1C0, //xfbHeight
	0x1B, //viXOrigin
	0x10, //viYOrigin
	0x29A, //viWidth
	0x1C0, //viHeight
	VI_XFBMODE_SF, //xFBmode
	0, //field_rendering
	0, //aa
	6, 6, 6, 6, 6, 6, //pix 0
	6, 6, 6, 6, 6, 6, //pix 1
	6, 6, 6, 6, 6, 6, //pix 2
	6, 6, 6, 6, 6, 6, //pix 3
	8, 8, 10, 12, 10, 8, 8, //vfilter
};

GXRenderModeObj sRMObjHReso = {
	VI_TVMODE_NTSC_INT, //viTVmode
	0x260, //fbWidth
	0x1C0, //efbHeight
	0x1C0, //xfbHeight
	0x1B, //viXOrigin
	0x10, //viYOrigin
	0x29A, //viWidth
	0x1C0, //viHeight
	VI_XFBMODE_DF, //xFBmode
	0, //field_rendering
	0, //aa
	6, 6, 6, 6, 6, 6, //pix 0
	6, 6, 6, 6, 6, 6, //pix 1
	6, 6, 6, 6, 6, 6, //pix 2
	6, 6, 6, 6, 6, 6, //pix 3
	8, 8, 10, 12, 10, 8, 8, //vfilter
};

//local prototypes
void viPostCallback(u32 retraceCount);
void* gcDvdCheckThread(void* param);
void gcRumbleCheck(void);
void gcResetCheck(void);

void marioStInit(void) {
	printf("[marioStInit] Starting marioStInit...\n"); fflush(stdout);
#ifdef TARGET_PC
	if (!OSIsRestart() || (OSGetResetCode() & OS_RESET_SHUTDOWN)) {
#else
	if (!(OSGetResetCode() & 0x80000000) || (OSGetResetCode() & OS_RESET_SHUTDOWN)) {
#endif
		DEMOInit(&GXNtsc480IntDfMarioSt);
	}
	else {
		switch (OSGetResetCode() & OS_RESET_HOTRESET) {
			case 0:
				DEMOInit(&GXNtsc480IntDfMarioSt);
				break;
			case 1:
				DEMOInit(&GXNtsc480ProgMarioSt);
				break;
		}
	}
	printf("[marioStInit] DEMOInit finished.\n"); fflush(stdout);
	DEMOEnableGPHangWorkaround(5);
	setupErrorHandler();
	DEMOPadInit();
	GXSetCopyClear((GXColor){0, 0, 0, 0}, 0xFFFFFF);
	__GXSetIndirectMask(0);
	printf("[marioStInit] Initializing GlobalWork gp...\n"); fflush(stdout);
	memset(gp, 0, sizeof(GlobalWork));
	gp->fbWidth = DEMOGetRenderModeObj()->fbWidth;
	gp->efbHeight = DEMOGetRenderModeObj()->efbHeight;
	gp->unk1294 = 1;
	gp->unk1274 = 0;
	printf("[marioStInit] GlobalWork basic fields set.\n"); fflush(stdout);

	gp->retraceLocalTime = 0;
	gp->retraceDeltaTime = 0;
	gp->retraceTime = OSGetTime();

	gp->renderFieldTime = 0;
	gp->renderTime = 0;

	gp->systemLv3Time = 0;
	gp->systemLv2Time = 0;
	gp->systemLv1Time = 0;
	gp->systemLv0Time = 0;

	gp->mapFadeOutType = FADE_IN_BLACK;
	gp->mapFadeOutDuration = 300;
	gp->mapFadeInType = FADE_OUT_BLACK;
	gp->mapFadeInDuration = 300;
	gp->areaFadeOutType = FADE_IN_BLACK;
	gp->areaFadeOutDuration = 300;
	gp->areaFadeInType = FADE_OUT_BLACK;
	gp->areaFadeInDuration = 300;
#ifdef TARGET_PC
	gp->language = 3; // 3 = French PAL ("Paper Mario: La Porte Millenaire")
#else
	if ((OSGetFontEncode() == OS_FONT_ENCODE_SJIS) && !(gp->flags & 0x1000))
		gp->language = 0;
	else
		gp->language = 3; // 3 = French PAL ("Paper Mario: La Porte Millenaire")
#endif
	gp->framerate = VIGetTvFormat() == VI_NTSC ? 60 : 50;
	gp->systemLevelFlags = 0;
	printf("[marioStInit] badgeShop_init...\n"); fflush(stdout);
	badgeShop_init(); //Badge Shop
	printf("[marioStInit] yuugijou_init...\n"); fflush(stdout);
	yuugijou_init(); //Pianta Parlor
	printf("[marioStInit] johoya_init...\n"); fflush(stdout);
	johoya_init(); //?????
	printf("[marioStInit] __memAlloc mapAlloc (660KB)...\n"); fflush(stdout);
	gp->mapAlloc = __memAlloc(HEAP_DEFAULT, 660 * 1024);
	printf("[marioStInit] mapAlloc allocated at %p\n", gp->mapAlloc); fflush(stdout);
	printf("[marioStInit] VISetPostRetraceCallback...\n"); fflush(stdout);
	VISetPostRetraceCallback(viPostCallback);
	printf("[marioStInit] Calling romFontInit...\n"); fflush(stdout);
	romFontInit();
	printf("[marioStInit] romFontInit done.\n"); fflush(stdout);
	OSCreateThread(&DvdCheckThread, gcDvdCheckThread, NULL, (void*)(stack + sizeof(stack)), sizeof(stack), 16, OS_THREAD_ATTR_DETACH);
	DvdCheckThreadOn = TRUE;
	OSResumeThread(&DvdCheckThread);
	printf("[marioStInit] Calling DVDMgrInit...\n"); fflush(stdout);
	DVDMgrInit();
	printf("[marioStInit] DVDMgrInit done.\n"); fflush(stdout);
	psndInit();
	printf("[marioStInit] psndInit done.\n"); fflush(stdout);
	aramMgrInit();
	printf("[marioStInit] aramMgrInit done.\n"); fflush(stdout);
	fileInit();
	printf("[marioStInit] fileInit done.\n"); fflush(stdout);
	smartInit();
	printf("[marioStInit] smartInit done.\n"); fflush(stdout);
	dispInit();
	printf("[marioStInit] dispInit done.\n"); fflush(stdout);
	camInit();
	printf("[marioStInit] camInit done.\n"); fflush(stdout);
	fontmgrInit();
	printf("[marioStInit] fontmgrInit done.\n"); fflush(stdout);
	windowInit();
	printf("[marioStInit] windowInit done.\n"); fflush(stdout);
	mapInit();
	printf("[marioStInit] mapInit done.\n"); fflush(stdout);
	hitInit();
	printf("[marioStInit] hitInit done.\n"); fflush(stdout);
	itemInit();
	printf("[marioStInit] itemInit done.\n"); fflush(stdout);
	iconInit();
	printf("[marioStInit] iconInit done.\n"); fflush(stdout);
	fadeInit();
	printf("[marioStInit] fadeInit done.\n"); fflush(stdout);
	bgInit();
	printf("[marioStInit] bgInit done.\n"); fflush(stdout);
	shadowInit();
	printf("[marioStInit] shadowInit done.\n"); fflush(stdout);
	evtmgrInit();
	printf("[marioStInit] evtmgrInit done.\n"); fflush(stdout);
	animInit();
	printf("[marioStInit] animInit done.\n"); fflush(stdout);
	msgInit();
	printf("[marioStInit] msgInit done.\n"); fflush(stdout);
	npcInit();
	printf("[marioStInit] npcInit done.\n"); fflush(stdout);
	mobjInit();
	printf("[marioStInit] mobjInit done.\n"); fflush(stdout);
	effInit();
	printf("[marioStInit] effInit done.\n"); fflush(stdout);
	marioInit();
	printf("[marioStInit] marioInit done.\n"); fflush(stdout);
	caseInit();
	printf("[marioStInit] caseInit done.\n"); fflush(stdout);
	imgInit();
	printf("[marioStInit] imgInit done.\n"); fflush(stdout);
	lightInit();
	printf("[marioStInit] lightInit done.\n"); fflush(stdout);
	offscreenInit();
	printf("[marioStInit] offscreenInit done.\n"); fflush(stdout);
	arcInit();
	printf("[marioStInit] arcInit done.\n"); fflush(stdout);
	extInit();
	printf("[marioStInit] extInit done.\n"); fflush(stdout);
	swInit();
	printf("[marioStInit] swInit done.\n"); fflush(stdout);
	cardInit();
	printf("[marioStInit] cardInit done.\n"); fflush(stdout);
	winInit();
	printf("[marioStInit] winInit done.\n"); fflush(stdout);
	statusWinInit();
	printf("[marioStInit] statusWinInit done.\n"); fflush(stdout);
	envInit();
	printf("[marioStInit] envInit done.\n"); fflush(stdout);
	countDownInit();
	printf("[marioStInit] countDownInit done.\n"); fflush(stdout);
	nameEntInit();
	printf("[marioStInit] nameEntInit done.\n"); fflush(stdout);
	winMgrInit();
	printf("[marioStInit] winMgrInit done.\n"); fflush(stdout);
	seqInit_MARIOSTORY();
	printf("[marioStInit] seqInit_MARIOSTORY done.\n"); fflush(stdout);
	seqSetSeq(0, NULL, NULL);
	printf("[marioStInit] seqSetSeq(0) done!\n"); fflush(stdout);
	gp->unk1274 = 0;
	if (OSGetSoundMode() == OS_SOUND_MODE_MONO) {
		SoundSetOutputMode(SND_OUTPUTMODE_MONO);
	}
	else {
		SoundSetOutputMode(SND_OUTPUTMODE_STEREO);
	}
	printf("[marioStInit] === ALL SUBSYSTEMS INITIALIZED SUCCESSFULLY ===\n"); fflush(stdout);
}

void marioStMain(void) {
	int i;
#ifdef TARGET_PC
	static int frame = 0;
	static int s_lastSeq = -1;
	static int s_seqFrames = 0;
	BOOL verbose;
	if (seqGetSeq() != s_lastSeq) {
		s_lastSeq = seqGetSeq();
		s_seqFrames = 0;
	}
	s_seqFrames++;
	verbose = (frame < 5);
	if (verbose) {
		printf("[marioStMain] start frame=%d, seq=%d (seqFrame=%d)\n", frame, seqGetSeq(), s_seqFrames); fflush(stdout);
	}
#else
#define verbose 0
#endif

	while (gp->dvdError) {
		OSYieldThread();
	}
	makeKey();
	if (verbose) { printf("  -> makeKey done\n"); fflush(stdout); }
	seqMain();
	if (verbose) { printf("  -> seqMain done\n"); fflush(stdout); }
	cardMain();
	if (verbose) { printf("  -> cardMain done\n"); fflush(stdout); }
	bgMain();
	if (verbose) { printf("  -> bgMain done\n"); fflush(stdout); }
	shadowMain();
	if (verbose) { printf("  -> shadowMain done\n"); fflush(stdout); }
	animMain();
	if (verbose) { printf("  -> animMain done\n"); fflush(stdout); }
	battleMain();
	if (verbose) { printf("  -> battleMain done\n"); fflush(stdout); }
	camMain();
	if (verbose) { printf("  -> camMain done\n"); fflush(stdout); }
	windowMain();
	if (verbose) { printf("  -> windowMain done\n"); fflush(stdout); }
	mapMain();
	if (verbose) { printf("  -> mapMain done\n"); fflush(stdout); }
	hitMain();
	if (verbose) { printf("  -> hitMain done\n"); fflush(stdout); }
	evtmgrMain();
	if (verbose) { printf("  -> evtmgrMain done\n"); fflush(stdout); }
	mapDisp();
	if (verbose) { printf("  -> mapDisp done\n"); fflush(stdout); }
	marioMain();
	if (verbose) { printf("  -> marioMain done\n"); fflush(stdout); }
	mobjMain();
	if (verbose) { printf("  -> mobjMain done\n"); fflush(stdout); }
	npcMain();
	if (verbose) { printf("  -> npcMain done\n"); fflush(stdout); }
	fadeMain();
	if (verbose) { printf("  -> fadeMain done\n"); fflush(stdout); }
	imgMain();
	if (verbose) { printf("  -> imgMain done\n"); fflush(stdout); }
	lightMain();
	if (verbose) { printf("  -> lightMain done\n"); fflush(stdout); }
	offscreenMain();
	if (verbose) { printf("  -> offscreenMain done\n"); fflush(stdout); }
	itemMain();
	if (verbose) { printf("  -> itemMain done\n"); fflush(stdout); }
	caseMain();
	if (verbose) { printf("  -> caseMain done\n"); fflush(stdout); }
	iconMain();
	if (verbose) { printf("  -> iconMain done\n"); fflush(stdout); }
	extMain();
	if (verbose) { printf("  -> extMain done\n"); fflush(stdout); }
	psndMain();
	if (verbose) { printf("  -> psndMain done\n"); fflush(stdout); }
	winMain();
	if (verbose) { printf("  -> winMain done\n"); fflush(stdout); }
	statusWinMain();
	if (verbose) { printf("  -> statusWinMain done\n"); fflush(stdout); }
	envMain();
	if (verbose) { printf("  -> envMain done\n"); fflush(stdout); }
	countDownMain();
	if (verbose) { printf("  -> countDownMain done\n"); fflush(stdout); }
	nameEntMain();
	if (verbose) { printf("  -> nameEntMain done\n"); fflush(stdout); }
	winMgrMain();
	if (verbose) { printf("  -> winMgrMain done\n"); fflush(stdout); }
	effMain();
	if (verbose) { printf("  -> effMain done\n"); fflush(stdout); }
	gcResetCheck();
	if (verbose) { printf("  -> gcResetCheck done\n"); fflush(stdout); }
#ifdef TARGET_PC
	frame++;
#endif
	if (gp->flags & 0x1000) {
		for (i = 0; i < 4; i++) {
			if (gp->dirs[i]) break;
			if (gp->button[i]) break;
			if (gp->stickX[i]) break;
			if (gp->stickY[i]) break;
			if (gp->substickX[i]) break;
			if (gp->substickY[i]) break;
			if (gp->triggerLeft[i]) break;
			if (gp->triggerRight[i]) break;
		}
		if (i >= 4) {
			if (OSTicksToSeconds(OSGetTime() - none_key) > 120 && (seqGetSeq() == 2 || seqGetSeq() == 4)) {
				gp->flags |= 0x2000;
			}
		}
		else {
			none_key = OSGetTime();
		}
	}
}

void marioStDisp(void) {
	camDraw();
	g_bFirstSmartAlloc = FALSE;
}

void marioStSystemLevel(s32 level) {
	switch (level) {
		case 0:
			_mariostSystemLevel = level;
			gp->systemLevelFlags &= ~0xF;
			evtStartAll(0xEF);
			break;
		case 1:
			_mariostSystemLevel = level;
			gp->systemLevelFlags &= ~0xE;
			gp->systemLevelFlags |= 1;
			evtStopAll(1);
			break;
		case 2:
			_mariostSystemLevel = level;
			gp->systemLevelFlags &= ~0xC;
			gp->systemLevelFlags |= 3;
			evtStopAll(2);
			break;
		case 3:
			_mariostSystemLevel = level;
			gp->systemLevelFlags &= ~8;
			gp->systemLevelFlags |= 7;
			evtStopAll(0x10);
			break;
		case 4:
			_mariostSystemLevel = level;
			gp->systemLevelFlags |= 0xF;
			evtStopAll(0xEF);
			break;
	}
}

s32 marioStGetSystemLevel(void) {
	return gp->systemLevelFlags;
}

void viPostCallback(u32 retraceCount) {
	OSTime time, check;
	u32 reset = PAD_BUTTON_B | PAD_BUTTON_X | PAD_BUTTON_START; //TODO: make sure it inlines right

	time = OSGetTime();
	gp->retraceDeltaTime = time - gp->retraceTime;
	gp->retraceLocalTime += gp->retraceDeltaTime;
	check = OSSecondsToTicks(359999999);
	if (check < gp->retraceLocalTime) {
		gp->retraceLocalTime = check;
	}
	if (!(gp->systemLevelFlags & 8)) {
		gp->systemLv3Time += gp->retraceDeltaTime;
	}
	if (!(gp->systemLevelFlags & 4)) {
		gp->systemLv2Time += gp->retraceDeltaTime;
	}
	if (!(gp->systemLevelFlags & 2)) {
		gp->systemLv1Time += gp->retraceDeltaTime;
	}
	if (!(gp->systemLevelFlags & 1)) {
		gp->systemLv0Time += gp->retraceDeltaTime;
	}
	if (!gp->dvdError && !winCheck()) {
		gp->renderTime += gp->retraceDeltaTime;
		if (!gp->inBattle) {
			gp->renderFieldTime += gp->retraceDeltaTime;
		}
	}
	gp->retraceTime = time;
	gp->retraceCount = retraceCount;
	if (battleDisableHResetCheck()) {
		gp->unk127C = FALSE;
	}
	else {
		if (gp->unk127C) {
			if (!OSGetResetButtonState()) {
				gp->resetType = 1;
			}
		}
		else {
			if (OSGetResetButtonState()) {
				gp->unk127C = TRUE;
			}
		}
		if (gp->unk1324) {
			if (OSIsRestart() && !gp->field_0x1290) {
				if ((keyGetButton(0) & reset) == reset) {
					gp->resetType = 0;
					goto exitcheck;
				}
				else {
					gp->field_0x1290 = TRUE;
				}
			}
			if ((keyGetButton(0) & reset) == reset) {
				if (!gp->softResetWait) {
					gp->softResetWait = TRUE;
					gp->softResetTimeout = OSGetTime();
				}
				if (OSTicksToMilliseconds(OSGetTime() - gp->softResetTimeout) > 500) {
					gp->resetType = 1;
				}
			}
			else {
				gp->softResetWait = FALSE;
			}
			if (DVDGetDriveStatus() == -1) {
				gp->resetType = 0;
			}
		}
	}
exitcheck:
	gcRumbleCheck();
	psndMainInt();
}

//TODO: fuck up float conversions for 1:1 cuz this is cleaner
void* gcDvdCheckThread(void* param) {
#ifdef TARGET_PC
	while (1) {
		OSYieldThread();
	}
	return NULL;
#else
	const char* message;
	s32 msgId;
	f32 width;
	Mtx44 mtx;

	while (1) {
		msgId = 0;
		switch (DVDGetDriveStatus()) {
			case DVD_STATE_COVER_OPEN:
				msgId = 5;
				gp->dvdError = TRUE;
				break;
			case DVD_STATE_NO_DISK:
				msgId = 6;
				gp->dvdError = TRUE;
				break;
			case DVD_STATE_WRONG_DISK:
				msgId = 6;
				gp->dvdError = TRUE;
				break;
			case DVD_STATE_RETRY:
				msgId = 7;
				gp->dvdError = TRUE;
				break;
			case DVD_RESULT_FATAL_ERROR:
				msgId = 8;
				gp->dvdError = TRUE;
				break;
			default:
				gp->dvdError = FALSE;
				break;
			case DVD_STATE_WAITING:
				break;
		}
		if (gp->dvdError) {
			SoundOpenCover();
		}
		else {
			SoundCloseCover();
		}
		if (gp->dvdError) {
			if (cardIsExec()) {
				gp->dvdError = FALSE;
			}
		}
		if (gp->dvdError) {
			DEMOBeforeRender();
			if (msgId) {
				message = romFontGetMessage(msgId);
				width = (f32)romFontGetWidth(message);
				MTXPerspective(mtx, 25.0f, 1.2666667f, 1.0f, 10000.0f);
				GXSetProjection(mtx, GX_PERSPECTIVE);
				romFontPrintGX(-width * 0.5f, 60.0f, 1.0f, &(GXColor){0xFF, 0xFF, 0xFF, 0xFF}, message);
			}
			DEMODoneRender();
			makeKey();
			gcResetCheck();
		}
		OSYieldThread();
	}
#endif
}

void gcRumbleCheck(void) {
	/*OSTime start;
	volatile BOOL array[4] = { 0 };
	int i;

	start = OSGetTime();
	for (i = 0; i < 4; i++) {
		if (gp->field_0x12E8[i]) {
			if (OSTicksToMilliseconds(start - gp->field_0x12F0[i]) > 30000) {
				gp->field_0x12EC[i] = 0;
				gp->field_0x12E8[i] = 0;
			}
			array[i] = TRUE;
		}
	}*/
}

#pragma dont_inline on
void gcResetCheck(void) {
	u32 code;

	if (gp->resetType) {
		VISetBlack(TRUE);
		VIFlush();
		VIWaitForRetrace();
		VIWaitForRetrace();
		VIWaitForRetrace();
		psndExit();
		DVDMgrDelete();
		if ((OSGetCurrentThread() != &DvdCheckThread) && DvdCheckThreadOn) {
			OSCancelThread(&DvdCheckThread);
			DvdCheckThreadOn = FALSE;
		}
		while (cardIsExec()) {
			cardMain();
			VIWaitForRetrace();
		}
		if (gp->resetType != 2) {
			if (!(gp->flags & 4)) {
				code = OS_RESET_SHUTDOWN;
			}
			else {
				if (!(gp->flags & 8)) {
					code = OS_RESET_RESTART;
				}
				else {
					code = OS_RESET_HOTRESET;
				}
			}
			OSResetSystem(OS_RESET_RESTART, code, FALSE);
		}
		else {
			OSResetSystem(OS_RESET_HOTRESET, 0, TRUE);
		}
		while (1) {}
	}
}
#pragma dont_inline reset
