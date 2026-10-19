#include "evt/evt_shop.h"
#include "data/item_data.h"
#include "driver/msgdrv.h"
#include "mario/mario_pouch.h"
#include "mario/mariost.h"
#include "memory.h"
#include <string.h>

extern GlobalWork* gp;
extern GlobalWork* wp_80422810;
extern s32 fn_802424CC(void);
extern ItemData itemDataTable[];

//.sbss
#ifndef TARGET_PC
char lbl_8042B470[16];
evt_shop_work* lbl_8042B480;
#define mapname lbl_8042B470
#define _wp lbl_8042B480
#else
char mapname[16];
static evt_shop_work* _wp;
#endif

USER_FUNC(evt_shop_setup) {
	evt_shop_work* work_ptr;
	s32 r25, r27, r28, r30;
	int i;

	r28 = evtGetValue(event, event->args[0]);
	r27 = evtGetValue(event, event->args[1]);
	r30 = evtGetValue(event, event->args[2]);
	r25 = evtGetValue(event, event->args[3]);
	work_ptr = _mapAlloc(sizeof(evt_shop_work));
	memset(work_ptr, 0, sizeof(evt_shop_work));
	_wp = work_ptr;
	strncpy(mapname, gp->currentMapName, sizeof(gp->currentMapName) - 1);
	work_ptr->field_0x4 = r28;
	work_ptr->field_0x8 = r27;
	work_ptr->field_0xC = r30;
	work_ptr->field_0x10 = r25;
	work_ptr->field_0x20 = 6;
	for (i = 0; i < work_ptr->field_0x20; i++) {

	}
	return EVT_RETURN_DONE;
}

USER_FUNC(get_ptr) {
	evtSetValue(event, event->args[0], (s32)_wp);
	return EVT_RETURN_DONE;
}

USER_FUNC(reset_shop_point) {
	PouchData* pouch = pouchGetPtr();
	pouch->mShopPoints = 0;
	pouch->field_0x5B4 = 0;
	return EVT_RETURN_DONE;
}

USER_FUNC(get_shop_point) {
	s32* args = event->args;
	PouchData* pouch = pouchGetPtr();
	evtSetValue(event, *args, pouch->mShopPoints);
	return EVT_RETURN_DONE;
}

USER_FUNC(set_buy_item_id) {
	_wp->field_0xD4 = evtGetValue(event, event->args[0]);
	return EVT_RETURN_DONE;
}

USER_FUNC(shopper_name) {
	evtSetValue(event, event->args[0], ((s32*)_wp->field_0xC)[0]);
	return EVT_RETURN_DONE;
}

USER_FUNC(get_fook_evt) {
	evtSetValue(event, event->args[0], ((s32*)_wp->field_0xC)[1]);
	return EVT_RETURN_DONE;
}

USER_FUNC(get_buy_evt) {
	evtSetValue(event, event->args[0], ((s32*)_wp->field_0xC)[2]);
	return EVT_RETURN_DONE;
}

USER_FUNC(disp_off) {
	u16* value = (u16*)evtGetValue(event, event->args[0]);
	*value &= ~1;
	return EVT_RETURN_DONE;
}

USER_FUNC(help_main) {
	return fn_802424CC();
}

USER_FUNC(sell_pouchcheck_func) {
	pouchGetPtr();
	event->lwData[0] = pouchGetHaveItemCnt();
	return EVT_RETURN_DONE;
}

USER_FUNC(keep_pouchcheck_func) {
	pouchGetPtr();
	event->lwData[0] = pouchGetHaveItemCnt();
	event->lwData[1] = pouchGetEmptyKeepItemCnt();
	return EVT_RETURN_DONE;
}

USER_FUNC(unkeep_pouchcheck_func) {
	pouchGetPtr();
	event->lwData[0] = pouchGetKeepItemCnt();
	event->lwData[1] = pouchGetEmptyHaveItemCnt();
	return EVT_RETURN_DONE;
}

USER_FUNC(get_msg) {
	s32* args = event->args;
	void* table = (void*)evtGetValue(event, args[0]);
	s32 index = evtGetValue(event, args[1]);
	void** entries = *(void***)((u8*)table + 0xC);
	const char* message = *(const char**)((u8*)entries[index] + 0xC);
	evtSetValue(event, args[2], (s32)message);
	return EVT_RETURN_DONE;
}

USER_FUNC(name_price) {
	s32* args = event->args;
	s32 itemId = evtGetValue(event, args[0]);
	evtSetValue(event, args[1], (s32)msgSearch(itemDataTable[itemId].name));
	evtSetValue(event, args[2], *(u16*)((u8*)&itemDataTable[itemId] + 0x1A));
	return EVT_RETURN_DONE;
}

USER_FUNC(shop_flag_onoff) {
	s32* args = event->args;
	s32 onoff = evtGetValue(event, args[0]);
	s32 mask = evtGetValue(event, args[1]);
	evt_shop_work* work = _wp;
	if (onoff != 0) {
		work->field_0x0 |= mask;
	} else {
		work->field_0x0 &= ~mask;
	}
	return EVT_RETURN_DONE;
}

s32 evtShopIsActive(void) {
	if (strcmp(wp_80422810->currentMapName, mapname) == 0 && _wp->field_0x0 != 0) {
		return 1;
	}
	return 0;
}
