#include "database.h"
#include "driver/arcdrv.h"
#include "manager/evtmgr.h"
#include "evt/evt_cmd.h"
#include "memory.h"
#include "system.h"
#include "parse.h"
#include "driver/animdrv.h"
#include "driver/npcdrv.h"
#include "data/npc_data.h"
#include "data/mapdata.h"
#include "mario/mariost.h"
#include <string.h>
#include <stdio.h>

extern GlobalWork* gp;
extern int sprintf(char* str, const char* format, ...);

//.sbss
void* ptr;
u32 size;

void setupDataLoad(const char* mapName) {
	char path[64];
	DVDEntry* dvd;
	u32 filesize;

	ptr = NULL;
	sprintf(path, "%s/m/%s/s", getMarioStDvdRoot(), mapName);
	if (strcmp(mapName, "muj_20") || evtGetValue(NULL, GSW(0)) != 2) {
		arcOpen(path, &ptr, &size);
		if (!ptr) {
			dvd = DVDMgrOpen(path, 2, 0);
			if (dvd) {
				filesize = DVDMgrGetLength(dvd);
				size = filesize;
				if (filesize) {
					ptr = _mapAlloc(OSRoundUp32B(filesize));
					DVDMgrRead(dvd, ptr, OSRoundUp32B(size), 0);
				}
				DVDMgrClose(dvd);
			}
		}
	}
}

BOOL setupDataCheck(void) {
	if (!ptr) {
		return TRUE;
	}
	parseInit(ptr, size);
	while (parsePush("<tribe>")) {
		char name[64] = {0};
		parseTagGet1("<name>", 0, name);
		parsePopNext();
		NpcTribeInfo* tribe = npcGetTribe(name);
		if (tribe && tribe->modelName) {
			if (!animGroupBaseAsync(tribe->modelName, gp->inBattle != 0, 0)) {
				return FALSE;
			}
		}
	}
	parsePop();
	return TRUE;
}

void setupDataBase(const char* area, const char* map) {
	if (!ptr) {
		return;
	}
	parseInit(ptr, size);
	while (parsePush("<Npc>")) {
		char name[64] = {0};
		char tribeName[64] = {0};
		Vec pos = {0.0f, 0.0f, 0.0f};
		f32 dir = 0.0f;
		char aiType[64] = {0};
		char territoryTypeStr[64] = {0};
		Vec terrBase = {0.0f, 0.0f, 0.0f};
		Vec terrLoiter = {0.0f, 0.0f, 0.0f};
		Vec terrHoming = {0.0f, 0.0f, 0.0f};
		f32 searchRange = 0.0f;
		f32 searchAngle = 0.0f;
		f32 homingRange = 0.0f;
		f32 homingAngle = 0.0f;
		char btlSetupNoStr[64] = {0};

		parseTagGet1("<name>", 0, name);
		parseTagGet1("<tribe>", 0, tribeName);
		parseTagGet1("<position>", 3, &pos);
		parseTagGet1("<dir>", 2, &dir);
		parseTagGet1("<aitype>", 0, aiType);
		parseTagGet1("<territory_type>", 0, territoryTypeStr);
		parseTagGet1("<territory_base>", 3, &terrBase);
		parseTagGet1("<territory_loiter>", 3, &terrLoiter);
		parseTagGet1("<territory_homing>", 3, &terrHoming);
		parseTagGet1("<search_range>", 2, &searchRange);
		parseTagGet1("<search_angle>", 2, &searchAngle);
		parseTagGet1("<homing_range>", 2, &homingRange);
		parseTagGet1("<homing_angle>", 2, &homingAngle);
		parseTagGet1("<btl_setup_no>", 0, btlSetupNoStr);
		parsePopNext();

		NpcTribeInfo* tribe = npcGetTribe(tribeName);
		if (tribe) {
			npcEntry(name, tribe->modelName);
			NpcEntry* entry = npcNameToPtr(name);
			if (entry) {
				entry->flags |= 4;
				entry->position = pos;
				entry->prevPosition = pos;
				entry->direction = dir;
				entry->tribe = tribe;
				entry->width = tribe->width;
				entry->height = tribe->height;
				if (tribe->stayAnim) {
					strncpy(entry->stayAnim, tribe->stayAnim, sizeof(entry->stayAnim) - 1);
				}
				if (tribe->talkAnim) {
					strncpy(entry->talkAnim, tribe->talkAnim, sizeof(entry->talkAnim) - 1);
				}
				if (tribe->initialAnim) {
					strncpy(entry->currentAnim, tribe->initialAnim, sizeof(entry->currentAnim) - 1);
					if (entry->poseId >= 0) {
						animPoseSetAnim(entry->poseId, entry->currentAnim, 1);
					}
				}
				entry->footstepSfxA = tribe->footstepSfxA;
				entry->footstepSfxB = tribe->footstepSfxB;
				entry->footstepStride = tribe->footstepStride;
				entry->jumpSfx = tribe->jumpSfx;
				entry->landSfx = tribe->landSfx;

				entry->territoryBase = terrBase;
				entry->territoryLoiter = terrLoiter;
				entry->territoryHoming = terrHoming;
				entry->searchRange = searchRange;
				entry->searchAngle = searchAngle;
				entry->homingRange = homingRange;
				entry->homingAngle = homingAngle;
			}
		}
	}
	parsePop();
}
