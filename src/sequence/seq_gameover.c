#include "seq/seq_gameover.h"
#include "driver/camdrv.h"
#include "driver/fadedrv.h"
#include "driver/mapdrv.h"
#include "driver/npcdrv.h"
#include "driver/seqdrv.h"
#include "driver/windowdrv.h"
#include "manager/evtmgr.h"
#include "evt/evt_cmd.h"
#include "evt/evt_mario.h"
#include "mario/mario.h"
#include "mario/mario_pouch.h"
#include "mario/mariost.h"
#include "pmario_sound.h"
#include "system.h"
#include <dolphin/pad.h>
#include <string.h>

#ifndef TARGET_PC
#ifndef intptr_t
typedef s32 intptr_t;
#endif
#endif

extern GlobalWork* gp;
extern const GXColor lbl_8042CF28;

extern void fn_80007740(void);
extern void _unload(const char* map, s32 r4, s32 r5);
extern void statusWinForceOff(void);
extern void statusWinDispOff(void);
extern void statusWinDispOn(void);
extern void countDownEnd(void);
extern void nameEntReInit(void);
extern void camShiftReset(void);
extern void fn_800DB9A8(s32 r3, s32 r4, s32 r5);
extern void fn_800DD010(void);
extern void marioStSystemLevel(s32 level);
extern void N_pouchUnEquipBadgeID(s32 id);
extern void N_evt_snd_bgm_unpause(void);

// Script helpers & compatibility macros
#define USE_BUF(data) SET_INT_READ_ADDR(PTR(data))
#define BUF_READ_ALL(cnt, var) EVT_CMD(1, OPCODE_DO), (cnt), EVT_CMD(1, OPCODE_READ), (var),
#define RUN_CHILD_EVT(script) CALL_CHILD(script)
#define RUN_CHILD_EVT_ID(script, idOut) EVT_CMD(2, OPCODE_RUN_EVENT_ID), PTR(script), (idOut),
#define WAIT_FRM(f) WAIT_FRAMES(f)
#define LBL(l) LABEL(l)
#define JUMP(l) GOTO(l)
#define CHK_EVT(id, out) IS_EVENT_RUNNING(id, out)
#define IF_EQ(a, b) IF_INT_EQUAL(a, b)
#define SET(a, b) SET_VALUE(a, b)
#define RUN_EVT(script) SPAWN_EVENT(script)
#undef LOOP
#define LOOP(c) END_LOOP
#undef USER_FUNC
#define USER_FUNC CALL

s32 evt_mario_cont_onoff(EventEntry* event, BOOL isFirstCall);
s32 evt_mario_set_pos(EventEntry* event, BOOL isFirstCall);
s32 evt_cam_road_reset(EventEntry* event, BOOL isFirstCall);
s32 evt_party_cont_onoff(EventEntry* event, BOOL isFirstCall);
s32 evt_party_set_pos(EventEntry* event, BOOL isFirstCall);
s32 evt_mario_kill_party(EventEntry* event, BOOL isFirstCall);
s32 evt_mario_clear_party(EventEntry* event, BOOL isFirstCall);
s32 evt_mario_dispflag_onoff(EventEntry* event, BOOL isFirstCall);
s32 evt_mario_set_dir(EventEntry* event, BOOL isFirstCall);
s32 evt_mario_adjust_dir(EventEntry* event, BOOL isFirstCall);
s32 evt_sub_get_language(EventEntry* event, BOOL isFirstCall);
s32 evt_mapobj_flag_onoff(EventEntry* event, BOOL isFirstCall);
s32 evt_mapobj_trans(EventEntry* event, BOOL isFirstCall);
s32 evt_cam_ctrl_onoff(EventEntry* event, BOOL isFirstCall);
s32 evt_cam3d_evt_set(EventEntry* event, BOOL isFirstCall);
s32 evt_map_playanim(EventEntry* event, BOOL isFirstCall);
s32 fn_800E7294(EventEntry* event, BOOL isFirstCall);
s32 fn_800E763C(EventEntry* event, BOOL isFirstCall);

static EvtEntry* s_gameOverEvt;

static s32 fall_data[] = {
	200, 170, 140, 110, 80, 50, 20, 10,
	26, 29, 30, 29, 26, 10, 0,
	12, 15, 16, 15, 11, 6, 0,
	4, 3, 1, 0
};

static s32 fall_data2[] = {
	200, 170, 140, 110, 80, 50, 20, 0,
	12, 15, 16, 15, 11, 6, 0,
	4, 3, 1, 0
};

