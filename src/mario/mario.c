#include "mario/mario.h"
#include "mario/mario_motion.h"
#include "driver/animdrv.h"
#include "party/party_vivian.h"
#include "memory.h"
#include "mario/mario_cam.h"
#include <string.h>
#include <stdio.h>
#include <math.h>
#include <dolphin/pad.h>
#include "system.h"

//.data
const char* dotMarioPose[4][6] = {
	{"Z_1", "S_1", "W_1", "R_1", "U_1", "J_1"},
	{"Z_15", "S_2", "W_15", "R_15", "U_2", "J_2"},
	{"Z_17", "S_3", "W_17", "R_17", "U_3", "J_3"},
	{"Z_18", "S_4", "W_18", "R_18", "U_4", "J_4"}
};

//.bss
Vec blurPos[8];

//.sbss
MarioWork* mp;

extern const f32 lbl_8042CA8C;
extern const f32 lbl_8042CA88;
extern const f32 lbl_8042CA90;
extern const f32 lbl_8042CAA8;
extern const f32 lbl_8042CB00;
extern const f32 lbl_8042CB10;
extern const f32 lbl_8042CB40;
extern const f32 lbl_8042CB44;
extern const f32 lbl_8042CB48;
extern const f32 lbl_8042CB4C;
extern const f32 lbl_802CD420[];

//local prototypes
void marioDispBlurSub(s32* unused, s32 blurId);
const char* toRearPose(const char* name);
const char* toFrontPose(const char* name);

MarioWork* marioGetPtr(void) {
	return mp;
}

s32 marioGetColor(void) {
	return mp->colorId;
}

void marioSetCharMode(s32 mode) {

}

void marioSetFamicomMode(s32 mode) {

}

void marioSetSpec(void) {
	MarioWork* work = mp;
	if (work->characterId == 1) {
		(work->wPlayerCollisionBox).x = 20.0f;
		(work->wPlayerCollisionBox).y = 47.0f;
		work->mBaseWalkSpeed = 1.0f;
		work->mBaseDashSpeed = 2.0f;
	}
	else if (work->characterId == 2) {
		work->mBaseWalkSpeed = 1.0f;
		work->mBaseDashSpeed = 2.25f;
	}
	else {
		if (!(work->flags & 0x2000000)) {
			(work->wPlayerCollisionBox).x = 20.0f;
			(work->wPlayerCollisionBox).y = 37.0f;
		}
		else {
			(work->wPlayerCollisionBox).x = 10.0f;
			(work->wPlayerCollisionBox).y = 25.9f;
		}
		work->mBaseWalkSpeed = 1.0f;
		work->mBaseDashSpeed = 2.25f;
	}
}

static BOOL s_marioPoseInitialized = FALSE;

void marioEntry(void) {
	if (!mp) return;
	mp->flags |= 1;
	if (!s_marioPoseInitialized) {
#ifdef TARGET_PC
		printf("[marioEntry] Initializing Mario 3D pose 'c_mario'...\n");
		fflush(stdout);
#endif
		mp->paperAnimGroupId[0] = animPoseEntry("c_mario", 0);
		if (mp->paperAnimGroupId[0] >= 0) {
			animPoseSetAnim(mp->paperAnimGroupId[0], "M_S_1", TRUE);
			mp->animName = "M_S_1";
			s_marioPoseInitialized = TRUE;
		}
	}
	mp->scale = (Vec){ 1.0f, 1.0f, 1.0f };
	dispEntry(CAMERA_3D, 0.0f, 2, marioDisp, mp);
}

void marioItemGetDisable(void) {
	MarioWork* work = mp;
	work->flags |= 0x4000;
	work->field_0x7A = 0x14;
}

void marioItemGetChk(void) {
	MarioWork* work;
	s32 var;

	work = mp;
	var = mp->field_0x7A;
	if (var > 0) {
		var--;
		work->field_0x7A = (s16)var;
		if ((s16)var <= 0) {
			work->field_0x7A = 0;
			mp->flags &= ~0x4000u;
		}
	}
}

BOOL marioItemGetOk(void) {
	return (mp->flags & 0x4000) == 0;
}

BOOL marioCaseEventValidChk(void) {
	if (mp->currMotionId == MARIO_MOTION_PLANE) {
		return FALSE;
	}
	else {
		return vivianGetStatus() == 0;
	}
}

BOOL mario8005BB80(void) {
	if (mp->currMotionId == MARIO_MOTION_ROLL || mp->flags & 0x100000) {
		return FALSE;
	}
	return TRUE;
}

BOOL marioCheckMenuDisable(void) {
	return TRUE;
}

BOOL marioChkKey(void) {
	return TRUE;
}

BOOL marioChkCtrl(void) {
	return TRUE;
}

s32 mario8005B5A8(void) {
	return 0;
}

void marioFBattlePrepare(void) {

}

void marioFBattlePost(void) {

}

s32 marioCtrlOffChk(void) {
	return 0;
}

s32 marioKeyOffChk(void) {
	return 0;
}

s32 marioCtrlOff(void) {
	return 0;
}

s32 marioCtrlOn(void) {
	return 0;
}

s32 marioCtrlOff2(void) {
	return 0;
}

s32 marioCtrlOn2(void) {
	return 0;
}

s32 marioKeyOff(void) {
	MarioWork* mario = mp;
	mario->flags |= 8;
	mario->keyLock++;
	return mario->keyLock;
}

