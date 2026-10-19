#include "seq/seq_battle.h"
#include "seq/seq_title.h"
#include "battle/battle.h"
#include "battle/battle_information.h"
#include "battle/battle_seq_end.h"
#include "driver/animdrv.h"
#include "driver/camdrv.h"
#include "driver/effdrv.h"
#include "driver/extdrv.h"
#include "driver/fadedrv.h"
#include "driver/icondrv.h"
#include "driver/imgdrv.h"
#include "driver/itemdrv.h"
#include "driver/lightdrv.h"
#include "driver/mobjdrv.h"
#include "driver/npcdrv.h"
#include "driver/offscreendrv.h"
#include "driver/seqdrv.h"
#include "event.h"
#include "manager/filemgr.h"
#include "manager/fontmgr.h"
#include "mario/mario.h"
#include "mario/mario_party.h"
#include "mario/mario_pouch.h"
#include "mario/mariost.h"
#include "memory.h"
#include "pmario_sound.h"
#include "party.h"
#include "system.h"
#include <dolphin/os.h>
#include <stdio.h>
#include <string.h>

extern GlobalWork* gp;
static struct BattleWork* lbl_8042B4B0 = NULL;

typedef struct FieldBattle {
	s16 mode;
	u8 field_2[0x6];
	NpcEntry* field_0x8;
	s32 field_0xC;
	s32 field_0x10;
	s32 field_0x14;
	u8 field_0x18[4];
	s32 field_0x1C;
	u8 field_0x20[0x10];
	FieldBattleInfo battleInfo;
	u8 field_0x3C[0x550 - 0x3C];
	u32 field_0x550;
	u32 field_0x554;
	s32 field_0x558;
	u8 field_0x55C[0x570 - 0x55C];
	s32 field_0x570;
} FieldBattle;

// Forward declarations of internal routines
static void fn_800749A8(void);
static s32 battleSequenceInit(void);

static s32 s_battleWorkPrev;

s32 evt_offscreen_entry(EvtEntry* evt, BOOL isFirstCall) {
	const char* name = (const char*)evtGetValue(evt, evt->args[0]);
	offscreenEntry(name);
	return 2;
}

static void fn_800749A8(void) {
	CameraEntry* cam;
	FieldBattleInfo* fbat;
	Vec scaleVec;
	Mtx mtxScale;
	s32 gsw0;

	BattleEnd();
	fn_80030130();
	smartAutoFree(2);
	itemReInit();
	iconReInit();
	extReset();
	fn_80028BDC();

	cam = camGetPtr(CAMERA_3D);
	if (cam->flags & 4) {
		camSetMode(CAMERA_3D, 1);
	}
	else {
		camSetMode(CAMERA_3D, 3);
	}

	npcReset(1);
	mobjReset(1);
	offscreenReset(1);
	imgAutoRelease(1);
	effAutoRelease(1);
	animPoseAutoRelease(1);
	evtmgrReInit();
	npcRecoveryFieldNpc();

	gp->inBattle = 0;
	evtStartAll(4);

	if (s_battleWorkPrev == 0) {
		marioCtrlOn();
		partyCtrlOn();
		marioGetPtr()->flags |= 1;
	}

	L_psndBGM_stop();
	psndClearFlag(0x80);

	{
		Light* paperCraft = lightGetPaperCraft();
		if (paperCraft) {
			paperCraft->field_0x24.x = -200.0f;
			paperCraft->field_0x24.y = 100.0f;
			paperCraft->field_0x24.z = 100.0f;
			PSMTXScale(mtxScale, 3.0f, 3.0f, 3.0f);
			PSMTXMultVec(mtxScale, &paperCraft->field_0x24, &paperCraft->field_0x24);
		}
	}

	gsw0 = evtGetValue(NULL, (s32)0xF5DE0180);
	if (gsw0 >= 109 && gsw0 < 111) {
		unk_800d7270(0x120, 0, 0x2EE, 1);
	}
	else if (gsw0 != 402 && seqGetNextSeq() != SEQ_GAME_OVER) {
		psndBGMOn(0x120, NULL);
		psndENVOn(0x120, 0);
	}
}

