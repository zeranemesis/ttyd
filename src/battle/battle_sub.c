#include "battle/battle_sub.h"
#include "battle/battle_unit.h"

extern BattleWork* _battleWorkPointer;

void btlsubResetMoveColorLvAll(BattleWork* work) {
	BattleUnit* unit;
	s32 i;

	for (i = 0; i < 64; i++) {
		unit = BattleGetUnitPtr(work, i);
		if (unit != NULL) {
			unit->mMoveColorLv = 0xFF;
		}
	}
}

s32 BattleTransID(EventEntry* evt, s32 type) {
	BattleUnit* unit;

	switch (type) {
		case -3:
			unit = BattleGetMarioPtr(_battleWorkPointer);
			return unit->unitId;
		case -4:
			unit = BattleGetPartyPtr(_battleWorkPointer);
			if (unit != NULL) {
				return unit->unitId;
			}
			return -1;
		case -5:
			unit = BattleGetSystemPtr(_battleWorkPointer);
			return unit->unitId;
		case -6:
			return _battleWorkPointer->field_424;
		case -1:
			return -1;
		default:
			return type;
	}
}