s32 marioKeyOn(void) {
	MarioWork* mario = mp;
	mario->keyLock--;
	if (mario->keyLock == 0) {
		mario->flags &= ~8;
	}
	return mario->keyLock;
}

s16 marioSetMutekiTime(s32 msec) {
	return 0;
}

BOOL marioBgmodeChk(void) {
	return mp ? ((mp->flags >> 25) & 1) : FALSE;
}

void marioBgmodeOn(void) {

}

void marioBgmodeOff(void) {

}

BOOL marioChkSts(s32 flags) {
	return FALSE;
}

void marioResetHitObj(const char* name) {

}

void marioReset(void) {
	MarioWork* mario = mp;
	MarioWork* currentMario;
	const Vec* resetVecs = (const Vec*)((const u8*)lbl_802CD420 + 0x88);

	if (mario->characterId == 1) {
		mario->wPlayerCollisionBox.x = lbl_8042CB10;
		mario->wPlayerCollisionBox.y = lbl_8042CB44;
		mario->mBaseWalkSpeed = lbl_8042CA90;
		mario->mBaseDashSpeed = lbl_8042CA8C;
	} else if (mario->characterId == 2) {
		mario->mBaseWalkSpeed = lbl_8042CA90;
		mario->mBaseDashSpeed = lbl_8042CB40;
	} else {
		if (!(mario->flags & MARIO_FLAG_BG_MODE)) {
			mario->wPlayerCollisionBox.x = lbl_8042CB10;
			mario->wPlayerCollisionBox.y = lbl_8042CB48;
		} else {
			mario->wPlayerCollisionBox.x = lbl_8042CB00;
			mario->wPlayerCollisionBox.y = lbl_8042CB4C;
		}
		mario->mBaseWalkSpeed = lbl_8042CA90;
		mario->mBaseDashSpeed = lbl_8042CB40;
	}

	mario->flags &= ~MARIO_FLAG_PAPER_MODE;
	mario->flags &= ~MARIO_FLAG_TUBE_MODE;

	currentMario = mp;
	if ((currentMario->dispFlags & 0x40000000) && currentMario->paperAnimName != NULL) {
		animPoseSetPaperAnimGroup(currentMario->paperAnimGroupId[0], NULL, FALSE);
		currentMario->paperAnimName = NULL;
		currentMario->dispFlags &= ~0x40000000;
		currentMario = mp;
		if (currentMario->paperPoseId >= 0) {
			animPaperPoseRelease(currentMario->paperPoseId);
			currentMario = mp;
			currentMario->paperPoseId = -1;
		}
	}

	currentMario = mp;
	if ((currentMario->dispFlags & 0x40000000) && !(currentMario->dispFlags & 2) &&
		!(currentMario->flags & MARIO_FLAG_IS_8_BIT) &&
		(currentMario->paperAnimName == NULL || strcmp(currentMario->paperAnimName, NULL) != 0))
	{
		currentMario->paperAnimName = NULL;
		currentMario->poseTime = 0;
		currentMario->trigFlags |= MARIO_TRIG_FLAG_CHG_PAPER_ANIM;
	}

	mario->dispFlags &= ~8;
	mario->dispFlags &= ~0x100;
	mario->field_0xBC = resetVecs[0];
	mario->field_0xB0 = resetVecs[1];
	mario->field_0x98 = resetVecs[2];
	mario->field_A4 = resetVecs[3];
	mario->baseSpeed = 0.0f;
	marioChgMot(MARIO_MOTION_STAY);
}

void marioOfsRotReset(void) {

}

void marioSoundInit(void) {

}

void marioPoseInit(void) {

}

void marioInit(void) {
	if (!mp) {
		mp = (MarioWork*)__memAlloc(HEAP_DEFAULT, sizeof(MarioWork));
	}
	memset(mp, 0, sizeof(MarioWork));
	mp->scale.x = 1.0f;
	mp->scale.y = 1.0f;
	mp->scale.z = 1.0f;
	mp->field_68 = 2.0f;
	marioResetCamShift();
	marioResetCamShiftRate();
	marioInitCamId();
	marioResetCamFollowRate();
	marioSetSpec();
}

void marioReInit(void) {
	marioInit();
}

#include "driver/hitdrv.h"
#include "driver/seqdrv.h"
#include "mario/mariost.h"

extern void pcDrawSolidQuad(f32 x1, f32 y1, f32 x2, f32 y2, GXColor color);
extern void pcDrawText(f32 x, f32 y, f32 scale, GXColor color, const char* str);

static f32 s_marioVelY = 0.0f;
static f32 s_marioGroundY = 0.0f;
static f32 s_marioWalkCycle = 0.0f;

