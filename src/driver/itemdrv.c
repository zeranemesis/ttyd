#include "driver/itemdrv.h"
#include "driver/icondrv.h"
#include "driver/swdrv.h"
#include "evt/evt_cmd.h"
#include "manager/evtmgr_cmd.h"
#include "mario/mario_pouch.h"
#include "mario/mariost.h"
#include "memory.h"
#include <string.h>

extern GlobalWork* gp;
extern GlobalWork* wp_80422810;
extern void fn_802424CC(void);
extern ItemEntry* fn_800AEBC0(const char* name, ItemType type, s32 a3, s32 evtId, s32 a5,
		f32 x, f32 y, f32 z);
extern int sprintf(char* str, const char* format, ...);

//.bss
static ItemWork work[2];

//TODO: inline function? I see nothing in the symbol map
#define itemGetWork() (wp_80422810->inBattle ? &work[1] : &work[0])

inline void itemCheckBadge(ItemEntry* entry) {
	if (!wp_80422810->inBattle) {
        if (BADGE_MIN <= entry->itemId && entry->itemId < BADGE_MAX) {
            if (entry->flags & 0x100) {
                pouchArriveBadge(entry->itemId);
            }
        }
    }
}

u32 itemStatus(ItemEntry* entry, u32 mask) {
	return *(u32*)((u8*)entry + 0x38) & mask;
}

void itemStatusOn(ItemEntry* entry, u32 mask) {
	*(u32*)((u8*)entry + 0x38) |= mask;
}

u16 itemFlag(ItemEntry* entry, u16 mask) {
	return entry->flags & mask;
}

void itemFlagOff(ItemEntry* entry, u32 mask) {
	entry->flags &= ~mask;
}

void itemFlagOn(ItemEntry* entry, u32 mask) {
	entry->flags |= mask;
}

void itemSetPosition(ItemEntry* entry, f32 x, f32 y, f32 z) {
	entry->position.x = x;
	entry->position.y = y;
	entry->position.z = z;
}

void itemModeChange(ItemEntry* entry, u16 mode) {
	entry->mode = mode;
	entry->field_0x26 = 0;
}

void itemPickUp(ItemEntry* entry) {
	u32* status = (u32*)((u8*)entry + 0x38);

	*status |= 0x1000;
	entry->mode = 8;
	entry->field_0x26 = 0;
	*(u32*)((u8*)entry + 0x7C) = 0;
	*(u32*)((u8*)entry + 0x78) = 0;
	*status &= ~0x100;
}

void itemForceGet(ItemEntry* entry) {
	u32* status = (u32*)((u8*)entry + 0x38);

	*(u32*)((u8*)entry + 0x7C) = 0;
	*(u32*)((u8*)entry + 0x78) = 0;
	entry->mode = 2;
	entry->field_0x26 = 0;
	entry->flags |= 0x1000;
	*status &= ~0x10000;
}

void winHelpMain(void) {
	fn_802424CC();
}

ItemEntry* itemNameToPtr(char* name) {
	ItemWork* wp = itemGetWork();
	ItemEntry* entry;
	int i;

	for (entry = wp->entries, i = 0; i < wp->count; i++, entry++) {
		if (!strcmp(entry->name, name)) {
			return entry;
		}
	}
	return NULL;
}

void itemInit(void) { // 1:1
    ItemWork* wp;

    wp = &work[0];
    wp->count = 32;
    wp->entries = __memAlloc(HEAP_DEFAULT, sizeof(ItemEntry) * wp->count);
    memset(wp->entries, 0, sizeof(ItemEntry) * wp->count);
    wp->currId = 0;

    wp = &work[1];
    wp->count = 32;
    wp->entries = __memAlloc(HEAP_DEFAULT, sizeof(ItemEntry) * wp->count);
    memset(wp->entries, 0, sizeof(ItemEntry) * wp->count);
    wp->currId = 0;
}

void itemReInit(void) {
    ItemWork* wp = itemGetWork();
    ItemEntry* entry;
    int i;

    if (!wp_80422810->inBattle) {
        for (entry = wp->entries, i = 0; i < wp->count; i++, entry++) {
            if (entry->flags & 1) { //flush all current badges
                itemCheckBadge(entry);
            }
        }
    }

    //clear all existing entries
    for (entry = wp->entries, i = 0; i < wp->count; i++, entry++) {
        iconDelete(entry->name);
        entry->flags &= ~1;
        memset(entry->name, 0, sizeof(entry->name));
    }
}

void itemCoinDrop(Vec* position) {
	ItemEntry* coin;

	if (((s32 (*)(void))pouchGetCoin)() <= 0) {
		return;
	}

	pouchAddCoin(-1);
	coin = fn_800AEBC0(NULL, (ItemType)0x79, 8, -1, 0,
	position->x, position->y, position->z);
	if (coin != NULL) {
		itemStatusOn(coin, 0x40);
	}
}






ItemEntry* itemEntry(const char* name, ItemType type, s32 a3, s32 evtId, s32 a5, f32 a6, f32 a7, f32 a8) {
	ItemWork* wp = itemGetWork();
	ItemEntry* entry;
	s32 evtVar;
	int i;
	char itemName[64];


	evtVar = evtId;
	if (evtId != -1) {
		if (evtId >= GSWF(0) && evtId <= EVTDAT_GSWF_MAX) {
			evtVar = evtId + EVTDAT_GSWF_BASE;
			if (swGet(evtId + EVTDAT_GSWF_BASE)) {
				return NULL;
			}
		}
		else {
			if (evtId >= LSWF(0) && evtId <= EVTDAT_LSWF_MAX) {
				evtVar = evtId + EVTDAT_LSWF_BASE;
				if (_swGet(evtId + EVTDAT_LSWF_BASE)) {
					return NULL;
				}
			}
		}
	}

	if (name == NULL) {
		if (++wp->currId >= 0xFFFFF) { //fix overflow
			wp->currId = 1;
		}
		sprintf(itemName, "_i%05x", wp->currId);
		name = itemName;
	}

	entry = wp->entries;
	for (i = 0; i < wp->count; i++, entry++) {
		if (entry->flags & 1 && !strcmp(entry->name, name)) {
			break;
		}
	}

	entry = wp->entries;
	if (wp->count > 0) { //TODO: redundant check?
		for (i = 0; i < wp->count; i++, entry++) {
			if (!(entry->flags & 1)) {
				break;
			}
		}
	}
}


void itemMain(void) {

}
