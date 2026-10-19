#include "battle/battle_stage_object.h"
#include "battle/battle.h"
#include "system.h"
#include <string.h>

extern BattleWork* _battleWorkPointer;
extern void BattleAudience_Case_HaikeiSet(void);

#ifndef TARGET_PC
extern const f32 lbl_8042FAB8;
#define BATTLE_STAGE_OBJECT_ZERO lbl_8042FAB8
#else
#define BATTLE_STAGE_OBJECT_ZERO 0.0f
#endif

//local prototypes
void BattleObjectConfig(void);

BattleStageObject* BattleGetObjectPtr(s32 id) {
	BattleWork* wp = _battleWorkPointer;
	int i;

	for (i = 0; i < 32; i++) {
		if (wp->stageObjects[i].id == id) {
			return &wp->stageObjects[i];
		}
	}
	return NULL; //if none of the IDs match
}

BattleStageObject* BattleSearchObjectPtr(const char* name) {
	BattleWork* wp = _battleWorkPointer;
	BattleStageObject* object = wp->stageObjects;
	int i;

	for (i = 0; i < 32; i++) {
		if (object->id && !strcmp(object->data->name, name)) {
			return object;
		}
		object++;
	}
	return NULL; //if name not found
}

void BattleStageObjectInit(void) {

}





USER_FUNC(_set_mobj_shake_init) {
	BattleStageObject* obj;
	int i, layer;
	u8 position;

	//get the layer this object is on
	layer = evtGetValue(event, *event->args);

	for (i = 0; i < 32; i++) {
		obj = &_battleWorkPointer->stageObjects[i];
		if (obj->id > 0 && obj->data->mLayer == layer) {
			obj->field_0x72 = 0;
			obj->mShakePeriodLength = (u8)(irand(10) + 60);
			if (irand(100) & 1) {
				position = 0;
			}
			else {
				position = (u8)(obj->mShakePeriodLength / 2);
			}
			obj->mShakePeriodPosition = position;
			obj->mShakeAngle = BATTLE_STAGE_OBJECT_ZERO;
		}
	}
	return EVT_RETURN_DONE;
}







void BattleObjectConfig(void) {
	//stubbed in retail
}

void BattleStageObjectMain(void) {

}

USER_FUNC(_bgset_audience_bgset_access) {
	BattleAudience_Case_HaikeiSet();
	return 2;
}

void _rate_mix_3(s8* maximum, u8* total1, u8* total2, s8* value, u8* addition1, u8* addition2) {
	if (*value > 0) {
		if (*value > *maximum) {
			*maximum = *value;
		}
		*total1 += *addition1;
		*total2 += *addition2;
		if ((s8)*total2 == 0) {
			*maximum = 0;
			*total1 = 0;
		}
	}
}

void _rate_mix(s8* maximum, u8* total, s8* value, u8* addition) {
	if (*value > 0) {
		if (*value > *maximum) {
			*maximum = *value;
		}
		*total += *addition;
	}
}
