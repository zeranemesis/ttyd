#include <dolphin/types.h>
#include <dolphin/mtx.h>
#include "mario/mariost.h"

extern GlobalWork* gp;

typedef struct CamShiftWork {
    f32 unk0;
    f32 unk4;
    f32 unk8;
    f32 unkC;
    f32 unk10;
    u32 unk14;
    OSTime lastTime;
    f32 unk20;
    f32 unk24;
    f32 unk28;
    f32 lastMarioX;
    f32 lastMarioZ;
} CamShiftWork;

static CamShiftWork s_camShiftWork;

void camShiftReset(void) {
    s_camShiftWork.unk0 = -2.0f;
    s_camShiftWork.unk4 = -2.0f;
    s_camShiftWork.unk8 = -2.0f;
    s_camShiftWork.unkC = -2.0f;
    s_camShiftWork.unk10 = -2.0f;
    s_camShiftWork.unk20 = 50.0f;
    s_camShiftWork.unk24 = -1.0f;
    s_camShiftWork.unk28 = 5000.0f;
    s_camShiftWork.lastMarioX = -2.0f;
    s_camShiftWork.lastMarioZ = -2.0f;
}

void camShiftPostMain(void* camera) {
    if (gp) {
        s_camShiftWork.lastTime = gp->renderTime;
    }
}

void camShiftMain(void* camera, void* mario, f32* bank) {
    if (bank) {
        *bank = 0.0f;
    }
}
