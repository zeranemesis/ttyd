#include "battle/battle_stage.h"
#include "battle/battle.h"
#include "memory.h"

extern const f32 lbl_8042F5FC;
extern const f32 lbl_8042F624;
extern const f32 lbl_8042F628;
extern const f32 lbl_8042F62C;
extern const f32 lbl_8042F630;
extern const u8 lbl_802FF418[];
extern void fn_80139B50(void);

//.sbss
static BattleSpotWork work;

//.sbss2
s32 lbl_804360F4;

//.sdata
static BattleSpotWork* inta_tex = &work;

//.sdata2
s32 batSpotInit_28 = -1;

// forward declarations
void batSpotDisp(CameraId camId, void* param);
BattleSpotEntry* batSpotEntry(void);
extern void fn_80139ACC(void);
extern void fn_800F9774(void* ptr);

BattleStage* BattleStageGetPtr(void) {
	return (BattleStage*)((u8*)_battleWorkPointer + 0x163FC);
}

void BattleStageInit(void) {
	BattleWork* wp = _battleWorkPointer;
	u8* stage = (u8*)wp + 0x163FC;
	u32 zero = 0;
	u32 one = 1;
	u8 byteZero = 0;
	u8 byteDefault = 0x8C;
	int i;

	memset(stage, 0, sizeof(BattleStage));
	*(u32*)(stage + 0) = zero;
	for (i = 0; i < 12; i++) {
		*(u32*)(stage + 4 + i * 0xE8) = zero;
		*(u32*)(stage + 0xE8 + i * 0xE8) = zero;
	}
	*(u32*)(stage + 0xAE4) = one;
	stage[0xAE8] = byteZero;
	stage[0xAE9] = byteZero;
	stage[0xAEA] = byteZero;
	stage[0xAEC] = byteZero;
	*(u32*)(stage + 0xAF0) = zero;
	*(u32*)(stage + 0xAF8) = zero;
	*(u32*)(stage + 0xB04) = one;
	stage[0xB08] = byteDefault;
	stage[0xB09] = byteDefault;
	stage[0xB0A] = byteZero;
	stage[0xB0C] = byteZero;
	*(u32*)(stage + 0xB10) = zero;
	*(u32*)(stage + 0xB18) = zero;
	*(f32*)(stage + 0xB24) = lbl_8042F624;
	*(f32*)(stage + 0xB28) = lbl_8042F628;
	*(f32*)(stage + 0xB2C) = lbl_8042F62C;
	*(f32*)(stage + 0xB30) = *(f32*)(stage + 0xB24);
	*(f32*)(stage + 0xB34) = lbl_8042F630;
	*(f32*)(stage + 0xB38) = lbl_8042F628;
	BattleStageObjectInit();
	fn_80139B50();
}

void BattleStageMain(void) {
}

void BattleStageEnd(void) {
	u8* stage = (u8*)_battleWorkPointer + 0x163FC;
	s32 i;
	s32 offset;
	u8* entry;
	void* ptr;

	fn_80139ACC();
	for (i = 0, offset = 0; i < 12; i++, offset += 0xE8) {
		entry = stage + 4 + offset;
		ptr = *(void**)(entry + 0xE4);
		if (ptr) {
			fn_800F9774(ptr);
			*(void**)(entry + 0xE4) = NULL;
		}
	}
}

BattleSpotWork* BatSpotGetPtr(void) {
	return inta_tex;
}

void batSpotInit(void) {
	int i;
	BattleSpotEntry* entry;

	inta_tex->count = 5;
	inta_tex->entries = __memAlloc(HEAP_BATTLE, sizeof(BattleSpotEntry) * inta_tex->count);
	memset(inta_tex->entries, 0, sizeof(BattleSpotEntry) * inta_tex->count);

	for (i = 0; i < 5; i++) {
		entry = batSpotEntry();
		entry->field_4 = (Vec){50.0f * i + -100.0f, 0.0f, -50.0f};
		entry->field_10 = (Vec){0.0f, 0.0f, 0.0f};
		entry->field_1C = (Vec){20.0f, 20.0f, 150.0f};
		entry->field_28 = batSpotInit_28;
	}
}