static s32 battleSequenceInit(void) {
	FieldBattle* fbat = (FieldBattle*)fbatGetPointer();
	NpcEntry* npc = fbat->field_0x8;
	PouchData* pouch = pouchGetPtr();
	FieldBattleInfo* info;
	s32 partyId;
	PartyEntry* party;
	const char* msg;
	s32 unitKind;
	s32 firstAtk;

	camSetMode(CAMERA_3D, 2);

	if (gp->field_C != 0) {
		msg = DbgBtlSel_GetMsgDataPtr();
		if (!msg) {
			msg = "gon_11";
		}
		fn_800859B0(msg, 0);
	}

	fn_80030160();
	lbl_8042B4B0 = NULL;
	animPoseBattleInit();
	npcReleaseFieldNpc();

	gp->inBattle = 1;
	evtmgrReInit();
	fn_800DCF0C(0x10);

	info = (FieldBattleInfo*)&fbat->battleInfo;
	BattleInformationInit(info);
	BattleInformationSetMode(info, 0);

	partyId = marioGetPartyId();
	party = partyGetPtr(partyId);
	if (party) {
		unitKind = BattleTransPartyIdToUnitKind((s8)party->currentMemberId);
		if (!(pouch->partyData[(s8)party->currentMemberId].flags & 2)) {
			BattleInformationSetParty(info, unitKind);
		}
	}

	if (fbat->field_0x1C & 1) {
		BattleInformationSetFirstAttack(info, 0);
	}
	else {
		firstAtk = fbat->field_0x10;
		if (firstAtk == 0x20) {
			BattleInformationSetFirstAttack(info, 5);
		}
		else if (firstAtk == 0x8) {
			BattleInformationSetFirstAttack(info, 3);
		}
		else if (firstAtk == 0x4) {
			BattleInformationSetFirstAttack(info, 2);
		}
		else if (firstAtk == 0x2) {
			BattleInformationSetFirstAttack(info, 1);
		}
		else if (firstAtk == 0x10) {
			BattleInformationSetFirstAttack(info, 4);
		}
		else if (firstAtk == 0x100) {
			BattleInformationSetFirstAttack(info, 8);
		}
		else if (firstAtk == 0x80) {
			BattleInformationSetFirstAttack(info, 7);
		}
		else if (firstAtk == 0x40) {
			BattleInformationSetFirstAttack(info, 6);
		}
		else if (firstAtk == 0x1000) {
			BattleInformationSetFirstAttack(info, fbat->field_0x14);
		}
		else {
			BattleInformationSetFirstAttack(info, 0);
		}
	}

	BattleInfomationSetBattleSetupInfo(info, (NpcBattleInfo*)&npc->field_0x230);
	info->field_0x18 = *(u8*)((u8*)npc + 0x2F4);
	info->field_0x1a = *(u8*)((u8*)npc + 0x2F5);
	info->field_0x1b = *(u8*)((u8*)npc + 0x2F6);

	BattleInit(info);
	return 0;
}