static void marioHudDisp(CameraId camId, void* param) {
	CameraEntry* cam2d;
	MarioWork* wp = (MarioWork*)param;
	char buf[128];
	GXColor bgCol = { 0x10, 0x14, 0x20, 0xD8 };
	GXColor borderCol = { 0xE6, 0xB8, 0x00, 0xFF };
	GXColor textWhite = { 0xFF, 0xFF, 0xFF, 0xFF };
	GXColor textYellow = { 0xFF, 0xEA, 0x70, 0xFF };

	if (!wp || seqGetSeq() != SEQ_GAME) return;

	cam2d = camGetPtr(CAMERA_2D);
	if (!cam2d) return;

	GXSetAlphaUpdate(GX_TRUE);
	GXSetCullMode(GX_CULL_NONE);
	GXSetZCompLoc(GX_FALSE);
	GXSetAlphaCompare(GX_ALWAYS, 0, GX_AOP_AND, GX_ALWAYS, 0);
	GXSetBlendMode(GX_BM_BLEND, GX_BL_SRCALPHA, GX_BL_INVSRCALPHA, GX_LO_CLEAR);
	GXSetZMode(GX_FALSE, GX_ALWAYS, GX_FALSE);

	GXLoadPosMtxImm(cam2d->view, GX_PNMTX0);
	GXSetCurrentMtx(GX_PNMTX0);

	pcDrawSolidQuad(-290.0f, 224.0f, 290.0f, 174.0f, borderCol);
	pcDrawSolidQuad(-288.0f, 222.0f, 288.0f, 176.0f, bgCol);

	sprintf(buf, "PORT-LACANAIE (%s)  |  MARIO X:%.0f Y:%.0f Z:%.0f  |  PV 25/25",
	        gp && gp->currentMapName[0] ? gp->currentMapName : "gor_00",
	        wp->position.x, wp->position.y, wp->position.z);
	pcDrawText(-278.0f, 204.0f, 1.0f, textYellow, buf);
	pcDrawText(-278.0f, 184.0f, 0.95f, textWhite, "ZQSD / WASD / Fleches: Deplacer  |  Espace / Entree: Sauter");
}

void marioMain(void) {
	u32 btnTrg;
	s8 stickX, stickY;
	f32 speed, dx, dz;
	f32 groundY;
	f32 hitX, hitY, hitZ, hitDist;
	f32 hitNX, hitNY, hitNZ;
	HitEntry* hitGnd;
	BOOL isGrounded;
	CameraEntry* cam;

	if (!mp || !(mp->flags & 1)) return;

	// Lecture directe des touches de la manette 0 / clavier
	btnTrg = keyGetButtonTrg(0);
	stickX = keyGetStickX(0);
	stickY = keyGetStickY(0);

	speed = 0.8f;
	dx = ((f32)stickX / 127.0f) * speed;
	dz = -((f32)stickY / 127.0f) * speed;

	// Detection du sol reel via les polygones de collision de la map
	groundY = (gp && !strcmp(gp->currentMapName, "gor_00")) ? 2.5f : 0.0f;
	hitX = 0.0f; hitY = 0.0f; hitZ = 0.0f; hitDist = 2000.0f;
	hitNX = 0.0f; hitNY = 0.0f; hitNZ = 0.0f;
	hitGnd = hitCheckFilter(NULL, &hitX, &hitY, &hitZ, &hitDist, &hitNX, &hitNY, &hitNZ,
	                        mp->position.x + dx, mp->position.y + 20.0f, mp->position.z + dz,
	                        0.0f, -1.0f, 0.0f);
	if (hitGnd) {
		groundY = hitY;
	}
	s_marioGroundY = groundY;

	// Deplacement et orientation de Mario (avec limites de la place de Port-Lacanaie)
	if (fabsf(dx) > 0.02f || fabsf(dz) > 0.02f) {
		f32 nextX = mp->position.x + dx;
		f32 nextZ = mp->position.z + dz;
		if (nextX < -31.0f) nextX = -31.0f;
		if (nextX >  31.0f) nextX =  31.0f;
		if (nextZ < -54.0f) nextZ = -54.0f;
		if (nextZ >  15.0f) nextZ =  15.0f;
		mp->position.x = nextX;
		mp->position.z = nextZ;
		mp->targetYaw = atan2f(-dx, -dz) * 180.0f / 3.14159265f;
		s_marioWalkCycle += 0.35f;
		if (s_marioVelY == 0.0f && (!mp->animName || strcmp(mp->animName, "M_W_1") != 0)) {
			mp->animName = "M_W_1";
			if (mp->paperAnimGroupId[0] >= 0) {
				animPoseSetAnim(mp->paperAnimGroupId[0], "M_W_1", TRUE);
			}
		}
	} else {
		s_marioWalkCycle = 0.0f;
		if (s_marioVelY == 0.0f && (!mp->animName || strcmp(mp->animName, "M_S_1") != 0)) {
			mp->animName = "M_S_1";
			if (mp->paperAnimGroupId[0] >= 0) {
				animPoseSetAnim(mp->paperAnimGroupId[0], "M_S_1", TRUE);
			}
		}
	}

	isGrounded = (fabsf(mp->position.y - groundY) <= 0.6f && s_marioVelY <= 0.0f);

	// Saut (Bouton A / Espace / Entree)
	if ((btnTrg & PAD_BUTTON_A) && isGrounded) {
		s_marioVelY = 1.6f;
		mp->animName = "M_J_1B";
		if (mp->paperAnimGroupId[0] >= 0) {
			animPoseSetAnim(mp->paperAnimGroupId[0], "M_J_1B", FALSE);
		}
	}

	// Gravite et contact au sol (s_marioVelY separe de mp->field_64 pour ne pas deformer camMain!)
	mp->field_64 = 0.0f;
	if (!isGrounded || s_marioVelY != 0.0f) {
		mp->position.y += s_marioVelY;
		s_marioVelY -= 0.11f;
		if (mp->position.y <= groundY) {
			mp->position.y = groundY;
			s_marioVelY = 0.0f;
		}
	} else {
		mp->position.y = groundY;
		s_marioVelY = 0.0f;
	}

	// Mise a jour de la camera 3D (target, cameraAt ET matrice view!) pour suivre Mario en temps reel
	cam = camGetPtr(CAMERA_3D);
	if (cam) {
		cam->target.x += (mp->position.x - cam->target.x) * 0.18f;
		cam->target.y += (mp->position.y + 4.5f - cam->target.y) * 0.18f;
		cam->target.z += (mp->position.z - cam->target.z) * 0.18f;
		cam->cameraAt = cam->target;
		cam->cameraPos.x = cam->target.x;
		cam->cameraPos.y = cam->target.y + 12.0f;
		cam->cameraPos.z = cam->target.z + 32.0f;
		MTXLookAt(cam->view, &cam->cameraPos, &cam->cameraUp, &cam->cameraAt);
	}

	// Inscrire Mario pour le rendu 3D et le HUD 2D de la frame courante
	dispEntry(CAMERA_3D, 0.0f, 2, marioDisp, mp);
	if (seqGetSeq() == SEQ_GAME) {
		dispEntry(CAMERA_2D, 0.0f, 0, marioHudDisp, mp);
	}
}

