#include "manager/filemgr.h"
#include "dolphin/os.h"
#include "driver/arcdrv.h"
#include "memory.h"
#include "sdk/texPalette.h"
#include <stdarg.h>
#include <string.h>

extern int vsprintf(char* string, const char* format, va_list arg);
extern OSThread dvdmgr_thread;

//.bss
char _filename[0x400]; //for fileAllocf/fileAsyncf vsprintf

//.sbss
FileManagerWork fileWork;

//.sdata
FileManagerWork* attack_event_80424DB8 = &fileWork;

//local prototypes
void fileGarbageDataAdrClear(FileEntry* entry);
void fileGarbageDataAdrSet(void* data, s32 type);
FileEntry* _fileAlloc(const char* filename, u8 type, void (*callback)(FileEntry*));
void dvdReadDoneCallBack(s32 error, DVDFileInfo* info);
s32 fileAsync(const char* filename, u8 type, FileCallback callback);




void fileInit(void) {
	FileEntry* ptr;
	int i;

	attack_event_80424DB8->entries = __memAlloc(HEAP_DEFAULT, sizeof(FileEntry) * 0x400);
	attack_event_80424DB8->archiveType = 0;
	attack_event_80424DB8->firstused = 0;
	attack_event_80424DB8->lastused = 0;
	memset(attack_event_80424DB8->entries, 0, sizeof(FileEntry) * 0x400);

	for (ptr = attack_event_80424DB8->entries, i = 0; i < 0x400; i++, ptr++) {
		ptr->next = ptr + 1;
	}

	attack_event_80424DB8->firstavailable = attack_event_80424DB8->entries;
	attack_event_80424DB8->lastavailable = &attack_event_80424DB8->entries[0x3FF];
	attack_event_80424DB8->lastavailable->next = NULL;
}

#pragma dont_inline on
void fileGarbageDataAdrClear(FileEntry* entry) {
	TEXPalette* tpl;
	TEXHeader* img;
	CLUTHeader* pal;
	int i;
	void* data = *entry->data;
	switch (entry->field_0x1) {
		case 4: //TPL file
			tpl = data;
			if (tpl->versionNumber != 0x0020AF30) {
				OSPanic("filemgr.c", 147, "invalid version number for texture palette");
			}
			if ((uintptr_t)tpl->descriptorArray >= (uintptr_t)tpl) {
				for (i = 0; i < tpl->numDescriptors; i++) {
					img = tpl->descriptorArray[i].textureHeader;
					if (img) {
						if (img->unpacked) {
							img->unpacked = FALSE;
							img->data = (char*)((uintptr_t)img->data - (uintptr_t)tpl);
						}
						tpl->descriptorArray[i].textureHeader = (void*)((uintptr_t)tpl->descriptorArray[i].textureHeader - (uintptr_t)tpl);
					}
					pal = tpl->descriptorArray[i].CLUTHeader;
					if (pal) {
						if (pal->unpacked) {
							pal->unpacked = FALSE;
							pal->data = (char*)((uintptr_t)pal->data - (uintptr_t)tpl);
						}
						tpl->descriptorArray[i].CLUTHeader = (void*)((uintptr_t)tpl->descriptorArray[i].CLUTHeader - (uintptr_t)tpl);
					}
				}
				tpl->descriptorArray = (void*)((uintptr_t)tpl->descriptorArray - (uintptr_t)tpl);
			}
			break;

		default: break;
	}
	//case 0, binary/default, rel
	//case 4, TEXPalette*, state 1
	//case 5, AnimationPoseData*, state 1
}
#pragma dont_inline reset

#pragma dont_inline on
void fileGarbageDataAdrSet(void* data, s32 type) {
	if (type == 4 && data) {
		UnpackTexPalette((TEXPalette*)data);
	}
}
#pragma dont_inline reset

void fileGarbageMoveMem(void* data, FileEntry* file) {
	if (file->state == 3) {
		if (file->entry == NULL) {
			fileGarbageDataAdrClear(file);
		}
	}
	else {
		fileGarbageDataAdrClear(file);
	}
	memmove(data, *file->data, ((u32*)file->data)[1]);
	*file->data = data;
	if (file->state == 3) {
		if (file->entry != NULL) {
			file->entry->address = data;
		}
		else {
			fileGarbageDataAdrSet(*file->data, file->field_0x1);
		}
	}
	else {
		fileGarbageDataAdrSet(*file->data, file->field_0x1);
	}
}

void _fileGarbage(BOOL a1) {

}

FileEntry* fileAllocf(u8 type, const char* format, ...) {
	va_list va;

	va_start(va, format);
	vsprintf(_filename, format, va);
	return _fileAlloc(_filename, type, NULL);
}

