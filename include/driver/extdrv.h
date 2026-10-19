#pragma once

#include <dolphin/types.h>

typedef struct ExtWork {
	s32 poseNum; //0x0, TODO: poseId
	u8 field_0x4[4]; //0x4
	void* field_0x8;
	void* field_0xC;
	void* field_0x10;
	void* field_0x14;
	u8 field_0x18[0x24 - 0x18];
	void* field_0x24;
	void* field_0x28;
	void* field_0x2C;
	u8 field_0x30[0x70 - 0x30];
} ExtWork;

void extInit(void);
void extMain(void);
void extReset(void);
void extEntry(s32 arg0, s32 arg1, s32 arg2, s32 arg3, s32 arg4);
