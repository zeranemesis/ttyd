#pragma once

#include <dolphin/types.h>
#include "manager/evtmgr.h"

s32 evtmgrCmd(EventEntry* entry);
s32 evtGetValue(EventEntry* entry, s32 index);
s32 evtGetNumber(EventEntry* entry, s32 index);
s32 evtSetValue(EventEntry* entry, s32 index, s32 value);
f32 evtGetFloat(EventEntry* entry, s32 index);
f32 evtSetFloat(EventEntry* entry, s32 index, f32 value);
void* evtGetPtrVal(EventEntry* entry, s32 index);
const char* evtGetString(EventEntry* entry, s32 index);
#ifdef TARGET_PC
#include <stdint.h>
void* evtRestorePtr(uint32_t lower);
#endif