void seq_battleMain(SequenceWork* work) {
	GXColor blackColor;
	blackColor.r = 0;
	blackColor.g = 0;
	blackColor.b = 0;
	blackColor.a = 0xFF;

	switch (work->state) {
		case 0:
			fadeEntry(22, 1000, blackColor);
			work->state++;
			work->field_0x10 = 0;
			break;

		case 1:
			if (fileAsyncf(4, 0, "%s/%s", getMarioStDvdRoot(), "battle/audience/audience_normal.tpl") &&
			    fileAsyncf(4, 0, "%s/%s", getMarioStDvdRoot(), "battle/audience/audience_luigi.tpl")) {
				work->field_0x10++;
				if (fadeIsFinish()) {
					work->state++;
				}
			}
			break;

		case 2: {
			FieldBattle* fbat = (FieldBattle*)fbatGetPointer();
			if (fbat->field_0x10 == 0x01000000 || fbat->field_0x10 == 0x00800000) {
				if (s_battleWorkPrev == 0) {
					marioGetPtr()->flags &= ~1;
				}
			}
			else {
				if (s_battleWorkPrev == 0) {
					marioCtrlOff();
					partyCtrlOff();
					marioGetPtr()->flags &= ~1;
					padRumbleOff(0);
				}
			}
			((FieldBattle*)fbatGetPointer())->field_0x570 = 0;
			work->state++;
			break;
		}

		case 3: {
			FieldBattle* fbat = (FieldBattle*)fbatGetPointer();
			PouchData* pouch = pouchGetPtr();
			BOOL wait = FALSE;

			if (fbat->field_0x550 != 0) {
				if (fbat->field_0x558 >= 1 && fbat->field_0x558 < 3 && fbat->field_0x554 != 0) {
					wait = FALSE;
				}
				else {
					wait = TRUE;
				}
			}

			if (!wait) {
				fbat->field_0x570++;
				if (fbat->field_0x570 < 200) {
					break;
				}
			}

			switch (pouch->field_0x88) {
				case 0:
					fn_80028A60("bti_01", *(u32*)((u8*)fbat->field_0x8->field_0x230 + 4));
					break;
				case 1:
					fn_80028A60("bti_02", *(u32*)((u8*)fbat->field_0x8->field_0x230 + 4));
					break;
				case 2:
					fn_80028A60("bti_03", *(u32*)((u8*)fbat->field_0x8->field_0x230 + 4));
					break;
				default:
					fn_80028A60("bti_04", *(u32*)((u8*)fbat->field_0x8->field_0x230 + 4));
					break;
			}
			work->state++;
			break;
		}

		case 4:
			if (battleSequenceInit() == 0) {
				work->state++;
			}
			break;

		case 5:
			if (battleSeqEndCheck()) {
				work->field_0x10 = 0;
				work->state++;
				if (gp->field_C != 0) {
					work->state = 11;
				}
			}
			else {
				Vec listenerPos;
				CameraEntry* cam;
				listenerPos.x = 0.0f;
				listenerPos.y = 0.0f;
				listenerPos.z = 400.0f;
				if (lbl_8042B4B0 && (lbl_8042B4B0->flags & 0x100)) {
					listenerPos.z += 200.0f;
				}
				cam = camGetPtr(CAMERA_3D);
				psndSetPosDirListener(&listenerPos, cam->viewYaw);
			}
			break;

		case 6:
			work->field_0x10++;
			if (work->field_0x10 > 30) {
				if ((gp->flags & (1 << 12)) && (gp->flags & (1 << 13))) {
					seqSetSeq(SEQ_GAME_OVER, (const char*)1, NULL);
				}
				else {
					seqSetSeq(SEQ_GAME, (const char*)1, NULL);
				}
			}
			break;

		case 10:
			fadeEntry(22, 200, blackColor);
			work->state++;
			break;

		case 11:
			if (fadeIsFinish()) {
				gp->field_C = 0;
				seqSetSeq(SEQ_GAME_OVER, (const char*)1, NULL);
			}
			break;

		default:
			break;
	}
}

void seq_battleExit(SequenceWork* work) {
	fn_800749A8();
}

void seq_battleInit(SequenceWork* work) {
	FieldBattle* fbat;
	NpcEntry* npc;

	s_battleWorkPrev = gp->field_C;
	fbat = (FieldBattle*)fbatGetPointer();
	npc = fbat->field_0x8;

	if ((evtGetValue(NULL, (s32)0xF5DE0180) < 109 ||
	     evtGetValue(NULL, (s32)0xF5DE0180) >= 111) &&
	    evtGetValue(NULL, (s32)0xF5DE0180) != 402) {
		psndBGMOff(0x400);
		psndBGMOn(1, *(char**)((u8*)npc + 0x2F0));
	}
	psndENVOff(0x400);
	padRumbleOff(0);
	evtStopAll(4);
	fn_800DCF0C(0x80);

	fbat = (FieldBattle*)fbatGetPointer();
	if (fbat->field_0x10 == 0x01000000 || fbat->field_0x10 == 0x00800000) {
		if (s_battleWorkPrev == 0) {
			marioCtrlOff();
			partyCtrlOff();
			padRumbleOff(0);
		}
	}
}
