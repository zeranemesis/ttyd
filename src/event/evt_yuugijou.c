#include "evt/evt_yuugijou.h"
#include <string.h>
#include "manager/evtmgr.h"
#include "manager/evtmgr_cmd.h"

PiantaParlorWork yuugijouwork;
PiantaParlorWork* lbl_8042B698;


s32 evtYuuWindowEndChk(EventEntry* event) {
    return (lbl_8042B698->field_0x0 & 8) != 0;
}

s32 yuuminigame_window_end(EventEntry* event) {
    lbl_8042B698->field_0x0 |= 8;
    return 2;
}

s32 yuugijou_montemedal_wait(EventEntry* event) {
    if ((s32)lbl_8042B698->field_0x10 != lbl_8042B698->tokens) {
        return 0;
    }
    return 2;
}

s32 yuugijou_get_montemedal(EventEntry* event) {
    evtSetValue(event, event->args[0], lbl_8042B698->tokens);
    return 2;
}

extern s32 bdsw;
void unk_8022439c(s32 value) {
    bdsw = value;
}
s32 yuugijou_set_yupflag(EventEntry* event) {
    if (evtGetValue(event, event->args[0]) != 0) {
        lbl_8042B698->field_0x0 |= 4;
    } else {
        lbl_8042B698->field_0x0 &= ~4;
    }
    return 2;
}

s32 yuugijou_get_takeoutflag(EventEntry* event) {
    s32* args = event->args;
    if (lbl_8042B698->field_0x0 & 2) {
        evtSetValue(event, args[0], 1);
    } else {
        evtSetValue(event, args[0], 0);
    }
    return 2;
}

s32 yuugijou_get_montemedal_keep(EventEntry* event) {
    s32* args = event->args;
    if (lbl_8042B698->field_0x0 & 2) {
        evtSetValue(event, args[0], 0);
    } else {
        evtSetValue(event, args[0], lbl_8042B698->field_0x4);
    }
    return 2;
}
s32 yuugijou_montemedal_disp_onoff(EventEntry* event) {
    if (evtGetValue(event, event->args[0]) != 0) {
        lbl_8042B698->field_0x0 |= 1;
        lbl_8042B698->field_0x10 = lbl_8042B698->tokens;
    } else {
        lbl_8042B698->field_0x0 &= ~1;
    }
    return 2;
}
extern s32 window_open[];
extern s32 minigame_end[];
extern s32 bdsw;

s32 evtYuuWindow(EventEntry* event) {
    lbl_8042B698->field_0x0 &= ~8;
    return (s32)evtEntryType(window_open, 0, 0, 0);
}
s32 yuuminigame_end(EventEntry* event) {
    evtEntryType(minigame_end, 0, 0, 0xEF);
    return 2;
}

s32 unk_802243a4(EventEntry* event) {
    evtSetValue(event, event->args[0], bdsw);
    return 2;
}
s32 yuugijou_montemedal_return(EventEntry* event) {
    if ((lbl_8042B698->field_0x0 & 2) == 0) {
        lbl_8042B698->tokens = lbl_8042B698->field_0x4;
        lbl_8042B698->field_0x10 = lbl_8042B698->field_0x4;
        lbl_8042B698->field_0x0 |= 2;
        lbl_8042B698->field_0x0 |= 1;
    }
    return 2;
}

s32 yuugijou_montemedal_keep(EventEntry* event) {
    if (lbl_8042B698->field_0x0 & 2) {
        lbl_8042B698->field_0x4 = lbl_8042B698->tokens;
        lbl_8042B698->tokens = 0;
        lbl_8042B698->field_0x10 = 0;
        lbl_8042B698->field_0x0 &= ~2;
        lbl_8042B698->field_0x0 &= ~1;
    }
    return 2;
}
void yuugijou_init(void) {
	lbl_8042B698 = &yuugijouwork;
	memset(lbl_8042B698, 0, sizeof(PiantaParlorWork));

	lbl_8042B698->field_0x14 = 1;
	lbl_8042B698->field_0x18 = 1;
	lbl_8042B698->field_0x1C = 1;
	lbl_8042B698->field_0x20 = 1;
	lbl_8042B698->field_0x24 = 1;

	lbl_8042B698->field_0x74 = 0x5B964;
	lbl_8042B698->field_0x78 = 0x5825A;
	lbl_8042B698->field_0x7C = 0x4F826;
	lbl_8042B698->field_0x80 = 0x4C3D8;
	lbl_8042B698->field_0x84 = 0x4237E;

	lbl_8042B698->field_0xD4 = 0xE;
	lbl_8042B698->field_0xD8 = 0xC;
	lbl_8042B698->field_0xDC = 0xF;
	lbl_8042B698->field_0xE0 = 0xB;
	lbl_8042B698->field_0xE4 = 0xA;

	lbl_8042B698->field_0x2C = 1;
	lbl_8042B698->field_0x30 = 1;
	lbl_8042B698->field_0x34 = 1;
	lbl_8042B698->field_0x38 = 1;
	lbl_8042B698->field_0x3C = 1;

	lbl_8042B698->field_0x8C = 2;
	lbl_8042B698->field_0x90 = 5;
	lbl_8042B698->field_0x94 = 0x12;
	lbl_8042B698->field_0x98 = 0x1B;
	lbl_8042B698->field_0x9C = 0x8A;

	lbl_8042B698->field_0xEC = 0xF;
	lbl_8042B698->field_0xF0 = 0xE;
	lbl_8042B698->field_0xF4 = 0xE;
	lbl_8042B698->field_0xF8 = 0xE;
	lbl_8042B698->field_0xFC = 0xC;

	lbl_8042B698->field_0x44 = 1;
	lbl_8042B698->field_0x48 = 1;
	lbl_8042B698->field_0x4C = 1;
	lbl_8042B698->field_0x50 = 1;
	lbl_8042B698->field_0x54 = 1;

	lbl_8042B698->field_0xA4 = 0x20850;
	lbl_8042B698->field_0xA8 = 0x20436;
	lbl_8042B698->field_0xAC = 0x20184;
	lbl_8042B698->field_0xB0 = 0x1F91E;
	lbl_8042B698->field_0xB4 = 0x1E3AC;

	lbl_8042B698->field_0x104 = 0x13;
	lbl_8042B698->field_0x108 = 0x11;
	lbl_8042B698->field_0x10C = 0xF;
	lbl_8042B698->field_0x110 = 0xE;
	lbl_8042B698->field_0x114 = 8;

	lbl_8042B698->field_0x5C = 1;
	lbl_8042B698->field_0x60 = 1;
	lbl_8042B698->field_0x64 = 1;
	lbl_8042B698->field_0x68 = 1;
	lbl_8042B698->field_0x6C = 1;

	lbl_8042B698->field_0xBC = 0x16BDE;
	lbl_8042B698->field_0xC0 = 0x16968;
	lbl_8042B698->field_0xC4 = 0x16382;
	lbl_8042B698->field_0xC8 = 0x1627E;
	lbl_8042B698->field_0xCC = 0x15950;

	lbl_8042B698->field_0x11C = 0x10;
	lbl_8042B698->field_0x120 = 0xF;
	lbl_8042B698->field_0x124 = 0xC;
	lbl_8042B698->field_0x128 = 0xB;
	lbl_8042B698->field_0x12C = 0xB;
}
