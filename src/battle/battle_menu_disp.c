#include "battle/battle_menu_disp.h"
#include "system.h"

extern BattleWork* _battleWorkPointer;
extern int sprintf(char* str, const char* format, ...);

//.sbss
#ifdef TARGET_PC
s8 seq;
#else
extern s8 lbl_8042B4C0;
#define seq lbl_8042B4C0
#endif

//.sdata
#ifdef TARGET_PC
GXColor seleItemCoordCol = {0xFF, 0xC0, 0xC0, 0xCF};
#else
extern GXColor lbl_804235E0;
#define seleItemCoordCol lbl_804235E0
extern const char lbl_802FB470[];
#endif

void SelectedItemCoordinateColorUpDate(void) {
    switch (seq) {
        case 0:
            if (++seleItemCoordCol.g == 0xFF) {
                seq = 1;
            }
            break;
        case 1:
            if (--seleItemCoordCol.r == 0xC0) {
                seq = 2;
            }
            break;
        case 2:
            if (++seleItemCoordCol.b == 0xFF) {
                seq = 3;
            }
            break;
        case 3:
            if (--seleItemCoordCol.g == 0xC0) {
                seq = 4;
            }
            break;
        case 4:
            if (++seleItemCoordCol.r == 0xFF) {
                seq = 5;
            }
            break;
        case 5:
            if (--seleItemCoordCol.b == 0xC0) {
                seq = 0;
            }
            break;
    }
}

BOOL BattleMenuKeyOKInACT(BattleWork* wp) {
	if (wp->commandMenu.unk540[0x18] == 0 && ((u8*)wp->commandMenu.field_55C)[0x14] == 0) {
		return FALSE;
	}
	return TRUE;
}

void battleMenuDispInit(void) {
	BattleWork* wp = _battleWorkPointer;
	char path[256];

#ifdef TARGET_PC
	sprintf(path, "%s/battle/common/battle_common.tpl", getMarioStDvdRoot());
#else
	sprintf(path, lbl_802FB470, getMarioStDvdRoot());
#endif
	wp->menuTex = fileAlloc(path, 4);
}

void battleMenuDispEnd(void) {
	fileFree(_battleWorkPointer->menuTex);
}

void BattleMenuDisp_MultiItem_Main(void) {}
void BattleMenuDisp_ChangePartySelect_Main(void) {}
void BattleMenuDisp_Operation_Main(void) {}
void BattleMenuDisp_WeaponSelect_Main(void) {}
void BattleMenuDisp_ActSelect_Main(void) {}