EVT_BEGIN(_fall_char)
	USE_BUF(fall_data)
	BUF_READ_ALL(26, LW(1))
	USER_FUNC(evt_mapobj_trans, 1, LW(0), 0, LW(1), 0)
	WAIT_FRM(1)
	LOOP(0)
	RETURN
EVT_END()

EVT_BEGIN(_fall_char_last)
	USE_BUF(fall_data2)
	BUF_READ_ALL(19, LW(1))
	USER_FUNC(evt_mapobj_trans, 1, LW(0), 0, LW(1), 0)
	WAIT_FRM(1)
	LOOP(0)
	RETURN
EVT_END()

EVT_BEGIN(fall_gameover)
	SET(LW(0), (intptr_t)"g")
	RUN_CHILD_EVT(_fall_char)
	WAIT_FRM(166)
	SET(LW(0), (intptr_t)"a")
	RUN_CHILD_EVT(_fall_char)
	WAIT_FRM(333)
	SET(LW(0), (intptr_t)"m")
	RUN_CHILD_EVT(_fall_char)
	WAIT_FRM(166)
	SET(LW(0), (intptr_t)"e")
	RUN_CHILD_EVT(_fall_char)
	WAIT_FRM(166)
	SET(LW(0), (intptr_t)"o")
	RUN_CHILD_EVT(_fall_char)
	WAIT_FRM(166)
	SET(LW(0), (intptr_t)"v")
	RUN_CHILD_EVT(_fall_char)
	WAIT_FRM(166)
	SET(LW(0), (intptr_t)"e2")
	RUN_CHILD_EVT(_fall_char)
	WAIT_FRM(500)
	SET(LW(0), (intptr_t)"r")
	RUN_CHILD_EVT_ID(_fall_char_last, LW(10))
	LBL(0)
	CHK_EVT(LW(10), LW(11))
	IF_EQ(LW(11), 0)
		GOTO(1)
	END_IF
	WAIT_FRM(1)
	JUMP(0)
	LBL(1)
	WAIT_FRM(1000)
	RETURN
EVT_END()

EVT_BEGIN(fall_gameover_lang)
	SET(LW(0), (intptr_t)"f")
	RUN_CHILD_EVT(_fall_char)
	WAIT_FRM(133)
	SET(LW(0), (intptr_t)"i")
	RUN_CHILD_EVT(_fall_char)
	WAIT_FRM(233)
	SET(LW(0), (intptr_t)"n")
	RUN_CHILD_EVT(_fall_char)
	WAIT_FRM(133)
	SET(LW(0), (intptr_t)"d")
	RUN_CHILD_EVT(_fall_char)
	WAIT_FRM(133)
	SET(LW(0), (intptr_t)"e3")
	RUN_CHILD_EVT(_fall_char)
	WAIT_FRM(133)
	SET(LW(0), (intptr_t)"l")
	RUN_CHILD_EVT(_fall_char)
	WAIT_FRM(233)
	SET(LW(0), (intptr_t)"j")
	RUN_CHILD_EVT(_fall_char)
	WAIT_FRM(133)
	SET(LW(0), (intptr_t)"u")
	RUN_CHILD_EVT(_fall_char)
	WAIT_FRM(133)
	SET(LW(0), (intptr_t)"e4")
	RUN_CHILD_EVT(_fall_char)
	WAIT_FRM(133)
	SET(LW(0), (intptr_t)"g1")
	RUN_CHILD_EVT(_fall_char)
	WAIT_FRM(400)
	SET(LW(0), (intptr_t)"o289")
	RUN_CHILD_EVT_ID(_fall_char_last, LW(10))
	LBL(0)
	CHK_EVT(LW(10), LW(11))
	IF_EQ(LW(11), 0)
		GOTO(1)
	END_IF
	WAIT_FRM(1)
	JUMP(0)
	LBL(1)
	WAIT_FRM(1000)
	RETURN
EVT_END()

