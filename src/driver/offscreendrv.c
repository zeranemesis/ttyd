#include "driver/offscreendrv.h"
#include "driver/camdrv.h"
#include "driver/dispdrv.h"
#include "sdk/DEMOInit.h"
#include "mario/mariost.h"
#include "memory.h"
#include <string.h>

extern GlobalWork* wp_80422810;
#define gp wp_80422810

extern const f32 lbl_8042CE30;

//.sbss
static OffscreenWork lbl_8042B278[2];

#define offscreenGetWork(inBattle) ((inBattle) ? &lbl_8042B278[1] : &lbl_8042B278[0])
#define offscreenGetWorkGp() (gp->inBattle ? &lbl_8042B278[1] : &lbl_8042B278[0])

void offscreenInit(void) {
	OffscreenWork* workptr = &lbl_8042B278[0];
	lbl_8042B278[0].count = 10;
	workptr->entries = __memAlloc(HEAP_DEFAULT, sizeof(OffscreenEntry) * workptr->count);
	memset(workptr->entries, 0, sizeof(OffscreenEntry) * workptr->count);

	workptr = &lbl_8042B278[1];
	workptr->count = 10;
	workptr->entries = __memAlloc(HEAP_DEFAULT, sizeof(OffscreenEntry) * workptr->count);
	memset(workptr->entries, 0, sizeof(OffscreenEntry) * workptr->count);
}

void offscreenReset(BOOL inBattle) {
	OffscreenWork* workptr = offscreenGetWork(inBattle);
	memset(workptr->entries, 0, workptr->count * sizeof(OffscreenEntry));
}

s32 offscreenEntry(const char* name) {
	OffscreenWork* workptr = offscreenGetWorkGp();
	int i, count;
	OffscreenEntry* entry;

	for (i = 0, count = workptr->count, entry = workptr->entries; i < count; i++, entry++) {
		if (entry->flags & 1 && !strcmp(entry->name, name)) {
			break;
		}
	}

	for (i = 0, entry = workptr->entries; i < count; i++, entry++) {
		if (!(entry->flags & 1)) break;
	}

	entry->flags = 1;
	strcpy(entry->name, name);
	entry->currLeft = 1020;
	entry->currTop = 1020;
	entry->currRight = 2;
	entry->currBottom = 2;
	entry->prevLeft = 1020;
	entry->prevTop = 2;
	entry->prevRight = 2;
	entry->prevBottom = 2;
	entry->unk3C = 5;
	entry->unk40 = 5;
	return i;
}

void offscreenDisp(CameraId camId, void* param) {
	OffscreenWork* workptr = offscreenGetWorkGp();
	int i;
	int count = workptr->count;
	OffscreenEntry* entry = workptr->entries;
	u32 texSize;

	for (i = 0, entry = workptr->entries; i < count; i++, entry++) {
		if (entry->flags & 1) {
			if (entry->flags & 2) {
				u16 width = (u16)(entry->currRight - entry->currLeft);
				u16 height = (u16)(entry->currBottom - entry->currTop);

				GXSetTexCopySrc(entry->currLeft, entry->currTop, width, height);
				GXSetTexCopyDst(width, height, entry->unk3C, FALSE);
				texSize = GXGetTexBufferSize(width, height, entry->unk3C, FALSE, 0);
				entry->texPtr = smartAlloc(texSize, 3);
				GXSetZMode(TRUE, GX_LEQUAL, TRUE);
				GXCopyTex(entry->texPtr->address, TRUE);
				GXPixModeSync();
				GXInitTexObj(&entry->texObj, entry->texPtr->address, width, height, entry->unk40, 0, 0, 0);
				entry->flags |= 4;
			}
		}
	}
}

void offscreenMain(void) {
	OffscreenWork* workptr = offscreenGetWorkGp();
	int i, count;
	OffscreenEntry* entry;

	for (i = 0, count = workptr->count, entry = workptr->entries; i < count; i++, entry++) {
		if (entry->flags & 1) {
			entry->flags &= ~2;
			entry->flags &= ~4;
			entry->prevLeft = entry->currLeft;
			entry->prevTop = entry->currTop;
			entry->prevRight = entry->currRight;
			entry->prevBottom = entry->currBottom;
			entry->currLeft = 1020;
			entry->currTop = 1020;
			entry->currRight = 2;
			entry->currBottom = 2;
		}
	}
	dispEntry(CAMERA_OFFSCREEN2, lbl_8042CE30, 7, offscreenDisp, NULL);
}