void marioCtrlOff2Main(void) {

}

void marioMove(void) {

}

void marioMoveMain(void) {

}

BOOL marioChkPushAnime(void) {
	MarioWork* wp = mp;
	BOOL valid = FALSE;
	if (!strcmp(wp->animName, "M_O_1") || !strcmp(wp->animName, "M_O_2") || !strcmp(wp->animName, "M_O_2R")) {
		valid = TRUE;
	}
	return valid;
}

s32 marioAnimeId(void) {
	s32 flags = mp->dispFlags;
	if (flags & 0x10000000) {
		return 2;
	}
	else {
		return flags < 0; //TODO: fix
	}
}

void mario80058374(void) {
	MarioWork* wp = mp;
	MarioMotion mot = wp->currMotionId;
	s32 flags;

	if (mot != MARIO_MOTION_SHIP && mot != MARIO_MOTION_ROLL && mot != MARIO_MOTION_PLANE && mot != MARIO_MOTION_JABARA && mot != MARIO_MOTION_YOSHI) {
		flags = wp->dispFlags;
		if (!(flags & 0x1000000)) {
			marioChgPose("M_D_1"); //inline
		}
	}
}

void marioChgEvtPose(const char* name) {
	MarioWork* wp = mp;
	const char* animName;

	if (!(mp->dispFlags & 2) && !(mp->flags & 0x80000000)) {
		animName = mp->animName;
		if (!animName || strcmp(animName, name)) {
			wp->animName = name;
			wp->dispFlags &= ~0x20000000;
			wp->dispFlags |= 0x10000000;
			wp->trigFlags |= 0x1000;
		}
	}
}

const char* toDotMarioPose(const char* name) {
	const char* dot = NULL;

	if (!strcmp("M_Z_1", name)) {
		dot = dotMarioPose[mp->colorId][0];
	}
	if (!strcmp("M_S_1", name)) {
		dot = dotMarioPose[mp->colorId][1];
	}
	if (!strcmp("M_W_1", name)) {
		dot = dotMarioPose[mp->colorId][2];
	}
	if (!strcmp("M_R_1", name)) {
		dot = dotMarioPose[mp->colorId][3];
	}
	if (!strcmp("M_J_1B", name)) {
		dot = dotMarioPose[mp->colorId][5];
	}
	if (!strcmp("M_J_1C", name)) {
		dot = dotMarioPose[mp->colorId][5];
	}

	return dot;
}

void marioChgPose(const char* name) {
	MarioWork* wp = mp;
	const char *animName, *dot;

	if (!(wp->dispFlags & 2)) {
		animName = mp->animName;
		if (!animName || strcmp(animName, name)) {
			if (!(mp->flags & 0x80000000)) { //TODO: flip {}?
				dot = toDotMarioPose(name); //inline, TODO
				if (dot) {
					wp->animName = dot;
					wp->poseTime = 0;
					wp->dispFlags &= ~0x30000000;
					wp->trigFlags |= 0x1000;
				}
			}
			else {
				wp->animName = name;
				wp->poseTime = 0;
				wp->dispFlags &= ~0x30000000;
				wp->trigFlags |= 0x1000;
			}
		}
	}
}

void marioChgPoseTime(const char* name, s16 frames) {
	MarioWork* wp = mp;
	const char* animName;

	if (!(mp->dispFlags & 2) && !(mp->flags & 0x80000000)) {
		animName = mp->animName;
		if (!animName || strcmp(animName, name)) {
			wp->animName = name;
			wp->poseTime = frames;
			wp->dispFlags &= ~0x30000000;
			wp->trigFlags |= 0x1000;
		}
	}
}

void marioChgPaper(const char* name) {
	MarioWork* wp = mp;
	s32 flags = wp->dispFlags;
	const char* animName;

	if (flags & 0x40000000 && !(flags & 2) && !(mp->flags & 0x80000000)) {
		animName = mp->paperAnimName;
		if (!animName || strcmp(animName, name)) { //if different, do stuff
			wp->paperAnimName = name;
			wp->poseTime = 0;
			wp->trigFlags |= 0x4000;
		}
	}
}

