#include "battle/battle_seq_command.h"
#include "battle/battle.h"
#include "battle/battle_monosiri.h"
#include "battle/battle_unit.h"
#include "mario/mario_pouch.h"
#include "driver/camdrv.h"
#include "driver/dispdrv.h"

//TODO: move into battle/seq/?

extern void BattleMenuDisp_ActSelect_End(BattleWork* wp);
extern void BattleMenuDisp_ChangePartySelect_End(BattleWork* wp);
extern void BattleMenuDisp_WeaponSelect_End(BattleWork* wp);
extern void _btlcmd_GetCursorPtr(BattleCommand* command, s32 index, BattleCursor** cursor);
extern void BattleMenuDisp_ActSelect_Disp(BattleWork* wp);
extern void BattleMenuDisp_ChangePartySelect_Disp(BattleWork* wp);
//local prototypes
void BattleDrawEnemyHP(CameraId camId, void* param);

void BattleDrawEnemyHP(CameraId camId, void* param) {
	BattleWork* wp = param;
	BattleUnit* unit;
	Vec position;
	int i;

	camGetPtr(CAMERA_2D); //unused, probably just stored

	for (i = 0; i < 64; i++) {
		unit = wp->units[i];
		if (unit) {
			if (!BtlUnit_CheckStatus(unit, STATUS_INSTANT_KILL)) {
				if (battleCheckUnitMonosiriFlag(unit)) {
					if (!unit->mAttributeFlags & 0x2000000) {
						if (wp->commandMenu.field_4 & 0x1000000) {
							if (unit->flags & 1) {
								position.x = unit->mPosition.x;
								position.y = unit->mPosition.y;
								position.z = unit->mPosition.z;
								if (wp->flags & 0x80) {
									position.x = unit->mHomePosition.x;
									position.y = unit->mHomePosition.y;
									position.z = unit->mHomePosition.z;
								}
								position.x += (f32)unit->mHpGaugeOffset[0];
								position.y += (f32)unit->mHpGaugeOffset[1];
							}
						}
					}
				}
			}
		}
	}
}


void _commandRestoreRec(void) {
    BattleWork* wp = _battleWorkPointer;
    s32* record = (s32*)&wp->commandMenu.unk540[0x14];

    if (wp->commandMenu.unk540[0x18] == 0) {
        *record = wp->commandMenu.unk0;
    }
    else if (*record == wp->commandMenu.unk0) {
        *record = 0;
        wp->commandMenu.unk540[0x18] = 0;
    }
}

s32 _getHammerIconId(void) {
    s32 level = pouchGetPtr()->hammerLevel;

    switch (level) {
    case 3:
        return 0xA6;
    case 2:
        return 0xA5;
    default:
        return 0xA4;
    }
}

s32 _getJumpIconId(void) {
    s32 level = pouchGetPtr()->jumpLevel;

    switch (level) {
    case 3:
        return 0xA9;
    case 2:
        return 0xA8;
    default:
        return 0xA7;
    }
}
void _cursor_init(BattleCursor* cursor) {
	cursor->field_0 = 0;
	cursor->field_4 = 0;
}


void BattleCommandDisplay_WeaponMultiItemCancelCheckSetup(BattleWork* wp) {
    wp->commandMenu.cursor[13].field_0 = 0;
    wp->commandMenu.cursor[13].field_4 = 0;
    wp->commandMenu.cursor[13].unk8 = 2;
}
void BattleCommandInit(BattleWork* wp) {
	wp->commandMenu.unk0 = 0;
	wp->flags |= 0x80;
}



void BattleCommandDisplay_ActMenuDisp(CameraId camId, BattleWork* wp) {
    BattleCursor* cursor;

    _btlcmd_GetCursorPtr(&wp->commandMenu, 14, &cursor);
    BattleMenuDisp_ActSelect_Disp(wp);
}
void BattleCommandDisplay_ActMenuEnd(BattleWork* wp) {
    BattleMenuDisp_ActSelect_End(wp);
    wp->commandMenu.field_4 &= ~1;
}
void BattleCommandDisplay_AllEnd(BattleWork* wp) {
	wp->commandMenu.field_4 &= ~0x00111111;
}



void BattleCommandDisplay_WeaponSelectMenuEnd(BattleWork* wp) {
    BattleMenuDisp_WeaponSelect_End(wp);
    wp->commandMenu.field_4 &= ~0x10;
}
void BattleCommandDisplay_ChangePartySelectMenuEnd(BattleWork* wp) {
    BattleMenuDisp_ChangePartySelect_End(wp);
    wp->commandMenu.field_4 &= ~0x1000;
}
void BattleCommandDisplay_ProtectPartnerSelectMenuEnd(BattleWork* wp) {
	wp->commandMenu.field_4 &= ~0x00100000;
}