void offscreenAddBoundingBox(s32 id, u16 left, u16 top, u16 right, u16 bottom) {
	OffscreenEntry* entry;

	if (right > left) {
		entry = &offscreenGetWorkGp()->entries[id];
		if (entry->flags & 1) {
			if (left >= 2) {
				left -= 2;
			}
			if (top >= 2) {
				top -= 2;
			}
			if (right <= DEMOGetRenderModeObj()->fbWidth - 2) {
				right += 2;
			}
			if (bottom <= DEMOGetRenderModeObj()->efbHeight - 2) {
				bottom += 2;
			}
			left -= (left & 1);
			top -= (top & 1);
			right += (right & 1);
			bottom += (bottom & 1);
			if (left < entry->currLeft) {
				entry->currLeft = left;
				entry->flags |= 2;
			}
			if (top < entry->currTop) {
				entry->currTop = top;
				entry->flags |= 2;
			}
			if (right > entry->currRight) {
				entry->currRight = right;
				entry->flags |= 2;
			}
			if (bottom > entry->currBottom) {
				entry->currBottom = bottom;
				entry->flags |= 2;
			}
		}
	}
}

void offscreenDelete(const char* name) {
	OffscreenWork* workptr = offscreenGetWorkGp();
	int i;
	int count = workptr->count;
	OffscreenEntry* entry = workptr->entries;

	for (i = 0; i < count; i++, entry++) {
		if (entry->flags & 1) {
			if (!strcmp(entry->name, name)) {
				break;
			}
		}
	}
	entry->flags = 0;
}

OffscreenEntry* offscreenNameToPtr(const char* name) {
	OffscreenWork* workptr = offscreenGetWorkGp();
	int i;
	int count = workptr->count;
	OffscreenEntry* entry = workptr->entries;

	for (i = 0; i < count; i++, entry++) {
		if (entry->flags & 1) {
			if (!strcmp(entry->name, name)) {
				goto done;
			}
		}
	}
done:
	return entry;
}

s32 offscreenNameToId(const char* name) {
	OffscreenWork* workptr = offscreenGetWorkGp();
	int i;
	int count = workptr->count;
	OffscreenEntry* entry = workptr->entries;

	for (i = 0; i < count; i++, entry++) {
		if (entry->flags & 1) {
			if (!strcmp(entry->name, name)) {
				goto found;
			}
		}
	}
found:
	return i;
}

void offscreenGetTexObj(s32 id, GXTexObj** texObj, smartEntry** texData) {
	OffscreenEntry* entry = &offscreenGetWorkGp()->entries[id];

	if (!(entry->flags & 1)) {
		*texObj = NULL;
		*texData = NULL;
		return;
	}
	if (!(entry->flags & 2)) {
		*texObj = NULL;
		*texData = NULL;
		return;
	}
	if (!(entry->flags & 4)) {
		*texObj = NULL;
		*texData = NULL;
		return;
	}
	*texObj = &entry->texObj;
	*texData = entry->texPtr;
}

s32 offscreenGetBoundingBox_BeforeFrame(s32 id, u16* left, u16* top, u16* right, u16* bottom) {
	OffscreenEntry* entry = &offscreenGetWorkGp()->entries[id];

	if (!(entry->flags & 1)) {
		return 0;
	}
	if (!(entry->prevLeft <= entry->prevRight)) {
		return 0;
	}
	*left = entry->prevLeft;
	*top = entry->prevTop;
	*right = entry->prevRight;
	*bottom = entry->prevBottom;
	return 1;
}

s32 offscreenGetBoundingBox(s32 id, u16* left, u16* top, u16* right, u16* bottom) {
	OffscreenEntry* entry = &offscreenGetWorkGp()->entries[id];

	if (!(entry->flags & 1)) {
		return 0;
	}
	if (!(entry->flags & 2)) {
		return 0;
	}
	*left = entry->currLeft;
	*top = entry->currTop;
	*right = entry->currRight;
	*bottom = entry->currBottom;
	return 1;
}