FileEntry* fileAlloc(const char* filename, u8 type) {
	return _fileAlloc(filename, type, NULL);
}

FileEntry* _fileAlloc(const char* filename, u8 type, void (*callback)(FileEntry*)) {
	smartEntry* smart;
	DVDEntry* entry;
	FileEntry *search, *newentry, *temp;
	void* handle;
	u32 size, test, readSize;

	for (search = attack_event_80424DB8->firstused; search; search = search->next) {
		if (search->state && !strcmp(search->filename, filename)) {
			while (search->entry) {;} //infinite loop????
			if (search->state == 3) {
				search->state = 1;
				search->references = 0;
			}
			else if (search->state == 2) {
				search->state = 1;
			}
			search->references++;
			return search;
		}
	}
	newentry = attack_event_80424DB8->firstavailable;
	if (!newentry) {
		_fileGarbage(1);
		newentry = attack_event_80424DB8->firstavailable;
	}
	if (!newentry) {
		_fileGarbage(0);
		newentry = attack_event_80424DB8->firstavailable;
	}

	handle = NULL;
	temp = newentry->next;

	switch (attack_event_80424DB8->archiveType) {
		case 0:
			handle = arcOpen(filename, NULL, NULL);
			break;
		case 1:
			handle = arcOpen(filename, NULL, NULL);
			if (!handle) {
				handle = arcOpen(filename, NULL, NULL);
			}
			if (!handle) {
				handle = arcOpen(filename, NULL, NULL);
			}
			break;
		case 2:
			handle = arcOpen(filename, NULL, NULL);
			if (!handle) {
				handle = arcOpen(filename, NULL, NULL);
			}
			if (!handle) {
				handle = arcOpen(filename, NULL, NULL);
			}
			break;
	}
	if (handle) {
		fileGarbageDataAdrSet(handle, type);
	}
	if (handle) {
		newentry->state = 1;
		newentry->data = &newentry->field_0x4;
		*newentry->data = handle;
		newentry->references = 1;
		newentry->field_0x1 = type;
		newentry->next = NULL;
		strcpy(newentry->filename, filename);
		newentry->entry = NULL;
	}
	else {
		printf("[_fileAlloc] DVDMgrOpen('%s', 2, 0)...\n", filename); fflush(stdout);
		entry = DVDMgrOpen(filename, 2, 0);
		printf("[_fileAlloc] DVDMgrOpen result=%p\n", entry); fflush(stdout);
		if (!entry) {
			return NULL;
		}
		size = DVDMgrGetLength(entry);
		readSize = OSRoundUp32B(size);
		printf("[_fileAlloc] size=%u, readSize=%u\n", size, readSize); fflush(stdout);
		smart = smartAlloc(readSize, 0);
		if (!smart) {
			printf("[_fileAlloc] smartAlloc failed!\n"); fflush(stdout);
			DVDMgrClose(entry);
			return NULL;
		}
		smart->field_0x8 = newentry;
		printf("[_fileAlloc] Reading %u bytes into %p...\n", readSize, smart->address); fflush(stdout);
		DVDMgrRead(entry, smart->address, readSize, 0);
		DVDMgrClose(entry);
		printf("[_fileAlloc] DVDMgrRead complete. Magic: %02X %02X %02X %02X\n",
			((u8*)smart->address)[0], ((u8*)smart->address)[1], ((u8*)smart->address)[2], ((u8*)smart->address)[3]); fflush(stdout);
		newentry->data = &smart->address;
		newentry->state = 1;
		newentry->references = 1;
		newentry->field_0x1 = type;
		newentry->next = NULL;
		strcpy(newentry->filename, filename);
		newentry->entry = NULL;
		printf("[_fileAlloc] Calling fileGarbageDataAdrSet(type=%d)...\n", type); fflush(stdout);
		fileGarbageDataAdrSet(smart->address, type);
		printf("[_fileAlloc] fileGarbageDataAdrSet done!\n"); fflush(stdout);
	}
	attack_event_80424DB8->firstavailable = temp;
	if (!attack_event_80424DB8->firstavailable) {
		attack_event_80424DB8->lastavailable = NULL;
	}
	if (attack_event_80424DB8->firstused) {
		attack_event_80424DB8->lastused->next = newentry;
	}
	else {
		attack_event_80424DB8->firstused = newentry;
	}
	attack_event_80424DB8->lastused = newentry;
	return newentry;
}

void fileFree(FileEntry* handle) {
	if (handle) {
		if (handle->data) {
			if (handle->state == 1) {
				handle->references--;
				if (!handle->references) {
					handle->state = 2;
				}
			}
		}
	}
}

