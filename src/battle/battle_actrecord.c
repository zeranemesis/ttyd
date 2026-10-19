#include "battle/battle_actrecord.h"

void BtlActRec_AddPoint(u8* points, s32 amount) {
	*points += amount;
	if (*points > 100) {
		*points = 100;
	}
}

void BtlActRec_AddCount(u8* count) {
	if (*count < 100) {
		(*count)++;
	}
}
