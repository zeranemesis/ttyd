#include "battle/battle_ac_help.h"
#include "battle/battle.h"
#include "battle/battle_audience.h"
#include "driver/dispdrv.h"

extern const f32 lbl_80433DC0;

static void _ac_help_disp(CameraId cameraId, void* param) {}

void BattleAcHelpInit(void) {
	//stubbed in retail
}

void BattleAcHelpMain(void) {
    BattleWork* wp = _battleWorkPointer;

    if (!*(void**)((u8*)wp + 0x19008)) {
        return;
    }
    if (!*(s32*)((u8*)wp + 0x1900C)) {
        return;
    }
    if (wp->flags & 2) {
        return;
    }
    dispEntry(CAMERA_2D, lbl_80433DC0, 1, _ac_help_disp, NULL);
}

void BattleAcHelpSetDispType(s32 type) {
    *(s32*)((u8*)_battleWorkPointer + 0x1900C) = type;
	if (type != 0) {
		BattleAudience_WinSetActive(0);
	} else {
		BattleAudience_WinSetActive(1);
	}
}

void BattleAcHelpSetHelp(void* help) {
    *(void**)((u8*)_battleWorkPointer + 0x19008) = help;
}
