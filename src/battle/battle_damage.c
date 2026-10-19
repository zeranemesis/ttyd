#include "battle/battle_damage.h"
#include "battle/battle.h"
#include "battle/battle_unit.h"
#include "mario/mario_pouch.h"
#include <string.h>

void BattleInitCounterPreCheckWork(void* work) {
	memset(work, 0, 0x30);
	*(s32*)work = 1;
}

s32 _checkDamageCode_EmergencyRevival(void* weapon, s32* workFlags) {
	s32 damageCode = *(s32*)((u8*)weapon + 8);

	if (damageCode != 0xDE) {
		goto checkCodeRange;
	}
	if (pouchCheckItem(0x97) > 0) {
		*workFlags |= 0x800;
		return TRUE;
	}
	return FALSE;

checkCodeRange:
	if (damageCode >= 0xE0 && damageCode < 0xE7) {
		if (pouchCheckItem(0x97) > 0) {
			*workFlags |= 0x800;
			return TRUE;
		}
		return FALSE;
	}
	if (*(s32*)((u8*)weapon + 0x30C) == 0x97) {
		*workFlags |= 0x800;
		return TRUE;
	}
	return FALSE;
}

s32 BattleCheckDamage(BattleUnit* unit1, BattleUnit* unit2, BattleUnitPart* part, BattleWeapon* weapon, s32 flags) {

	if (!unit2) {
		return 18;
	}
	if (part) {
		unit2->currentTarget = part;
	}


}

s32 _getRegistStatus(BattleUnit* unit, s32 status) {
	u8* rates = *(u8**)((u8*)unit + 0x144);

	switch (status) {
	case 0: return rates[0];
	case 1: return rates[1];
	case 2: return rates[2];
	case 3: return rates[3];
	case 4: return rates[4];
	case 5: return rates[5];
	case 6: return rates[6];
	case 7: return rates[7];
	case 8: return rates[8];
	case 9: return rates[9];
	case 10: return rates[10];
	case 11: return rates[11];
	case 12: return rates[12];
	case 13: return rates[13];
	case 14: return rates[14];
	case 15: return rates[21];
	case 16: return rates[17];
	case 17: return rates[18];
	case 18: return rates[15];
	case 19: return rates[16];
	case 20: return rates[19];
	case 21: return rates[20];
	case 22:
	case 23:
	case 24:
	case 25:
	case 26:
	case 27:
	default: return 100;
	}
}