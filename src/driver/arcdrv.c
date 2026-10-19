#include "driver/arcdrv.h"
#include <string.h>

//.bss
static ArchiveEntry work[4];




void arcDelete(s32 id) {
	memset(&work[id], 0, sizeof(ArchiveEntry));
}

void arcInit(void) {
	int i;

	for (i = 0; i < 4; i++) {
		memset(&work[i], 0, sizeof(ArchiveEntry));
	}
}

void* arcOpen(const char* filename, void** addr, u32* length) {
	ARCFileInfo info;
	void* startAddr;
	int i;

	for (i = 0; i < 4; i++) {
		if ((work[i].flags & 1) && ARCOpen(&work[i].handle, filename, &info)) {
			startAddr = ARCGetStartAddrInMem(&info);
			if (addr) {
				*addr = startAddr;
			}
			if (length) {
				*length = ARCGetLength(&info);
			}
			ARCClose();
			return startAddr;
		}
	}
	if (addr) {
		*addr = NULL;
	}
	if (length) {
		*length = 0;
	}
	return NULL;
}

void arcEntry(s32 id, void* data, u32 size) {
	ArchiveEntry* entry = &work[id];
	entry->data = data;
	entry->size = size;
	ARCInitHandle(entry->data, &entry->handle);
	entry->flags |= 1;
}

BOOL arcDataCheck(void* data) {
	ArchiveEntry* entry = work;

	if ((u8*)entry->data <= (u8*)data && (u8*)entry->data + entry->size >= (u8*)data) return TRUE;
	entry++;
	if ((u8*)entry->data <= (u8*)data && (u8*)entry->data + entry->size >= (u8*)data) return TRUE;
	entry++;
	if ((u8*)entry->data <= (u8*)data && (u8*)entry->data + entry->size >= (u8*)data) return TRUE;
	entry++;
	if ((u8*)entry->data <= (u8*)data && (u8*)entry->data + entry->size >= (u8*)data) return TRUE;
	return FALSE;
}
