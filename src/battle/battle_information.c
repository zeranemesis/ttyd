#include "battle/battle_information.h"
#include "battle/battle.h"

#pragma dont_inline on
void BattleInformationSetMode(FieldBattleInfo* info, u32 mode) {
	info->mode = mode;
}

void BattleInformationSetParty(FieldBattleInfo* info, BattleUnitType party) {
	info->party = party;
}

void BattleInformationSetFirstAttack(FieldBattleInfo* info, u32 partyId) {
	info->partyId = partyId;
}

void BattleInformationInit(FieldBattleInfo* info) {
	BattleInformationSetMode(info, 0);
	BattleInformationSetParty(info, 0);
	BattleInformationSetFirstAttack(info, 0);
	BattleInformationSetResult(info, 0);
	info->setup = NULL;
	*(u32*)info->field_0x14 = 0;
	info->field_0x18 = 0;
	info->pad_19 = 0;
	info->field_0x1a = 0;
	info->field_0x1b = 0;
}
#pragma dont_inline reset

void BattleInfomationSetBattleSetupInfo(FieldBattleInfo* info, NpcBattleInfo* setup) {
	info->setup = setup;
}

#pragma dont_inline on
void BattleInformationSetResult(FieldBattleInfo* info, u32 result) {
	info->result = result;
}
#pragma dont_inline reset

u32 BattleInformationGetResult(FieldBattleInfo* info) {
	return info->result;
}

USER_FUNC(_disable_restore_command_cursor) {
	u8* work = (u8*)_battleWorkPointer;
	*(u32*)(work + 0x1C70) = 0;
	work[0x1C74] = 0;
	return EVT_RETURN_DONE;
}
