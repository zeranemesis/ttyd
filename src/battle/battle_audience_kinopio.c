#include "battle/battle_audience.h"

void BattleAudienceCtrlProcessKinopioSingSub(s32 index);

#pragma dont_inline on
void BattleAudienceCtrlProcessKinopioSingSub(s32 index) {
	BattleAudienceMember* member = BattleAudienceGetPtr(index);

	if (member->flags & 0x100) {
		BattleAudienceCtrlProcessKinopioSingSub(index - 1);
	} else if (member->status != 0x11) {
		BattleAudience_ChangeStatus(index, 0xD);
		*(s32*)((u8*)member + 0xF4) = 3;
	}
}
#pragma dont_inline reset
