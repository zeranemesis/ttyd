#include "battle/battle_status_effect.h"
#include "battle/battle.h"
#include "battle/battle_unit.h"
#include "driver/effdrv.h"
#include "driver/icondrv.h"
#include <string.h>

// Status Type Priority Data
static s8 scale_data_80424ED0[] = {
	STATUS_CHARGE,
	STATUS_POISON,
	STATUS_SLEEP,
	STATUS_STOP,
	STATUS_DIZZY,
	STATUS_CONFUSE,
	STATUS_ELECTRIC,
	STATUS_DODGY,
	STATUS_BURN,
	STATUS_FREEZE,
	STATUS_HUGE,
	STATUS_TINY,
	STATUS_ATTACK_UP,
	STATUS_ATTACK_DOWN,
	STATUS_DEFENSE_UP,
	STATUS_DEFENSE_DOWN,
	STATUS_ALLERGIC,
	STATUS_INVISIBLE,
	STATUS_FAST,
	STATUS_SLOW,
	STATUS_PAYBACK,
	STATUS_HOLD_FAST,
	STATUS_HP_REGEN,
	STATUS_FP_REGEN,
	-1
};

//local prototypes
void BSE_Sleep(BattleUnit* unit);
void BSE_SleepDelete(BattleUnit* unit);
void BSE_Biribiri(BattleUnit* unit);
void BSE_BiribiriDelete(BattleUnit* unit);
void BSE_Fire(BattleUnit* unit);
void BSE_FireDelete(BattleUnit* unit);
void BSE_Freeze(BattleUnit* unit);
void BSE_FreezeDelete(BattleUnit* unit);
void BSE_Kagegakure(BattleUnit* unit);
void BSE_KagegakureDelete(BattleUnit* unit);
s32 _get_pri(s8 type);

void BattleStatusEffectInit(BattleUnit* unit) {
	; //stubbed in retail
}

#pragma dont_inline on
void BattleStatusEffectMain(BattleUnit* unit) {
	BSE_Sleep(unit);
	BSE_Biribiri(unit);
	BSE_Fire(unit);
	BSE_Freeze(unit);
	BSE_Kagegakure(unit);
}

void BattleStatusEffectDelete(BattleUnit* unit) {
	BSE_SleepDelete(unit);
	BSE_BiribiriDelete(unit);
	BSE_FireDelete(unit);
	BSE_FreezeDelete(unit);
	BSE_KagegakureDelete(unit);
}
#pragma dont_inline reset

void BSE_Sleep(BattleUnit* unit) {

}

void BSE_SleepDelete(BattleUnit* unit) {
	EffectEntry** effect = (EffectEntry**)((u8*)unit + 0x328);
	if (*effect != NULL) {
		effDelete(*effect);
		*effect = NULL;
	}
}

void BSE_Biribiri(BattleUnit* unit) {

}

void BSE_BiribiriDelete(BattleUnit* unit) {
	EffectEntry** effect = (EffectEntry**)((u8*)unit + 0x334);
	if (*effect != NULL) {
		effDelete(*effect);
		*effect = NULL;
	}
}

void BSE_Fire(BattleUnit* unit) {

}

void BSE_FireDelete(BattleUnit* unit) {
	EffectEntry** effect = (EffectEntry**)((u8*)unit + 0x32C);
	if (*effect != NULL) {
		effDelete(*effect);
		*effect = NULL;
	}
}

void BSE_Freeze(BattleUnit* unit) {

}

void BSE_FreezeDelete(BattleUnit* unit) {
	EffectEntry** effect = (EffectEntry**)((u8*)unit + 0x330);
	if (*effect != NULL) {
		effSoftDelete(*effect);
		*effect = NULL;
	}
}

void BSE_Kagegakure(BattleUnit* unit) {

}

void BSE_KagegakureDelete(BattleUnit* unit) {
	void** effect = (void**)((u8*)unit + 0x338);
	if (*effect != NULL) {
		iconDelete((char*)*effect + 0x18);
		*effect = NULL;
	}
}

void BSE_TurnFirstProcessEffectEntry(BattleUnit* unit, s16 active) {
	unit->mBurnDamageFlameState = 1;
	unit->mbBurnDamageFlameActive = active;
}

BOOL BSE_TurnFirstProcessEffectMain(BattleUnit* unit) {
	BattleUnitPart* part;

	part = BtlUnit_GetPartsPtr(unit, BtlUnit_GetBodyPartsId(unit));
	if (!unit->mBurnDamageFlameState) {
		return FALSE;
	}
	if (!unit->mbBurnDamageFlameActive) {
		return FALSE;
	}
	if (unit->mbBurnDamageFlameActive != 1) {
		return FALSE;
	}
	//TODO: finish
	return FALSE;
}

void BattleStatusChangeInfoWorkInit(BattleUnit* unit) {
	s32 i;
	u8* data = (u8*)unit + 0xAEC;
	for (i = 0; i < 0x48; i += 0xC) {
		u8* entry = data + i;
		entry[0] = -1;
		entry[1] = 0;
		entry[2] = 0;
		entry[3] = 0;
		*(s32*)(entry + 4) = 0;
	}
}

#pragma dont_inline on
BOOL BattleStatusChangeAnnouceMain_Unit(BattleUnit* unit) {
	(void)unit;
	return FALSE;
}
#pragma dont_inline reset

BOOL BattleStatusChangeAnnouceMain(BattleWork* work) {
	s32 i;
	BOOL result = FALSE;

	for (i = 0; i < 0x40; i++) {
		BattleUnit* unit = BattleGetUnitPtr(work, i);
		if (unit != NULL && BattleStatusChangeAnnouceMain_Unit(unit)) {
			result = TRUE;
		}
	}
	return result;
}

//should be StatusEffectType, TODO fix for s8
s32 _get_pri(s8 type) {
	s8* val;
	s32 priority = 24; //TODO: #define?

	for (val = scale_data_80424ED0;;) {
		if (*val == -1) {
			return -1; //reached end of list
		}
		if (*val == type) {
			return priority;
		}
		priority--;
		val++;
	}
}


void BattleStatusChangeMsgWorkInit(void) {
	s32 i;
	u8* work = (u8*)_battleWorkPointer + 0x18DA0;
	memset(work, 0, 0x258);
	for (i = 0; i < 0x150; i += 0xC) {
		work[i] = -1;
	}
}