void marioSetPaperAnimeLocalTime(s16 frames) { //f32 cast reuse, flags check
	MarioWork* wp = mp;
	s32 group;

	animPoseSetLocalTime(wp->paperPoseId, (f32)frames);
	group = marioAnimeId(); //inlined
	animPoseSetLocalTime(wp->paperAnimGroupId[group], (f32)frames);
}

//TODO: move below marioPaperOff when -inline deferred is on
inline void marioPaperRelease(void) {
	MarioWork* wp = mp;
	s32 poseId;

	poseId = wp->paperPoseId;
	if (poseId >= 0) {
		animPaperPoseRelease(poseId);
		wp->paperPoseId = -1;
	}
}

BOOL marioPaperOn(const char* name) { //almost 1:1, some regalloc
	MarioWork* wp = mp;
	BOOL flag;

	marioPaperRelease();
	wp->dispFlags |= 0x40000000; //paper on
	wp->paperPoseId = animPaperPoseEntry(name, 2);
	flag = FALSE;
	if (   !strcmp(name, "p_roll")
		|| !strcmp(name, "p_plane")
		|| !strcmp(name, "p_hune")
		|| !strcmp(name, "p_jyabara")
		|| !strcmp(name, "p_dokan_x")
		|| !strcmp(name, "p_dokan_y"))
	{
		flag = TRUE;
	}
	animPoseSetPaperAnimGroup(wp->paperAnimGroupId[0], name, !flag);
	return TRUE;
}

void marioPaperOff(void) { //1:1
	MarioWork* wp = mp;

	if (wp->dispFlags & 0x40000000) {
		if (wp->paperAnimName) {
			animPoseSetPaperAnimGroup(mp->paperAnimGroupId[0], NULL, FALSE);
			wp->paperAnimName = 0;
			wp->dispFlags &= ~0x40000000;
			marioPaperRelease();
		}
	}
}

void marioPaperLightOn(void) { //1:1
	animPoseSetMaterialLightFlagOff(mp->paperAnimGroupId[0], 1);
}

void marioPaperLightOff(void) { //1:1
	animPoseSetMaterialLightFlagOn(mp->paperAnimGroupId[0], 1);
}

void marioGetScreenPos(Vec position, f32* x, f32* y, f32* z) { //1:1
	CameraEntry* camera;
	f32 proj[7];
	f32 view[6];

	camera = camGetPtr(CAMERA_3D);
	GXSetProjection(camera->projection, camera->projType);
	GXGetProjectionv(proj);
	GXGetViewportv(view);
	GXProject(position.x, position.y, position.z, camera->view, proj, view, x, y, z);
}

BOOL marioChkInScreen(s32 x, s32 y) { //1:1
	BOOL valid = FALSE;
	if (x >= 0 && x <= 600 && y >= 0 && y <= 535) {
		valid = TRUE;
	}
	return valid != FALSE; //why
}

f32 marioGetScale(void) { //1:1
	#ifndef TARGET_PC
	return mp->flags & 0x2000000 ? lbl_8042CA88 : lbl_8042CA8C;
	#else
	return mp->flags & 0x2000000 ? 1.2f : 2.0f;
	#endif
}

void marioMakeDispDir(void) {

}

void marioRearAnime(void) {

}

void marioPreDisp(void) {

}

void marioDispBlurSub(s32* unused, s32 blurId) {

}

void marioDispBlur(CameraId camId, void* param) {
	MarioWork* wp = param; //cast to correct type

}

static void drawMarioBox3D(f32 x0, f32 y0, f32 z0, f32 x1, f32 y1, f32 z1, GXColor col) {
	GXSetTevColor(GX_TEVREG0, col);
	GXBegin(GX_QUADS, GX_VTXFMT0, 24);
	// Front (+Z)
	GXPosition3f32(x0, y1, z1); GXPosition3f32(x1, y1, z1); GXPosition3f32(x1, y0, z1); GXPosition3f32(x0, y0, z1);
	// Back (-Z)
	GXPosition3f32(x1, y1, z0); GXPosition3f32(x0, y1, z0); GXPosition3f32(x0, y0, z0); GXPosition3f32(x1, y0, z0);
	// Left (-X)
	GXPosition3f32(x0, y1, z0); GXPosition3f32(x0, y1, z1); GXPosition3f32(x0, y0, z1); GXPosition3f32(x0, y0, z0);
	// Right (+X)
	GXPosition3f32(x1, y1, z1); GXPosition3f32(x1, y1, z0); GXPosition3f32(x1, y0, z0); GXPosition3f32(x1, y0, z1);
	// Top (+Y)
	GXPosition3f32(x0, y1, z0); GXPosition3f32(x1, y1, z0); GXPosition3f32(x1, y1, z1); GXPosition3f32(x0, y1, z1);
	// Bottom (-Y)
	GXPosition3f32(x0, y0, z1); GXPosition3f32(x1, y0, z1); GXPosition3f32(x1, y0, z0); GXPosition3f32(x0, y0, z0);
	GXEnd();
}

