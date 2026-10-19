#include "battle/battle_break_slot.h"
#include "battle/battle.h"

#pragma dont_inline on
void* BattleBreakSlotGetPtr(void) {
	return (u8*)_battleWorkPointer + 0x1616C;
}
#pragma dont_inline reset

#pragma dont_inline on
void* BattleBreakSlotReelGetPtr(s32 index) {
	return (u8*)BattleBreakSlotGetPtr() + 0x18 + index * 0x5C;
}
#pragma dont_inline reset

s32 BattleBreakSlot_GetBreakTurn(void) {
	return *(s32*)((u8*)BattleBreakSlotGetPtr() + 0x10);
}

void BattleBreakSlot_DecBreakTurn(void) {
	s32* turns = (s32*)((u8*)BattleBreakSlotGetPtr() + 0x10);
	*turns -= 1;
	if (*turns < 0) {
		*turns = 0;
	}
}

void BattleBreakSlot_HideReel(void) {
	u8* workPtr = BattleBreakSlotGetPtr();
	u8* reel = BattleBreakSlotReelGetPtr(0);
	s32 i = 3;
	do {
		*(u32*)(reel + 4) |= 2;
		reel += 0x5C;
	} while (--i);
	*(s32*)(workPtr + 0xC) = 0;
}

void BattleBreakSlot_Start(void) {
	u8* workPtr = BattleBreakSlotGetPtr();
	u8* reel;
	s32 i;
	if (*(s32*)(workPtr + 0xC) < 3) {
		return;
	}
	*(s32*)(workPtr + 0xC) = 0;
	*(s32*)(workPtr + 4) = 2;
	reel = BattleBreakSlotReelGetPtr(0);
	i = 3;
	do {
		*(s32*)(reel + 8) = 4;
		*(s32*)(reel + 0xC) = 0;
		reel += 0x5C;
	} while (--i);
}

void BattleBreakSlot_Init(void) {

}

void BattleBreakSlot_Main(void) {

}

void BattleBreakSlot_End(void) {
	//stubbed in retail
}
