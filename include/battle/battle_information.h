#pragma once

#include <dolphin/types.h>
#include "battle/battle_unit.h"
#include "driver/npcdrv.h"

typedef struct FieldBattleInfo {
	u32 mode; //0x0, unused
	BattleUnitType party; //0x4
	u32 partyId; //0x8
	NpcBattleInfo* setup; //0xC
	u32 result; //0x10
	u8 field_0x14[4]; //0x14
	u8 field_0x18; //0x18
	u8 pad_19; //0x19
	u8 field_0x1a; //0x1a
	u8 field_0x1b; //0x1b
} FieldBattleInfo;

void BattleInformationSetMode(FieldBattleInfo* info, u32 mode);
void BattleInformationSetParty(FieldBattleInfo* info, BattleUnitType party);
void BattleInformationSetFirstAttack(FieldBattleInfo* info, u32 partyId);
void BattleInformationInit(FieldBattleInfo* info);
void BattleInfomationSetBattleSetupInfo(FieldBattleInfo* info, NpcBattleInfo* setup);
void BattleInformationSetResult(FieldBattleInfo* info, u32 result);
u32 BattleInformationGetResult(FieldBattleInfo* info);