void marioDisp(CameraId camId, void* param) {
	Mtx trans, rotY, scaleMtx, modelMtx, mv;
	CameraEntry* cam;
	f32 legSwing;
	GXColor colShadow  = { 0x00, 0x00, 0x00, 0x90 };
	GXColor colShoes   = { 0x6B, 0x35, 0x10, 0xFF };
	GXColor colOverall = { 0x18, 0x48, 0xD8, 0xFF };
	GXColor colButtons = { 0xFF, 0xE8, 0x20, 0xFF };
	GXColor colShirt   = { 0xE6, 0x18, 0x18, 0xFF };
	GXColor colSkin    = { 0xFF, 0xCC, 0x99, 0xFF };
	GXColor colMustach = { 0x3A, 0x1C, 0x08, 0xFF };
	GXColor colEyes    = { 0x10, 0x10, 0x10, 0xFF };
	GXColor colGloves  = { 0xFF, 0xFF, 0xFF, 0xFF };
	MarioWork* wp = (MarioWork*)param;
	if (!wp || !(wp->flags & 1)) return;
	MTXTrans(trans, wp->position.x, wp->position.y, wp->position.z);
	MTXRotRad(rotY, 'y', wp->targetYaw * 3.14159265f / 180.0f);
	MTXScale(scaleMtx, wp->scale.x, wp->scale.y, wp->scale.z);
	MTXConcat(trans, rotY, modelMtx);
	MTXConcat(modelMtx, scaleMtx, modelMtx);

	if (wp->paperAnimGroupId[0] >= 0) {
		animPoseMain(wp->paperAnimGroupId[0]);
		animPoseDrawMtx(wp->paperAnimGroupId[0], modelMtx, 1, wp->targetYaw, 1.0f);
		animPoseDrawMtx(wp->paperAnimGroupId[0], modelMtx, 2, wp->targetYaw, 1.0f);
		animPoseDrawMtx(wp->paperAnimGroupId[0], modelMtx, 3, wp->targetYaw, 1.0f);
	}

	// Rendu 3D direct de Mario pour affichage immediat a l'ecran
	cam = camGetPtr(camId);
	if (!cam) return;

	GXSetAlphaUpdate(GX_TRUE);
	GXSetCullMode(GX_CULL_NONE);
	GXSetZCompLoc(GX_TRUE);
	GXSetAlphaCompare(GX_ALWAYS, 0, GX_AOP_AND, GX_ALWAYS, 0);
	GXSetBlendMode(GX_BM_BLEND, GX_BL_SRCALPHA, GX_BL_INVSRCALPHA, GX_LO_CLEAR);
	GXSetZMode(GX_TRUE, GX_LEQUAL, GX_TRUE);

	GXSetNumChans(0);
	GXSetNumTexGens(0);
	GXSetNumTevStages(1);
	GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD_NULL, GX_TEXMAP_NULL, GX_COLOR_NULL);
	GXSetTevColorOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
	GXSetTevAlphaOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
	GXSetTevColorIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_ZERO, GX_CC_ZERO, GX_CC_C0);
	GXSetTevAlphaIn(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_A0);

	GXClearVtxDesc();
	GXSetVtxDesc(GX_VA_POS, GX_DIRECT);
	GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_POS, GX_POS_XYZ, GX_F32, 0);

	// 1. Ombre au sol sous Mario
	MTXTrans(trans, wp->position.x, s_marioGroundY + 0.08f, wp->position.z);
	MTXConcat(cam->view, trans, mv);
	GXLoadPosMtxImm(mv, GX_PNMTX0);
	GXSetCurrentMtx(GX_PNMTX0);
	drawMarioBox3D(-1.3f, 0.0f, -1.3f, 1.3f, 0.08f, 1.3f, colShadow);

	// 2. Corps de Mario (oriente selon targetYaw, mis a l'echelle 0.16 des unites map)
	MTXScale(scaleMtx, wp->scale.x * 0.16f, wp->scale.y * 0.16f, wp->scale.z * 0.16f);
	MTXConcat(trans, rotY, modelMtx);
	MTXTrans(trans, wp->position.x, wp->position.y, wp->position.z);
	MTXConcat(trans, rotY, modelMtx);
	MTXConcat(modelMtx, scaleMtx, modelMtx);
	MTXConcat(cam->view, modelMtx, mv);
	GXLoadPosMtxImm(mv, GX_PNMTX0);
	GXSetCurrentMtx(GX_PNMTX0);

	legSwing = sinf(s_marioWalkCycle) * 4.0f;

	// Chaussures marron
	drawMarioBox3D(-7.5f, 0.0f, -4.5f + legSwing, -1.5f, 5.5f, 4.5f + legSwing, colShoes);
	drawMarioBox3D( 1.5f, 0.0f, -4.5f - legSwing,  7.5f, 5.5f, 4.5f - legSwing, colShoes);

	// Salopette bleue
	drawMarioBox3D(-7.0f, 5.5f, -3.5f, 7.0f, 16.5f, 3.5f, colOverall);

	// Chemise rouge et bretelles
	drawMarioBox3D(-7.5f, 13.0f, -3.2f, 7.5f, 22.0f, 3.2f, colShirt);
	drawMarioBox3D(-5.0f, 14.0f, -3.8f, -2.0f, 22.2f, 3.8f, colOverall);
	drawMarioBox3D( 2.0f, 14.0f, -3.8f,  5.0f, 22.2f, 3.8f, colOverall);

	// Boutons jaunes de la salopette (devant = -Z dans le repere de Mario)
	drawMarioBox3D(-4.5f, 14.5f, -4.2f, -2.5f, 16.5f, -3.5f, colButtons);
	drawMarioBox3D( 2.5f, 14.5f, -4.2f,  4.5f, 16.5f, -3.5f, colButtons);

	// Bras rouges et gants blancs
	drawMarioBox3D(-12.0f, 13.5f, -2.5f - legSwing * 0.6f, -7.5f, 21.0f, 2.5f - legSwing * 0.6f, colShirt);
	drawMarioBox3D(  7.5f, 13.5f, -2.5f + legSwing * 0.6f, 12.0f, 21.0f, 2.5f + legSwing * 0.6f, colShirt);
	drawMarioBox3D(-12.5f,  9.5f, -3.0f - legSwing * 0.6f, -7.5f, 14.0f, 3.0f - legSwing * 0.6f, colGloves);
	drawMarioBox3D(  7.5f,  9.5f, -3.0f + legSwing * 0.6f, 12.5f, 14.0f, 3.0f + legSwing * 0.6f, colGloves);

	// Tete couleur chair + nez + moustache + yeux (devant = -Z)
	drawMarioBox3D(-7.5f, 22.0f, -4.5f, 7.5f, 33.5f, 4.5f, colSkin);
	drawMarioBox3D(-2.5f, 25.5f, -7.0f, 2.5f, 29.0f, -4.2f, colSkin);
	drawMarioBox3D(-5.5f, 23.5f, -5.5f, 5.5f, 25.8f, -4.0f, colMustach);
	drawMarioBox3D(-4.5f, 28.5f, -5.0f, -2.2f, 31.5f, -4.2f, colEyes);
	drawMarioBox3D( 2.2f, 28.5f, -5.0f,  4.5f, 31.5f, -4.2f, colEyes);

	// Casquette rouge + visiere + embleme blanc
	drawMarioBox3D(-8.5f, 32.0f, -5.5f, 8.5f, 38.5f, 5.5f, colShirt);
	drawMarioBox3D(-8.0f, 32.0f, -9.5f, 8.0f, 34.5f, -4.5f, colShirt);
	drawMarioBox3D(-3.0f, 34.5f, -6.0f, 3.0f, 37.5f, -5.2f, colGloves);
}