EVT_BEGIN(main_gameover_script)
	USER_FUNC(evt_mario_key_onoff, 0)
	USER_FUNC(fn_800E7294)
	USER_FUNC(evt_mario_cont_onoff, 0)
	USER_FUNC(evt_mario_set_pos, 0, 0, 0)
	USER_FUNC(evt_cam_road_reset)
	USER_FUNC(evt_party_cont_onoff, 0, 0)
	USER_FUNC(evt_party_set_pos, 0, 0, -1000, 0)
	USER_FUNC(evt_mario_kill_party, 0)
	USER_FUNC(evt_party_cont_onoff, 1, 0)
	USER_FUNC(evt_party_set_pos, 1, 0, -1000, 0)
	USER_FUNC(evt_mario_clear_party, 1)
	WAIT_FRM(1)
	USER_FUNC(N_evt_snd_bgm_unpause)
	USER_FUNC(evt_mario_dispflag_onoff, 1, 2)
	USER_FUNC(fn_800E763C, PTR("M_D_2"))
	USER_FUNC(evt_mario_set_dir, 180, 0, 0)
	USER_FUNC(evt_mario_adjust_dir)
	USER_FUNC(evt_sub_get_language, LW(0))
	IF_EQ(LW(0), 4)
		USER_FUNC(evt_mapobj_flag_onoff, 1, 1, PTR("f"), 1)
		USER_FUNC(evt_mapobj_flag_onoff, 1, 1, PTR("i"), 1)
		USER_FUNC(evt_mapobj_flag_onoff, 1, 1, PTR("n"), 1)
		USER_FUNC(evt_mapobj_flag_onoff, 1, 1, PTR("d"), 1)
		USER_FUNC(evt_mapobj_flag_onoff, 1, 1, PTR("e3"), 1)
		USER_FUNC(evt_mapobj_flag_onoff, 1, 1, PTR("l"), 1)
		USER_FUNC(evt_mapobj_flag_onoff, 1, 1, PTR("j"), 1)
		USER_FUNC(evt_mapobj_flag_onoff, 1, 1, PTR("u"), 1)
		USER_FUNC(evt_mapobj_flag_onoff, 1, 1, PTR("e4"), 1)
		USER_FUNC(evt_mapobj_flag_onoff, 1, 1, PTR("g1"), 1)
		USER_FUNC(evt_mapobj_flag_onoff, 1, 1, PTR("o289"), 1)
		USER_FUNC(evt_mapobj_trans, 1, PTR("f"), 0, 200, 0)
		USER_FUNC(evt_mapobj_trans, 1, PTR("i"), 0, 200, 0)
		USER_FUNC(evt_mapobj_trans, 1, PTR("n"), 0, 200, 0)
		USER_FUNC(evt_mapobj_trans, 1, PTR("d"), 0, 200, 0)
		USER_FUNC(evt_mapobj_trans, 1, PTR("e3"), 0, 200, 0)
		USER_FUNC(evt_mapobj_trans, 1, PTR("l"), 0, 200, 0)
		USER_FUNC(evt_mapobj_trans, 1, PTR("j"), 0, 200, 0)
		USER_FUNC(evt_mapobj_trans, 1, PTR("u"), 0, 200, 0)
		USER_FUNC(evt_mapobj_trans, 1, PTR("e4"), 0, 200, 0)
		USER_FUNC(evt_mapobj_trans, 1, PTR("g1"), 0, 200, 0)
		USER_FUNC(evt_mapobj_trans, 1, PTR("o289"), 0, 200, 0)
	ELSE
		USER_FUNC(evt_mapobj_flag_onoff, 1, 1, PTR("g"), 1)
		USER_FUNC(evt_mapobj_flag_onoff, 1, 1, PTR("a"), 1)
		USER_FUNC(evt_mapobj_flag_onoff, 1, 1, PTR("m"), 1)
		USER_FUNC(evt_mapobj_flag_onoff, 1, 1, PTR("e"), 1)
		USER_FUNC(evt_mapobj_flag_onoff, 1, 1, PTR("o"), 1)
		USER_FUNC(evt_mapobj_flag_onoff, 1, 1, PTR("v"), 1)
		USER_FUNC(evt_mapobj_flag_onoff, 1, 1, PTR("e2"), 1)
		USER_FUNC(evt_mapobj_flag_onoff, 1, 1, PTR("r"), 1)
		USER_FUNC(evt_mapobj_trans, 1, PTR("g"), 0, 200, 0)
		USER_FUNC(evt_mapobj_trans, 1, PTR("a"), 0, 200, 0)
		USER_FUNC(evt_mapobj_trans, 1, PTR("m"), 0, 200, 0)
		USER_FUNC(evt_mapobj_trans, 1, PTR("e"), 0, 200, 0)
		USER_FUNC(evt_mapobj_trans, 1, PTR("o"), 0, 200, 0)
		USER_FUNC(evt_mapobj_trans, 1, PTR("v"), 0, 200, 0)
		USER_FUNC(evt_mapobj_trans, 1, PTR("e2"), 0, 200, 0)
		USER_FUNC(evt_mapobj_trans, 1, PTR("r"), 0, 200, 0)
	END_IF
	USER_FUNC(evt_cam_ctrl_onoff, 4, 1)
	USER_FUNC(evt_cam3d_evt_set, 0, 208, 653, 0, 91, -30, 0, 11)
	WAIT_FRM(1)
	USER_FUNC(evt_map_playanim, PTR("anm_hokori"), 1, 0)
	USER_FUNC(evt_sub_get_language, LW(0))
	IF_EQ(LW(0), 4)
		RUN_EVT(fall_gameover_lang)
	ELSE
		RUN_EVT(fall_gameover)
	END_IF
	RETURN
