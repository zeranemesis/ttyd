#include "mario/mario_sbr.h"
#include "mario/mario.h"
#include "driver/hitdrv.h"
#include <math.h>

extern const Vec lbl_802CFA54;
extern const Vec lbl_802CFA48;
extern const f32 lbl_8042D52C;
extern const f32 lbl_8042D528;
extern const f32 lbl_8042D524;
extern const f32 lbl_8042D4FC;
extern const f32 lbl_8042D4F8;
extern const f32 lbl_8042D51C;
extern const f32 lbl_8042D518;
extern const f32 lbl_8042D514;
extern const f32 lbl_8042D510;
extern const f32 lbl_8042D50C;
extern const f32 lbl_8042D508;
extern const f32 lbl_8042D504;
extern const f32 lbl_8042D500;


// new in retail
void marioSetBottomlessResetPosition(f32 x, f32 y, f32 z) {
    MarioWork* wp = marioGetPtr();
    Vec position;

    position = (Vec){ x, y + 37.0f, z };
    wp->bottomlessResetPosition = position;
}

f32 revise360(f32 angle) {
	while (angle < 0.0f) {
		angle += 360.0f;
    }
	while (angle >= 360.0f) {
		angle -= 360.0f;
    }
    return angle;
}

f32 toMovedir(f32 angle) {
    MarioWork* wp = marioGetPtr();
    f32 view;
    f32 direction;
    s32 rounded;

    if (wp->flags & MARIO_FLAG_PAPER_MODE) {
        view = wp->unk1AC;
        if (view == lbl_8042D500 || view == lbl_8042D504) {
            view = 0.0f;
        } else if (view == lbl_8042D508 || view == lbl_8042D50C) {
            view = lbl_8042D4F8;
        } else {
            while (view < 0.0f) {
                view += 360.0f;
            }
            while (view >= 360.0f) {
                view -= 360.0f;
            }
        }
        direction = (lbl_8042D518 - view) + wp->targetYaw;
        direction = direction * lbl_8042D51C +
                    (direction >= 0.0f ? lbl_8042D510 : lbl_8042D514);
        rounded = (s32)direction;
        angle = (f32)rounded / lbl_8042D51C;
    }
    return revise360(angle);
}

void N_marioReloadMapOnBottomlessOff(void) {
	MarioWork* wp = marioGetPtr();
	*(u32*)((u8*)wp + 0x2E0) = 0;
}

void N_marioReloadMapOnBottomlessOn(void) {
	MarioWork* wp = marioGetPtr();
	*(u32*)((u8*)wp + 0x2E0) = 1;
}

void N_marioSetBottomlessRespawnPos(f32 x, f32 y, f32 z) {
	MarioWork* wp = marioGetPtr();
	Vec position = lbl_802CFA54;
	position.x = x;
	position.y = y;
	position.z = z;
	*(Vec*)((u8*)wp + 0x2E4) = position;
}

void N_marioSetBottomlessRespawnPosOnBeroEntry(f32 x, f32 y, f32 z) {
	MarioWork* wp = marioGetPtr();
	Vec position = lbl_802CFA48;
	position.x = x;
	position.y = lbl_8042D52C + y;
	position.z = z;
	*(Vec*)((u8*)wp + 0x2E4) = position;
}

void marioSetPeakYpos(void) {
	MarioWork* wp = marioGetPtr();
	*(f32*)((u8*)wp + 0x12C) = *(f32*)((u8*)wp + 0x90);
}

s32 marioChkPlanePlace(HitEntry* hit) {
	s32 attr = hitGetAttr(hit) & 0x38;
	return attr != 0;
}

void* marioGetTouchNpcPtr(void) {
	MarioWork* wp = marioGetPtr();
	return *(void**)((u8*)wp + 0x1D8);
}

void marioResetGravity(void) {
	f32 gravity = lbl_8042D4FC;
	MarioWork* wp = marioGetPtr();
	*(f32*)((u8*)wp + 0x228) = gravity;
}

f32 marioGetMoveRate(f32 angle) {
    MarioWork* wp = marioGetPtr();
    f32 factor = lbl_8042D524;
    f32 delta = angle - wp->targetYaw;
    f32 radians = (delta * factor) / lbl_8042D4F8;
    return lbl_8042D4FC + fabsf((f32)cos(radians)) * lbl_8042D528;
}