const char* toRearPose(const char* name) { //1:1
	const char* rear = NULL;
	if (mp->characterId == 1 && !(mp->peachFlags & 0x2000)) {
		if (!strcmp(name, "P_Z_1")) {
			rear = "P_Z_1R";
		}
		else if (!strcmp(name, "P_S_1")) {
			rear = "P_S_1R";
		}
		else if (!strcmp(name, "P_W_1")) {
			rear = "P_W_1R";
		}
		else if (!strcmp(name, "P_R_1")) {
			rear = "P_R_1R";
		}
		else if (!strcmp(name, "P_T_1")) {
			rear = "P_T_1R";
		}
		else if (!strcmp(name, "P_Z_2")) {
			rear = "P_Z_2R";
		}
		else if (!strcmp(name, "P_S_2")) {
			rear = "P_S_2R";
		}
		else if (!strcmp(name, "P_W_2")) {
			rear = "P_W_2R";
		}
		else if (!strcmp(name, "P_R_2")) {
			rear = "P_R_2R";
		}
		else if (!strcmp(name, "P_T_2")) {
			rear = "P_T_2R";
		}
		else if (!strcmp(name, "P_Z_3")) {
			rear = "P_Z_3R";
		}
		else if (!strcmp(name, "P_S_3")) {
			rear = "P_S_3R";
		}
		else if (!strcmp(name, "P_W_3")) {
			rear = "P_W_3R";
		}
		else if (!strcmp(name, "P_R_3")) {
			rear = "P_R_3R";
		}
		else if (!strcmp(name, "P_T_3")) {
			rear = "P_T_3R";
		}
		else if (!strcmp(name, "P_O_Z_1")) {
			rear = "P_O_Z_1R";
		}
		else if (!strcmp(name, "P_O_S_1")) {
			rear = "P_O_S_1R";
		}
		else if (!strcmp(name, "P_O_F_1")) {
			rear = "P_O_F_1R";
		}
		else if (!strcmp(name, "P_O_R_1")) {
			rear = "P_O_R_1R";
		}
		else if (!strcmp(name, "P_O_N_1")) {
			rear = "P_O_N_1R";
		}
		else if (!strcmp(name, "P_O_S_2")) {
			rear = "P_O_S_2R";
		}
		else if (!strcmp(name, "P_O_F_2")) {
			rear = "P_O_F_2R";
		}
		else if (!strcmp(name, "P_O_R_2")) {
			rear = "P_O_R_2R";
		}
		else if (!strcmp(name, "P_O_N_2")) {
			rear = "P_O_N_2R";
		}
		else if (!strcmp(name, "P_O_D_1")) {
			rear = "P_O_D_1R";
		}
		else if (!strcmp(name, "P_O_D_2")) {
			rear = "P_O_D_2R";
		}
		else if (!strcmp(name, "P_A_1")) {
			rear = "P_A_1R";
		}
		return rear;
	}
	else {
		if (!strcmp(name, "M_Z_1")) {
			rear = "M_Z_1R";
		}
		else if (!strcmp(name, "M_S_1")) {
			rear = "M_S_1R";
		}
		else if (!strcmp(name, "M_W_1")) {
			rear = "M_W_1R";
		}
		else if (!strcmp(name, "M_R_1")) {
			rear = "M_R_1R";
		}
		else if (!strcmp(name, "M_J_1B")) {
			rear = "M_J_1BR";
		}
		else if (!strcmp(name, "M_J_1C")) {
			rear = "M_J_1CR";
		}
		else if (!strcmp(name, "M_H_1")) {
			rear = "M_H_1R";
		}
		else if (!strcmp(name, "M_H_2")) {
			rear = "M_H_2R";
		}
		else if (!strcmp(name, "M_H_4")) {
			rear = "M_H_4R";
		}
		else if (!strcmp(name, "M_H_5")) {
			rear = "M_H_5R";
		}
		else if (!strcmp(name, "M_H_7")) {
			rear = "M_H_7R";
		}
		else if (!strcmp(name, "M_H_8")) {
			rear = "M_H_8R";
		}
		else if (!strcmp(name, "M_O_2")) {
			rear = "M_O_1R";
		}
		else if (!strcmp(name, "M_I_2")) {
			rear = "M_I_2R";
		}
		else if (!strcmp(name, "M_U_2")) {
			rear = "M_U_2R";
		}
		else if (!strcmp(name, "M_C_1")) {
			rear = "M_C_1R";
		}
		return rear;
	}
}