EVT_END()

void seq_gameOverInit(SequenceWork* work) {
	winReInit();
	camDispOn(CAMERA_3D);
	fn_800DD010();
	psndSetReverb(0);
	L_psndBGM_stop();
	psndClearFlag(0x80);
	psndClearFlag(0x20);
	psndClearFlag(0x40);
	N_pouchUnEquipBadgeID(0x143);
	marioStSystemLevel(4);
}

void seq_gameOverExit(SequenceWork* work) {
	fn_800DD010();
	marioStSystemLevel(0);
}

void seq_gameOverMain(SequenceWork* work) {
	CameraEntry* cam;
	MarioWork* mario;

	switch (work->state) {
		case 0:
			if (work->mapName == NULL) {
				fadeEntry(2, 1000, lbl_8042CF28);
				gp->areaFadeInType = 10;
				gp->areaFadeInDuration = 300;
			}
			mapPreLoad();
			work->state++;
			break;

		case 1:
			if (fadeIsFinish()) {
				fn_80007740();
				_unload(gp->currentMapName, 0, 0);
				statusWinForceOff();
				statusWinDispOff();

				cam = camGetPtr(CAMERA_3D);
				cam->viewport[0] = 0.0f;
				cam->viewport[1] = 0.0f;
				cam->viewport[2] = (f32)gp->fbWidth;
				cam->viewport[3] = (f32)gp->efbHeight;

				cam = camGetPtr(CAMERA_2D);
				cam->viewport[0] = 0.0f;
				cam->viewport[1] = 0.0f;
				cam->viewport[2] = (f32)gp->fbWidth;
				cam->viewport[3] = (f32)gp->efbHeight;
				cam->flags &= ~0x80;
				cam->flags |= 0x400;
				cam->flags |= 0x200;

				fadeTecOff();
				fadeSoftFocusOff();
				npcClearDeadInfo();
				countDownEnd();
				nameEntReInit();
				work->state++;
			}
			break;

		case 2:
			if (mapPreLoad()) {
				mapLoad("sys_00");
				cam = camGetPtr(CAMERA_3D);
				mario = marioGetPtr();
				cam->blendPos.x = 0.0f;
				cam->blendPos.y = 0.01f;
				cam->blendPos.z = 0.0f;
				cam->blendAt.x = 0.0f;
				cam->blendAt.y = 0.01f;
				cam->blendAt.z = 0.0f;
				mario->position.x = 0.0f;
				mario->position.y = 0.0f;
				mario->position.z = 0.0f;
				camRoadReset();
				camShiftReset();
				work->state++;
			}
			break;

		case 3:
			fadeEntry(31, 200, lbl_8042CF28);
			work->state++;
			break;

		case 4:
			if (fadeIsFinish()) {
				fadeEntry(9, 1000, lbl_8042CF28);
				marioReset();
				marioEntry();
				psndBGMOn(0x200, (char*)0x93);
				s_gameOverEvt = evtEntryType(main_gameover_script, 0, 0, 0);
				work->state++;
			}
			break;

		case 5:
			if (!evtCheckID(s_gameOverEvt->eventId) || (keyGetButtonTrg(0) & (PAD_BUTTON_START | PAD_BUTTON_A))) {
				fadeEntry(36, 0, lbl_8042CF28);
				fn_800DB9A8(0x200, 4000, 0);
				work->state++;
			}
			break;

		case 6:
			if (fadeIsFinish()) {
				fn_80007740();
				_unload("sys_00", 0, 0);
				strcpy(gp->beroEnterName, "");
				strcpy(gp->currentMapName, "");
				strcpy(gp->currentAreaName, "");
				statusWinDispOn();
				cam = camGetPtr(CAMERA_2D);
				cam->flags &= ~0x600;
				if (gp->flags & 0x1000) {
					seqSetSeq(SEQ_E3, NULL, NULL);
				}
				else {
					seqSetSeq(SEQ_TITLE, NULL, NULL);
				}
			}
			break;

		default:
			break;
	}
}
