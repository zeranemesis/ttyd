#include "sdk/arc.h"
#include <dolphin/os.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>

#if defined(_MSC_VER)
#define bswap32(x) _byteswap_ulong(x)
#elif defined(__GNUC__)
#define bswap32(x) __builtin_bswap32(x)
#else
#define bswap32(x) (x)
#endif

typedef struct FSTEntry {
	u32 entryInfo; // top byte is 0 file 1 dir, low 24 bits string offset
	u32 entryOffsetOrParent; // file = offset from archiveStart, dir = parent index
	u32 entrySizeOrNext; // file = size, dir = next entry
} FSTEntry;

static int arc_strcasecmp(const char* s1, const char* s2, size_t n) {
	size_t i;
	for (i = 0; i < n; i++) {
		int c1 = tolower((unsigned char)s1[i]);
		int c2 = tolower((unsigned char)s2[i]);
		if (c1 != c2) return c1 - c2;
		if (c1 == '\0') return 0;
	}
	return 0;
}

s32 ARCConvertPathToEntrynum(ARCHandle* handle, const char* path) {
	FSTEntry* entry;
	u32 currDir, nextEntry;
	const char* ptr;
	size_t nameLen;
	BOOL isDir;
	BOOL found;
	u32 i;
	const char* entryName;
	BOOL entryIsDir;

	if (!handle || !handle->FSTStart || !path) {
		return -1;
	}

	currDir = handle->currDir;
	entry = (FSTEntry*)handle->FSTStart;

	while (1) {
		if (*path == '\0') {
			return (s32)currDir;
		}
		if (*path == '/') {
			currDir = 0;
			path++;
			continue;
		}
		if (*path == '.') {
			if (*(path + 1) == '.') {
				if (*(path + 2) == '/') {
					currDir = entry[currDir].entryOffsetOrParent;
					path += 3;
					continue;
				}
				else if (*(path + 2) == '\0') {
					return (s32)entry[currDir].entryOffsetOrParent;
				}
			}
			else if (*(path + 1) == '/') {
				path += 2;
				continue;
			}
			else if (*(path + 1) == '\0') {
				return (s32)currDir;
			}
		}

		for (ptr = path; *ptr != '\0' && *ptr != '/'; ptr++) {
			;
		}

		nameLen = (size_t)(ptr - path);
		isDir = (*ptr == '/');
		nextEntry = entry[currDir].entrySizeOrNext;

		found = FALSE;
		i = currDir + 1;
		while (i < nextEntry) {
			entryName = handle->FSTStringStart + (entry[i].entryInfo & 0x00FFFFFF);
			entryIsDir = ((entry[i].entryInfo >> 24) != 0);

			if (strlen(entryName) == nameLen && arc_strcasecmp(entryName, path, nameLen) == 0) {
				if (isDir) {
					if (entryIsDir) {
						currDir = i;
						path = ptr + 1;
						found = TRUE;
						break;
					}
				}
				else {
					return (s32)i;
				}
			}

			if (entryIsDir) {
				i = entry[i].entrySizeOrNext;
			}
			else {
				i++;
			}
		}

		if (!found) {
			return -1;
		}
	}
}

BOOL ARCOpen(ARCHandle* handle, const char* filename, ARCFileInfo* info) {
	s32 entryNum;
	FSTEntry* entry;

	if (!handle || !filename || !info) {
		return FALSE;
	}
	entryNum = ARCConvertPathToEntrynum(handle, filename);
	if (entryNum < 0) {
		return FALSE;
	}
	entry = (FSTEntry*)handle->FSTStart;
	if ((entry[entryNum].entryInfo >> 24) != 0) {
		return FALSE;
	}
	info->handle = handle;
	info->startOffset = entry[entryNum].entryOffsetOrParent;
	info->length = entry[entryNum].entrySizeOrNext;
	return TRUE;
}

BOOL ARCInitHandle(void* arcStart, ARCHandle* handle) {
	FSTEntry* entry;
	ARCHeader* header;
#ifdef TARGET_PC
	u8* bytes;
	u32 count;
	u32 i;
#endif

	if (!arcStart || !handle) {
		return FALSE;
	}

	header = (ARCHeader*)arcStart;

#ifdef TARGET_PC
	bytes = (u8*)arcStart;
	// Check U8 magic: 0x55AA382D in big-endian bytes
	if (bytes[0] == 0x55 && bytes[1] == 0xAA && bytes[2] == 0x38 && bytes[3] == 0x2D) {
		// Need to swap from Big-Endian to Host Endian on Little-Endian x86_64
		header->fstStart = bswap32(header->fstStart);
		header->fstSize = bswap32(header->fstSize);
		header->fileStart = bswap32(header->fileStart);

		entry = (FSTEntry*)((uintptr_t)arcStart + header->fstStart);
		count = bswap32(entry[0].entrySizeOrNext);
		for (i = 0; i < count; i++) {
			entry[i].entryInfo = bswap32(entry[i].entryInfo);
			entry[i].entryOffsetOrParent = bswap32(entry[i].entryOffsetOrParent);
			entry[i].entrySizeOrNext = bswap32(entry[i].entrySizeOrNext);
		}

		// Mark header magic as host endian to prevent double-swapping
		header->magic = 0x55AA382D;
	}
	else
#endif
	if (header->magic != 0x55AA382D) {
		OSHalt("ARCInitHandle: bad archive format");
		return FALSE;
	}

	handle->archiveStartAddr = arcStart;
	entry = (FSTEntry*)((uintptr_t)arcStart + header->fstStart);
	handle->FSTStart = (void*)entry;
	handle->fileStart = (void*)((uintptr_t)arcStart + header->fileStart);
	handle->entryNum = entry[0].entrySizeOrNext;
	handle->FSTStringStart = (char*)&entry[handle->entryNum];
	handle->FSTLength = header->fstSize;
	handle->currDir = 0;
	return TRUE;
}

void* ARCGetStartAddrInMem(ARCFileInfo* info) {
	if (!info || !info->handle) {
		return NULL;
	}
	return (void*)((uintptr_t)info->handle->archiveStartAddr + info->startOffset);
}

u32 ARCGetLength(ARCFileInfo* info) {
	if (!info) {
		return 0;
	}
	return info->length;
}

void ARCClose(void) {
}