void dvdReadDoneCallBack(s32 error, DVDFileInfo* info) {
	FileEntry* search;
	DVDEntry* entry;

	for (search = attack_event_80424DB8->firstused; search; search = search->next) {
		if (search->entry && &search->entry->info == info) {
			break;
		}
	}
	if (!search) {
		return;
	}

	entry = search->entry;
	printf("[dvdReadDoneCallBack] Finished reading '%s', error=%d\n", search->filename, error); fflush(stdout);
	if (error == -3) {
		search->entry = NULL;
		search->state = 2;
		search->field_0x1 = 11;
		DVDMgrClose(entry);
	}
	else {
		search->entry = NULL;
		fileGarbageDataAdrSet(*search->data, search->field_0x1);
		DVDMgrClose(entry);
		if (search->callback) {
			search->callback(search);
		}
	}
}

s32 fileAsyncf(u8 type, FileCallback callback, const char* format, ...) {
	va_list va;

	va_start(va, format);
	vsprintf(_filename, format, va);
	return fileAsync(_filename, type, callback);
}

s32 fileAsync(const char* filename, u8 type, FileCallback callback) {
	FileEntry *search, *newentry, *temp;
	smartEntry* smart;
	DVDEntry* entry;
	void* handle;
	u32 size, test, readSize;

	for (search = attack_event_80424DB8->firstused; search; search = search->next) {
		if (search->state && !strcmp(search->filename, filename)) {
			if (search->entry) return 0;
			if (search->state == 3) return 1;
			if (search->state == 2) {
				search->state = 3;
			}
			else {
				return 1;
			}
		}
	}

	newentry = attack_event_80424DB8->firstavailable;
	if (!newentry) {
		_fileGarbage(1);
		newentry = attack_event_80424DB8->firstavailable;
	}
	if (!newentry) {
		_fileGarbage(0);
		newentry = attack_event_80424DB8->firstavailable;
	}

	handle = NULL;
	temp = newentry->next;

	switch (attack_event_80424DB8->archiveType) {
		case 0:
			handle = arcOpen(filename, NULL, NULL);
			break;
		case 1:
			handle = arcOpen(filename, NULL, NULL);
			if (!handle) {
				handle = arcOpen(filename, NULL, NULL);
			}
			if (!handle) {
				handle = arcOpen(filename, NULL, NULL);
			}
			break;
		case 2:
			handle = arcOpen(filename, NULL, NULL);
			if (!handle) {
				handle = arcOpen(filename, NULL, NULL);
			}
			if (!handle) {
				handle = arcOpen(filename, NULL, NULL);
			}
			break;
	}

	if (handle) {
		fileGarbageDataAdrSet(handle, type);
	}

	if (handle) {
		newentry->state = 3;
		newentry->data = &newentry->field_0x4;
		*newentry->data = handle;
		newentry->references = 0;
		newentry->field_0x1 = type;
		newentry->next = 0;
		strcpy(newentry->filename, filename);
		newentry->entry = NULL;
		attack_event_80424DB8->firstavailable = attack_event_80424DB8->firstavailable->next;
		if (!attack_event_80424DB8->firstavailable) {
			attack_event_80424DB8->lastavailable = NULL;
		}
		if (attack_event_80424DB8->firstused) {
			attack_event_80424DB8->lastused->next = newentry;
		}
		else {
			attack_event_80424DB8->firstused = newentry;
		}
		attack_event_80424DB8->lastused = newentry;
		return 1;
	}
	else {
		entry = DVDMgrOpen(filename, 2, 0);
		if (!entry) {
			return -1;
		}
		size = DVDMgrGetLength(entry);
		DVDMgrClose(entry);
		readSize = OSRoundUp32B(size);
		smart = smartAlloc(readSize, 0);
		if (!smart) {
			return 0;
		}
		smart->field_0x8 = newentry;
		newentry->data = &smart->address;
		newentry->state = 3;
		newentry->references = 0;
		newentry->field_0x1 = type;
		newentry->next = NULL;
		newentry->callback = callback;
		strcpy(newentry->filename, filename);
		attack_event_80424DB8->firstavailable = temp;
		if (!attack_event_80424DB8->firstavailable) {
			attack_event_80424DB8->lastavailable = NULL;
		}
		if (attack_event_80424DB8->firstused) {
			attack_event_80424DB8->lastused->next = newentry;
		}
		else {
			attack_event_80424DB8->firstused = newentry;
		}
		attack_event_80424DB8->lastused = newentry;
		newentry->entry = DVDMgrOpen(filename, 2u, 0);
		DVDMgrReadAsync(newentry->entry, smart->address, readSize, 0, dvdReadDoneCallBack);
		return 0;
	}
}

//TODO: enum of all archive types
void fileSetCurrentArchiveType(s32 type) {
	attack_event_80424DB8->archiveType = type;
}
