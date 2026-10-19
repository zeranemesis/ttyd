#include "driver/extdrv.h"
#include "driver/dispdrv.h"
#include "mario/mariost.h"
#include "memory.h"
#include <string.h>

#ifdef TARGET_PC
extern GlobalWork* gp;
#else
extern GlobalWork* wp_80422810;
#define gp wp_80422810
#endif

static ExtWork work[2];

#define extGetWork() (gp->inBattle ? &work[1] : &work[0])

void extInit(void) {
	ExtWork* wp = extGetWork();
	wp->poseNum = 0;
	wp->field_0x8 = 0;
}

void extMain(void) {

}

void extReset(void) {
	ExtWork* wp = extGetWork();
	if (wp->field_0x8) __memFree(2, wp->field_0x8);
	if (wp->field_0xC) __memFree(2, wp->field_0xC);
	if (wp->field_0x10) __memFree(2, wp->field_0x10);
	if (wp->field_0x14) __memFree(2, wp->field_0x14);
	if (wp->field_0x24) __memFree(2, wp->field_0x24);
	if (wp->field_0x28) __memFree(2, wp->field_0x28);
	if (wp->field_0x2C) __memFree(2, wp->field_0x2C);
	memset(wp, 0, sizeof(ExtWork));
}

s32 extGetPoseNum(void) {
	return extGetWork()->poseNum;
}

void* extGetPosePtr(void) {
	return (void*)extGetWork()->field_0x8;
}





void extMakeTexture(CameraId camId, void* param) {

}