void batSpotMain() {
    s32 count = inta_tex->count;

    while (count-- > 0) {}
    if (inta_tex->skipRender > 0) {
		dispEntry(CAMERA_3D_IMAGE, lbl_8042F5FC, 0, batSpotDisp, NULL);
    }
}

void batSpotDisp(CameraId camId, void* param) {
}

//drawCone

// TODO: get this to not inline into batSpotInit
BattleSpotEntry* batSpotEntry(void) {
	int i;
	BattleSpotEntry* entry;
	volatile u32* defaults;
	volatile u32* words;
	u32 value0, value1;

	entry = inta_tex->entries;
	defaults = (volatile u32*)(lbl_802FF418 + 0x54);
	for (i = 0; i < inta_tex->count; i++, entry++) {
		if (!(entry->flags & 1)) {
			break;
		}
	}

	words = (volatile u32*)entry;
	value0 = defaults[0];
	value1 = defaults[1];
	words[1] = value0;
	words[2] = value1;
	value0 = defaults[2];
	words[3] = value0;
	value0 = defaults[3];
	value1 = defaults[4];
	words[7] = value0;
	words[8] = value1;
	value0 = defaults[5];
	words[9] = value0;
	value0 = defaults[6];
	value1 = defaults[7];
	words[4] = value0;
	words[5] = value1;
	value0 = defaults[8];
	words[6] = value0;
	entry->field_28 = lbl_804360F4;
	entry->flags |= 1;

	return entry;
}

void BattleStageOnLightInCommand(void) {}
void BattleStageOffLightInCommand(void) {}
void BattleStageOnLightFriendInCommand(void) {}
void BattleStageOffLightFriendInCommand(void) {}
void BattleStageOnLightEnemyInCommand(void) {}
void BattleStageOffLightEnemyInCommand(void) {}

#pragma dont_inline on
void BattleStageSetDark(u8* workPtr, s32 value, s32 duration, s32 mode) {
	workPtr[6] = workPtr[4];
	workPtr[8] = value;
	*(s32*)(workPtr + 0xC) = duration;
	*(s32*)(workPtr + 0x14) = duration;
	workPtr[0x1C] = mode;
	if (duration == 0) {
		workPtr[4] = value;
	}
}

void BattleStageSetDarkBase(u8* workPtr, s32 value, s32 duration, s32 mode) {
	workPtr[7] = workPtr[5];
	workPtr[9] = value;
	*(s32*)(workPtr + 0x10) = duration;
	*(s32*)(workPtr + 0x18) = duration;
	workPtr[0x1D] = mode;
	if (duration == 0) {
		workPtr[5] = value;
	}
}

void BattleStageSetStgDark(s32 value, s32 duration, s32 mode) {
	BattleWork* wp = _battleWorkPointer;
	BattleStageSetDark((u8*)wp + 0x16EE0, value, duration, mode);
}

void BattleStageSetStgDarkBase(s32 value, s32 duration, s32 mode) {
	BattleWork* wp = _battleWorkPointer;
	BattleStageSetDarkBase((u8*)wp + 0x16EE0, value, duration, mode);
}

void BattleStageReturnStgDarkBase(s32 value, s32 mode) {
	BattleWork* wp = _battleWorkPointer;
	u8* workPtr = (u8*)wp + 0x16EE0;
	BattleStageSetDark(workPtr, workPtr[5], value, mode);
}

void BattleStageSetAudDark(s32 value, s32 duration, s32 mode) {
	BattleWork* wp = _battleWorkPointer;
	BattleStageSetDark((u8*)wp + 0x16F00, value, duration, mode);
}

void BattleStageSetAudDarkBase(s32 value, s32 duration, s32 mode) {
	BattleWork* wp = _battleWorkPointer;
	BattleStageSetDarkBase((u8*)wp + 0x16F00, value, duration, mode);
}

void BattleStageReturnAudDarkBase(s32 value, s32 mode) {
	BattleWork* wp = _battleWorkPointer;
	u8* workPtr = (u8*)wp + 0x16F00;
	BattleStageSetDark(workPtr, workPtr[5], value, mode);
}
#pragma dont_inline reset

#pragma dont_inline on
void fn_80139ACC(void) {
	fn_800F9774(inta_tex->entries);
}
#pragma dont_inline reset