const char* toFrontPose(const char* name) { //1:1
	const char* front = NULL;
	if (mp->characterId == 1 && !(mp->peachFlags & 0x2000)) {
		if (!strcmp(name, "P_Z_1R")) {
			front = "P_Z_1";
		}
		else if (!strcmp(name, "P_S_1R")) {
			front = "P_S_1";
		}
		else if (!strcmp(name, "P_W_1R")) {
			front = "P_W_1";
		}
		else if (!strcmp(name, "P_R_1R")) {
			front = "P_R_1";
		}
		else if (!strcmp(name, "P_T_1R")) {
			front = "P_T_1";
		}
		else if (!strcmp(name, "P_Z_2R")) {
			front = "P_Z_2";
		}
		else if (!strcmp(name, "P_S_2R")) {
			front = "P_S_2";
		}
		else if (!strcmp(name, "P_W_2R")) {
			front = "P_W_2";
		}
		else if (!strcmp(name, "P_R_2R")) {
			front = "P_R_2";
		}
		else if (!strcmp(name, "P_T_2R")) {
			front = "P_T_2";
		}
		else if (!strcmp(name, "P_Z_3R")) {
			front = "P_Z_3";
		}
		else if (!strcmp(name, "P_S_3R")) {
			front = "P_S_3";
		}
		else if (!strcmp(name, "P_W_3R")) {
			front = "P_W_3";
		}
		else if (!strcmp(name, "P_R_3R")) {
			front = "P_R_3";
		}
		else if (!strcmp(name, "P_T_3R")) {
			front = "P_T_3";
		}
		else if (!strcmp(name, "P_O_Z_1R")) {
			front = "P_O_Z_1";
		}
		else if (!strcmp(name, "P_O_S_1R")) {
			front = "P_O_S_1";
		}
		else if (!strcmp(name, "P_O_F_1R")) {
			front = "P_O_F_1";
		}
		else if (!strcmp(name, "P_O_R_1R")) {
			front = "P_O_R_1";
		}
		else if (!strcmp(name, "P_O_N_1R")) {
			front = "P_O_N_1";
		}
		else if (!strcmp(name, "P_O_S_2R")) {
			front = "P_O_S_2";
		}
		else if (!strcmp(name, "P_O_F_2R")) {
			front = "P_O_F_2";
		}
		else if (!strcmp(name, "P_O_R_2R")) {
			front = "P_O_R_2";
		}
		else if (!strcmp(name, "P_O_N_2R")) {
			front = "P_O_N_2";
		}
		else if (!strcmp(name, "P_O_D_1R")) {
			front = "P_O_D_1";
		}
		else if (!strcmp(name, "P_O_D_2R")) {
			front = "P_O_D_2";
		}
		else if (!strcmp(name, "P_A_1R")) {
			front = "P_A_1";
		}
		return front;
	}
	else {
		if (!strcmp(name, "M_Z_1R")) {
			front = "M_Z_1";
		}
		else if (!strcmp(name, "M_S_1R")) {
			front = "M_S_1";
		}
		else if (!strcmp(name, "M_W_1R")) {
			front = "M_W_1";
		}
		else if (!strcmp(name, "M_R_1R")) {
			front = "M_R_1";
		}
		else if (!strcmp(name, "M_J_1BR")) {
			front = "M_J_1B";
		}
		else if (!strcmp(name, "M_J_1CR")) {
			front = "M_J_1C";
		}
		else if (!strcmp(name, "M_H_1R")) {
			front = "M_H_1";
		}
		else if (!strcmp(name, "M_H_2R")) {
			front = "M_H_2";
		}
		else if (!strcmp(name, "M_H_4R")) {
			front = "M_H_4";
		}
		else if (!strcmp(name, "M_H_5R")) {
			front = "M_H_5";
		}
		else if (!strcmp(name, "M_H_7R")) {
			front = "M_H_7";
		}
		else if (!strcmp(name, "M_H_8R")) {
			front = "M_H_8";
		}
		else if (!strcmp(name, "M_O_2R")) {
			front = "M_O_1";
		}
		else if (!strcmp(name, "M_I_2R")) {
			front = "M_I_2";
		}
		else if (!strcmp(name, "M_U_2R")) {
			front = "M_U_2";
		}
		else if (!strcmp(name, "M_C_1R")) {
			front = "M_C_1";
		}
		return front;
	}
}
