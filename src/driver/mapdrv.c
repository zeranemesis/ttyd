#include "driver/mapdrv.h"
#include "driver/arcdrv.h"
#include "driver/dispdrv.h"
#include "driver/camdrv.h"
#include "driver/offscreendrv.h"
#include "manager/dvdmgr.h"
#include "mario/mariost.h"
#include "memory.h"
#include "system.h"
#include "texPalette.h"
#include <math.h>
#include <string.h>
#ifdef TARGET_PC
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#else
#include <dolphin/types.h>
#ifndef uintptr_t
typedef u32 uintptr_t;
#endif
#endif

extern f32 queen_uv[4]; //gxsub.c
extern GlobalWork* gp;
extern int sprintf(char* str, const char* format, ...);

#define TWO_PI 6.28318530717958647692528676655900576839433879875021f
#define DEG_TO_RAD 0.017453292519943295f

static inline u32 swap32_m(u32 v) {
#if defined(_MSC_VER)
    return _byteswap_ulong(v);
#else
    return __builtin_bswap32(v);
#endif
}

static inline f32 swapF32_m(u32 v) {
    u32 s = swap32_m(v);
    f32 f;
    memcpy(&f, &s, 4);
    return f;
}

//.bss
MapError error_data[32];

//.sbss
MapWork* mapWork; //allocates two entries
s32 activeGroup;
BOOL mapClipOffFlag;
MapObject* current_mp;
BOOL error_flag;
s32 error_count;
BOOL disableMapDraw;
BOOL __mapdrv_make_dl;
Mtx projMtx;
void* projTexImgRef;
GXTexObj paperCraft_ambient;
//more stuff

//.sdata
f32 scrl_uv[4] = {0.0f, 0.0f, 0.0f, 0.0f};
s32 fog_type[5] = {2, 4, 5, 6, 7};

//local prototypes
void mapGetJointsSub(MapFileJoint* joint, s32* numJoints);
void mapGetBoundingBox(Vec min, Vec max);
void mapBuildTexture(MapObject* obj, TEXPalette* texture, void* table);
MapObject* _mapEnt(MapFileJoint* joint, MapObject* parent, Mtx mtx, s32 index);
MapObject* mapEntrySub(MapFileJoint* joint, MapObject* parent, Mtx mtx, BOOL isRoot, s32 index);
MapObject* mapEntry(MapFileJoint* joint, Mtx mtx, s32 index);
void makeDisplayList(BOOL group);
//anim functions?
void _mapLoad(MapWork* wp, BOOL group, const char* map);

static inline void _mapErrorEntry(s32 arg0, const char* arg1) {
	error_data[error_count].field_0x20 = arg0;
	strncpy(error_data[error_count].field_0x0, arg1, sizeof(error_data[0].field_0x0) - 1);
	error_flag = TRUE;
	error_count++;
	error_count %= 32;
}

static inline MapObject* _mapGetMapObj(const char* name) {
    MapObject* obj;
    int i;
    int j;
    MapEntry* entry;
    MapWork* wp;

    wp = &mapWork[activeGroup];
    if (!name) {
        obj = NULL;
        goto done;
    }
    for (i = 0, entry = wp->entries; i < wp->entries[0].count; entry++, i++) {
        for (j = 0, obj = entry->objects; j < entry->numJoints; j++, obj++) {
            if (!strcmp(obj->joints->name, name)) {
                goto done;
            }
        }
    }
    obj = NULL;
done:
    return obj;
}

void mapErrorEntry(s32 arg0, const char* arg1) { //1:1
	_mapErrorEntry(arg0, arg1);
}

const char* getMapDataDvdRoot(void) {
	return ".";
}

void N_mapDispOff(void) {
	disableMapDraw = TRUE;
}

void N_mapDispOn(void) {
	disableMapDraw = FALSE;
}

/* Candidate entrypoints declared in mapdrv.h. The N_ variants above
 * already control the map draw gate; verify their retail linkage and
 * generated code before marking this object as matching.
 */
void mapDispOff(void) {
    N_mapDispOff();
}

void mapDispOn(void) {
    N_mapDispOn();
}

MapWork* mapGetWork(void) {
	return &mapWork[activeGroup];
}

s32 mapGetActiveGroup(void) {
	return activeGroup;
}

//heavily inlined
void mapGetJointsSub(MapFileJoint* joint, s32* numJoints) { //1:1
	(*numJoints)++;
	if (joint->child) {
		mapGetJointsSub(joint->child, numJoints);
	}
	if (joint->next) {
		mapGetJointsSub(joint->next, numJoints);
	}
}

s32 mapGetJoints(MapFileJoint* joint) { //1:1
	s32 numJoints;

	numJoints = 0;
	if (joint) {
		numJoints = 1;
		if (joint->child) {
			mapGetJointsSub(joint->child, &numJoints);
		}
	}
	return numJoints;
}

void mapGetBoundingBox(Vec min, Vec max) {
	Vec vec;
	MapFileJoint* joint;
	MapObject* obj;
	MapWork* wp;
	int i;

	min = (Vec){0.0f, 0.0f, 0.0f};
	max = (Vec){0.0f, 0.0f, 0.0f};
	if (!mapWork) return;
	wp = &mapWork[activeGroup];
	if (!wp || !wp->entries[0].objects || wp->entries[0].numJoints <= 0) return;
	obj = wp->entries[0].objects;
	for (i = 0; i < wp->entries[0].numJoints; i++, obj++) {
		joint = obj->joints;
		if (joint && joint->partCount) {
			MTXMultVec(obj->modelWorldMtx, &joint->bboxMin, &vec);
			if (min.x > vec.x) {
				min.x = vec.x;
			}
			if (min.y > vec.y) {
				min.y = vec.y;
			}
			if (min.z > vec.z) {
				min.z = vec.z;
			}
			if (max.x < vec.x) {
				max.x = vec.x;
			}
			if (max.y < vec.y) {
				max.y = vec.y;
			}
			if (max.z < vec.z) {
				max.z = vec.z;
			}
			MTXMultVec(obj->modelWorldMtx, &obj->joints->bboxMax, &vec);
			if (min.x > vec.x) {
				min.x = vec.x;
			}
			if (min.y > vec.y) {
				min.y = vec.y;
			}
			if (min.z > vec.z) {
				min.z = vec.z;
			}
			if (max.x < vec.x) {
				max.x = vec.x;
			}
			if (max.y < vec.y) {
				max.y = vec.y;
			}
			if (max.z < vec.z) {
				max.z = vec.z;
			}
		}
	}
}

void mapInit(void) {
	mapWork = __memAlloc(HEAP_DEFAULT, sizeof(MapWork) * 2);
	memset(mapWork, 0, sizeof(MapWork) * 2);
	activeGroup = 0;
	mapClipOffFlag = FALSE;
}

void mapBuildTexture(MapObject* obj, TEXPalette* texture, void* table) {
	int i;
	MapFileJointPart* part;

	for (i = 0; i < obj->joints->partCount; i++) {
		part = &obj->joints->parts[i];
	}
	if (obj->child) {
		mapBuildTexture(obj->child, texture, table);
	}
	if (obj->next) {
		mapBuildTexture(obj->next, texture, table);
	}
}

static s32 mapFindTextureIndex(const char* name, u8* texTableRaw, u8* dataBase) {
	u32 texCount;
	u32 i;
	u32 s_off;
	const char* tName;

	if (!name || !texTableRaw) return -1;
	texCount = swap32_m(*(u32*)texTableRaw);
	for (i = 0; i < texCount; i++) {
		s_off = swap32_m(*(u32*)(texTableRaw + 4 + i * 4));
		tName = (const char*)(dataBase + s_off);
		if (!strcmp(tName, name)) {
			return (s32)i;
		}
	}
	return -1;
}

static void mapSetupPartTexture(MapFileJointPart* part, u8* dataBase, u8* texTableRaw, TEXPalette* tpl) {
	u8* matRaw;
	u8 texCount;
	u32 texDescTableOff;
	u32 texInfoOff;
	u32 nameOff;
	const char* texName;
	s32 texIdx;

	part->hasTex = FALSE;
	part->texIdx = -1;
	if (!part->material || !tpl || !texTableRaw) return;

	matRaw = (u8*)part->material;
	texCount = matRaw[11];
	if (texCount == 0) return;

	texDescTableOff = swap32_m(*(u32*)(matRaw + 12));
	if (texDescTableOff == 0) return;

	texInfoOff = swap32_m(*(u32*)(dataBase + texDescTableOff));
	if (texInfoOff == 0) return;

	nameOff = swap32_m(*(u32*)(dataBase + texInfoOff));
	if (nameOff == 0) return;

	texName = (const char*)(dataBase + nameOff);
	texIdx = mapFindTextureIndex(texName, texTableRaw, dataBase);
	if (texIdx >= 0) {
		part->hasTex = TRUE;
		part->texIdx = texIdx;
		TEXGetGXTexObjFromPalette(tpl, &part->texObj, (u32)texIdx);
	}
}

static MapFileJoint* parse_on_disk_joint(u8* dataBase, u32 jointOffset, MapFileJoint* parent, u8* texTableRaw, TEXPalette* tpl) {
	u8* raw;
	u32 nameOff, typeOff, childOff, nextOff;
	s32 partCount;
	MapFileJoint* j;
	int p, d;
	u32 matOff, meshOff;
	u8* mRaw;
	s32 dlCount;
	u32 elemMask, vcdOff;
	MapFileMesh* mesh;
	u32 dlOff, dlSize;

	if (jointOffset == 0) return NULL;
	raw = dataBase + jointOffset;

	nameOff = swap32_m(*(u32*)(raw + 0x00));
	typeOff = swap32_m(*(u32*)(raw + 0x04));
	childOff = swap32_m(*(u32*)(raw + 0x0C));
	nextOff = swap32_m(*(u32*)(raw + 0x10));

	partCount = (s32)swap32_m(*(u32*)(raw + 0x5C));
	if (partCount < 0 || partCount > 256) partCount = 0;

	j = (MapFileJoint*)__memAlloc(HEAP_DEFAULT, sizeof(MapFileJoint) + partCount * sizeof(MapFileJointPart));
	if (!j) return NULL;
	memset(j, 0, sizeof(MapFileJoint) + partCount * sizeof(MapFileJointPart));

	j->name = (const char*)(dataBase + nameOff);
	if (typeOff != 0) j->type = (const char*)(dataBase + typeOff);
	j->parent = parent;

	j->scale.x = swapF32_m(*(u32*)(raw + 0x18));
	j->scale.y = swapF32_m(*(u32*)(raw + 0x1C));
	j->scale.z = swapF32_m(*(u32*)(raw + 0x20));

	j->rotation.x = swapF32_m(*(u32*)(raw + 0x24));
	j->rotation.y = swapF32_m(*(u32*)(raw + 0x28));
	j->rotation.z = swapF32_m(*(u32*)(raw + 0x2C));

	j->translation.x = swapF32_m(*(u32*)(raw + 0x30));
	j->translation.y = swapF32_m(*(u32*)(raw + 0x34));
	j->translation.z = swapF32_m(*(u32*)(raw + 0x38));

	j->bboxMin.x = swapF32_m(*(u32*)(raw + 0x3C));
	j->bboxMin.y = swapF32_m(*(u32*)(raw + 0x40));
	j->bboxMin.z = swapF32_m(*(u32*)(raw + 0x44));

	j->bboxMax.x = swapF32_m(*(u32*)(raw + 0x48));
	j->bboxMax.y = swapF32_m(*(u32*)(raw + 0x4C));
	j->bboxMax.z = swapF32_m(*(u32*)(raw + 0x50));

	j->partCount = partCount;
	for (p = 0; p < partCount; p++) {
		matOff = swap32_m(*(u32*)(raw + 0x60 + p * 8));
		meshOff = swap32_m(*(u32*)(raw + 0x64 + p * 8));

		if (matOff != 0) {
			j->parts[p].material = (void*)(dataBase + matOff);
			mapSetupPartTexture(&j->parts[p], dataBase, texTableRaw, tpl);
		}

		if (meshOff != 0) {
			mRaw = dataBase + meshOff;
			dlCount = (s32)swap32_m(*(u32*)(mRaw + 0x04));
			elemMask = swap32_m(*(u32*)(mRaw + 0x08));
			vcdOff = swap32_m(*(u32*)(mRaw + 0x0C));
			if (dlCount < 0 || dlCount > 512) dlCount = 0;

			mesh = (MapFileMesh*)__memAlloc(HEAP_DEFAULT, sizeof(MapFileMesh) + dlCount * sizeof(PackedDisplayList));
			if (!mesh) continue;
			memset(mesh, 0, sizeof(MapFileMesh) + dlCount * sizeof(PackedDisplayList));
			mesh->isPackedDisplay = mRaw[0x03];
			mesh->displayListCount = dlCount;
			mesh->elementMask = elemMask;
			mesh->vcdTableOffset = vcdOff;

			for (d = 0; d < dlCount; d++) {
				dlOff = swap32_m(*(u32*)(mRaw + 0x10 + d * 8));
				dlSize = swap32_m(*(u32*)(mRaw + 0x14 + d * 8));
				mesh->displayLists.packedLists[d].offset = (uintptr_t)(dataBase + dlOff);
				mesh->displayLists.packedLists[d].size = dlSize;
			}
			j->parts[p].mesh = mesh;
		}
	}

	j->child = parse_on_disk_joint(dataBase, childOff, j, texTableRaw, tpl);
	j->next = parse_on_disk_joint(dataBase, nextOff, parent, texTableRaw, tpl);
	return j;
}

MapFileJoint* mapSearchDmdJoint(MapFileJoint* joint, const char* name) {
	if (!joint || !name) return NULL;
	if (joint->name && !strcmp(joint->name, name)) {
		return joint;
	}
	if (joint->child) {
		MapFileJoint* res = mapSearchDmdJoint(joint->child, name);
		if (res) return res;
	}
	if (joint->next) {
		MapFileJoint* res = mapSearchDmdJoint(joint->next, name);
		if (res) return res;
	}
	return NULL;
}

MapObject* _mapEnt(MapFileJoint* joint, MapObject* parent, Mtx mtx, s32 index) {
	MapWork* wp = &mapWork[activeGroup];
	MapEntry* entry = &wp->entries[index];
	MapObject* obj = NULL;
	int i;
	Mtx tMtx, sMtx, rxMtx, ryMtx, rzMtx;
	Mtx localMtx;

	for (i = 0; i < entry->numJoints; i++) {
		if (!entry->objects[i].joints) {
			obj = &entry->objects[i];
			break;
		}
	}
	if (!obj) return NULL;

	obj->joints = joint;
	obj->parent = parent;
	obj->cameraId = 4; // CAMERA_3D

	PSMTXTrans(tMtx, joint->translation.x, joint->translation.y, joint->translation.z);
	PSMTXScale(sMtx, joint->scale.x, joint->scale.y, joint->scale.z);
	PSMTXRotRad(rxMtx, 'x', joint->rotation.x * DEG_TO_RAD);
	PSMTXRotRad(ryMtx, 'y', joint->rotation.y * DEG_TO_RAD);
	PSMTXRotRad(rzMtx, 'z', joint->rotation.z * DEG_TO_RAD);

	PSMTXConcat(tMtx, rzMtx, localMtx);
	PSMTXConcat(localMtx, ryMtx, localMtx);
	PSMTXConcat(localMtx, rxMtx, localMtx);
	PSMTXConcat(localMtx, sMtx, localMtx);

	if (mtx) {
		PSMTXConcat(mtx, localMtx, obj->modelWorldMtx);
	} else {
		PSMTXCopy(localMtx, obj->modelWorldMtx);
	}
	PSMTXIdentity(obj->unkAC);
	return obj;
}

//recursively inlined
MapObject* mapEntrySub(MapFileJoint* joint, MapObject* parent, Mtx mtx, BOOL isRoot, s32 index) {
	MapObject* obj;

	if (!joint) return NULL;
	obj = _mapEnt(joint, parent, mtx, index);
	if (!obj) return NULL;

	if (joint->child) {
		obj->child = mapEntrySub(joint->child, obj, obj->modelWorldMtx, FALSE, index);
	}
	if (!isRoot) {
		if (joint->next) {
			obj->next = mapEntrySub(joint->next, parent, mtx, FALSE, index);
		}
	}
	return obj;
}

MapObject* mapEntry(MapFileJoint* joint, Mtx mtx, s32 index) {
	return mapEntrySub(joint, NULL, mtx, TRUE, index);
}

void makeDisplayList(BOOL group) {

}

MapEntryAnimData* mapSearchAnmObj(const char* animName) {
	return NULL;



}

void mapCheckAnimation(const char* animName, BOOL* valid, f32* a3) {

}

//more animation functions

void _mapLoad(MapWork* wp, BOOL group, const char* map) {
	MapHeader* header;
	MapEntry* entry;
	DVDEntry* dvd;
	char str1[128];
	char str2[128];
	char str3[128];
	u32 sizeCheck;
	u8 *data;
	u32 r4, r7, r0;
	int i;
	MapFileInfo* info;
	u8* texTableRaw;
	u8* dataBase;
	u32 rootJointOffset;
	u32 mapNameOffset;
	const char* mapName;
	MapFileJoint* rootJoint;
	MapFileJoint* mapRoot;
	Mtx scaleMtx;
	BOOL isBE;
	u32 swappedSize;

	entry = &wp->entries[group];
	error_flag = FALSE;
	error_count = 0;
	strncpy(entry->name, map, sizeof(entry->name));

	sprintf(str1, "./m/%s/d", map);
	arcOpen(str1, &entry->dat, &entry->datSize);
	if (!entry->dat) {
		dvd = DVDMgrOpen(str1, 2, 0);
		if (!dvd) {
			sprintf(str2, "%s/%s", ".", str1);
			dvd = DVDMgrOpen(str2, 2, 0);
		}
		if (dvd) {
			entry->datSize = DVDMgrGetLength(dvd);
			entry->dat = _mapAllocTail(OSRoundUp32B(entry->datSize));
			DVDMgrRead(dvd, entry->dat, OSRoundUp32B(entry->datSize), 0);
			DVDMgrClose(dvd);
		}
	}

	sprintf(str1, "./m/%s/t", map);
	entry->tpl = NULL;
	arcOpen(str1, &entry->tpl, &entry->tplSize);
	if (entry->tpl) {
		UnpackTexPalette(entry->tpl);
	}
	else {
		dvd = DVDMgrOpen(str1, 2, 0);
		if (!dvd) {
			sprintf(str3, "%s/%s", ".", str1);
			dvd = DVDMgrOpen(str3, 2, 0);
		}
		if (dvd) {
			entry->tplSize = DVDMgrGetLength(dvd);
			entry->tpl = _mapAllocTail(OSRoundUp32B(entry->tplSize));
			DVDMgrRead(dvd, entry->tpl, OSRoundUp32B(entry->tplSize), 0);
			DVDMgrClose(dvd);
			UnpackTexPalette(entry->tpl);
		}
	}

	if (!entry->dat) {
#ifdef TARGET_PC
		printf("[_mapLoad] Notice: Geometry binary for map '%s' not present on DVD.\n", map);
		fflush(stdout);
#endif
		return;
	}

	GXInvalidateVtxCache();
	header = &entry->header;
	data = (u8*)entry->dat;
	if (header != NULL) {
		memset(header, 0, sizeof(MapHeader));
		header->field_0x3C |= 1;
		memcpy(header, entry->dat, sizeof(MapFileHeader));
		isBE = FALSE;
		swappedSize = ((header->file.fileSize >> 24) & 0xFF) |
		              ((header->file.fileSize << 8) & 0xFF0000) |
		              ((header->file.fileSize >> 8) & 0xFF00) |
		              ((header->file.fileSize << 24) & 0xFF000000);
		if (header->file.fileSize != entry->datSize && swappedSize == entry->datSize) {
			isBE = TRUE;
			#define SWAP32(x) (((x >> 24) & 0xFF) | ((x << 8) & 0xFF0000) | ((x >> 8) & 0xFF00) | ((x << 24) & 0xFF000000))
			header->file.fileSize = SWAP32(header->file.fileSize);
			header->file.dataSize = SWAP32(header->file.dataSize);
			header->file.relCount = SWAP32(header->file.relCount);
			header->file.chunkCount = SWAP32(header->file.chunkCount);
			header->file.unused = SWAP32(header->file.unused);
		}
		if (header->file.fileSize == entry->datSize) {
			sizeCheck = sizeof(MapFileHeader);
			if (header->file.dataSize) {
				header->data = &data[sizeCheck];
				sizeCheck = header->file.dataSize + sizeof(MapFileHeader);
			}
			if (header->file.relCount) {
				header->rel = &data[sizeCheck];
				sizeCheck += 4 * header->file.relCount;
			}
			if (header->file.chunkCount) {
				header->chunks = (MapFileChunk*)&data[sizeCheck];
				sizeCheck += sizeof(MapFileChunk) * header->file.chunkCount;
			}
			if (header->file.unused) {
				header->unused = &data[sizeCheck];
				sizeCheck += 8 * header->file.unused;
			}
			if (sizeCheck < header->file.fileSize) {
				header->strings = (const char*)&data[sizeCheck];
			}

			if (isBE && header->chunks) {
				for (i = 0; i < header->file.chunkCount; i++) {
					header->chunks[i].offset = SWAP32(header->chunks[i].offset);
					header->chunks[i].string = SWAP32(header->chunks[i].string);
				}
			}
		}
	}
	info = NULL;
	texTableRaw = NULL;
	dataBase = (u8*)entry->header.data;
	if (header && header->strings && header->chunks && dataBase) {
		for (i = 0; i < header->file.chunkCount; i++) {
			const char* chunkName = &header->strings[header->chunks[i].string];
			if (!strcmp(chunkName, "information")) {
				info = (MapFileInfo*)&dataBase[header->chunks[i].offset];
			}
			else if (!strcmp(chunkName, "texture_table")) {
				texTableRaw = &dataBase[header->chunks[i].offset];
			}
		}
	}

	if (info && dataBase) {
		rootJointOffset = swap32_m(*(u32*)((u8*)info + 4));
		mapNameOffset = swap32_m(*(u32*)((u8*)info + 8));
		mapName = (const char*)(dataBase + mapNameOffset);

#ifdef TARGET_PC
		printf("[_mapLoad] Parsing map scene graph: rootJointOffset=0x%X, mapName='%s'\n", rootJointOffset, mapName);
		fflush(stdout);
#endif

		rootJoint = parse_on_disk_joint(dataBase, rootJointOffset, NULL, texTableRaw, entry->tpl);
		mapRoot = mapSearchDmdJoint(rootJoint, mapName);
		if (!mapRoot) mapRoot = rootJoint;

		entry->numJoints = mapGetJoints(mapRoot);
		if (entry->numJoints > 0) {
			entry->objects = (MapObject*)__memAlloc(HEAP_DEFAULT, entry->numJoints * sizeof(MapObject));
			if (entry->objects) {
				memset(entry->objects, 0, entry->numJoints * sizeof(MapObject));
			}
		}

		PSMTXScale(scaleMtx, 1.0f, 1.0f, 1.0f);
		entry->rootMapObj = mapEntrySub(mapRoot, NULL, scaleMtx, TRUE, group);
		entry->count = 1;
#ifdef TARGET_PC
		printf("[_mapLoad] Map '%s' loaded: %d joints, %p rootMapObj\n", map, entry->numJoints, entry->rootMapObj);
		fflush(stdout);
#endif
	}
}

void mapUnLoad(void) {

}

BOOL mapPreLoad(void) {
	return TRUE; //do nothing
}

void mapLoad(const char* map) {
	MapWork* wp;

	sysWaitDrawSync();
	wp = &mapWork[0];
	activeGroup = 0;
	memset(wp, 0, sizeof(MapWork));
	_mapLoad(&mapWork[0], 0, map);
	wp->entries[0].count = 1;
}






void mapSetMaterialTev(s32 texCount, s32 drawMode, s32 flags, Mtx mtx) {
	GXTevStageID tevStage;
	Mtx queen_mtx;

	if (texCount > 0) {
		switch (drawMode) {
			case 0:
				GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR0A0);
				GXSetTevOp(GX_TEVSTAGE0, GX_MODULATE);
				GXSetTevSwapMode(GX_TEVSTAGE0, GX_TEV_SWAP0, GX_TEV_SWAP0);
				tevStage = GX_TEVSTAGE1;
				break;

			case 1:
				GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR_NULL);
				GXSetTevColorOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
				GXSetTevAlphaOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
				GXSetTevColorIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_ZERO, GX_CC_ZERO, GX_CC_TEXC);
				GXSetTevAlphaIn(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_TEXA);
				GXSetTevSwapMode(GX_TEVSTAGE0, GX_TEV_SWAP0, GX_TEV_SWAP0);
				GXSetTevOrder(GX_TEVSTAGE1, GX_TEXCOORD1, GX_TEXMAP1, GX_COLOR_NULL);
				GXSetTevColorOp(GX_TEVSTAGE1, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
				GXSetTevAlphaOp(GX_TEVSTAGE1, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
				GXSetTevColorIn(GX_TEVSTAGE1, GX_CC_CPREV, GX_CC_TEXC, GX_CC_TEXA, GX_CC_ZERO);
				GXSetTevAlphaIn(GX_TEVSTAGE1, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_APREV);
				GXSetTevSwapMode(GX_TEVSTAGE1, GX_TEV_SWAP0, GX_TEV_SWAP0);
				GXSetTevOrder(GX_TEVSTAGE2, GX_TEXCOORD_NULL, GX_TEXMAP_NULL, GX_COLOR0A0);
				GXSetTevColorOp(GX_TEVSTAGE2, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
				GXSetTevAlphaOp(GX_TEVSTAGE2, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
				GXSetTevColorIn(GX_TEVSTAGE2, GX_CC_ZERO, GX_CC_CPREV, GX_CC_RASC, GX_CC_ZERO);
				GXSetTevAlphaIn(GX_TEVSTAGE2, GX_CA_ZERO, GX_CA_APREV, GX_CA_RASA, GX_CA_ZERO);
				GXSetTevSwapMode(GX_TEVSTAGE2, GX_TEV_SWAP0, GX_TEV_SWAP0);
				tevStage = GX_TEVSTAGE3;
				break;

			case 2:
				GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR_NULL);
				GXSetTevColorOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
				GXSetTevAlphaOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
				GXSetTevColorIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_ZERO, GX_CC_ZERO, GX_CC_TEXC);
				GXSetTevAlphaIn(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_TEXA);
				GXSetTevSwapMode(GX_TEVSTAGE0, GX_TEV_SWAP0, GX_TEV_SWAP0);
				GXSetTevOrder(GX_TEVSTAGE1, GX_TEXCOORD1, GX_TEXMAP1, GX_COLOR_NULL);
				GXSetTevColorOp(GX_TEVSTAGE1, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
				GXSetTevAlphaOp(GX_TEVSTAGE1, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
				GXSetTevColorIn(GX_TEVSTAGE1, GX_CC_ZERO, GX_CC_CPREV, GX_CC_TEXA, GX_CC_ZERO);
				GXSetTevAlphaIn(GX_TEVSTAGE1, GX_CA_ZERO, GX_CA_APREV, GX_CA_TEXA, GX_CA_ZERO);
				GXSetTevSwapMode(GX_TEVSTAGE1, GX_TEV_SWAP0, GX_TEV_SWAP0);
				GXSetTevOrder(GX_TEVSTAGE2, GX_TEXCOORD_NULL, GX_TEXMAP_NULL, GX_COLOR0A0);
				GXSetTevColorOp(GX_TEVSTAGE2, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
				GXSetTevAlphaOp(GX_TEVSTAGE2, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
				GXSetTevColorIn(GX_TEVSTAGE2, GX_CC_ZERO, GX_CC_CPREV, GX_CC_RASC, GX_CC_ZERO);
				GXSetTevAlphaIn(GX_TEVSTAGE2, GX_CA_ZERO, GX_CA_APREV, GX_CA_RASA, GX_CA_ZERO);
				GXSetTevSwapMode(GX_TEVSTAGE2, GX_TEV_SWAP0, GX_TEV_SWAP0);
				tevStage = GX_TEVSTAGE3;
				break;

			case 12:
				GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR_NULL);
				GXSetTevColorOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
				GXSetTevAlphaOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
				GXSetTevColorIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_ZERO, GX_CC_ZERO, GX_CC_TEXC);
				GXSetTevAlphaIn(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO);
				GXSetTevSwapMode(GX_TEVSTAGE0, GX_TEV_SWAP0, GX_TEV_SWAP0);
				GXSetTevOrder(GX_TEVSTAGE1, GX_TEXCOORD1, GX_TEXMAP1, GX_COLOR_NULL);
				GXSetTevColorOp(GX_TEVSTAGE1, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
				GXSetTevAlphaOp(GX_TEVSTAGE1, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
				GXSetTevColorIn(GX_TEVSTAGE1, GX_CC_ZERO, GX_CC_ZERO, GX_CC_ZERO, GX_CC_CPREV);
				GXSetTevAlphaIn(GX_TEVSTAGE1, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_TEXA);
				GXSetTevSwapMode(GX_TEVSTAGE1, GX_TEV_SWAP0, GX_TEV_SWAP0);
				GXSetTevOrder(GX_TEVSTAGE2, GX_TEXCOORD_NULL, GX_TEXMAP_NULL, GX_COLOR0A0);
				GXSetTevColorOp(GX_TEVSTAGE2, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
				GXSetTevAlphaOp(GX_TEVSTAGE2, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
				GXSetTevColorIn(GX_TEVSTAGE2, GX_CC_ZERO, GX_CC_CPREV, GX_CC_RASC, GX_CC_ZERO);
				GXSetTevAlphaIn(GX_TEVSTAGE2, GX_CA_ZERO, GX_CA_APREV, GX_CA_RASA, GX_CA_ZERO);
				GXSetTevSwapMode(GX_TEVSTAGE2, GX_TEV_SWAP0, GX_TEV_SWAP0);
				tevStage = GX_TEVSTAGE3;
				break;

			case 3:
				GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR_NULL);
				GXSetTevColorOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
				GXSetTevAlphaOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
				GXSetTevColorIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_ZERO, GX_CC_ZERO, GX_CC_TEXC);
				GXSetTevAlphaIn(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_TEXA);
				GXSetTevSwapMode(GX_TEVSTAGE0, GX_TEV_SWAP0, GX_TEV_SWAP0);
				GXSetTevOrder(GX_TEVSTAGE1, GX_TEXCOORD1, GX_TEXMAP1, GX_COLOR_NULL);
				GXSetTevColorOp(GX_TEVSTAGE1, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
				GXSetTevAlphaOp(GX_TEVSTAGE1, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
				GXSetTevColorIn(GX_TEVSTAGE1, GX_CC_CPREV, GX_CC_ZERO, GX_CC_TEXA, GX_CC_ZERO);
				GXSetTevAlphaIn(GX_TEVSTAGE1, GX_CA_APREV, GX_CA_ZERO, GX_CA_TEXA, GX_CA_ZERO);
				GXSetTevSwapMode(GX_TEVSTAGE1, GX_TEV_SWAP0, GX_TEV_SWAP0);
				GXSetTevOrder(GX_TEVSTAGE2, GX_TEXCOORD_NULL, GX_TEXMAP_NULL, GX_COLOR0A0);
				GXSetTevColorOp(GX_TEVSTAGE2, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
				GXSetTevAlphaOp(GX_TEVSTAGE2, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
				GXSetTevColorIn(GX_TEVSTAGE2, GX_CC_ZERO, GX_CC_CPREV, GX_CC_RASC, GX_CC_ZERO);
				GXSetTevAlphaIn(GX_TEVSTAGE2, GX_CA_ZERO, GX_CA_APREV, GX_CA_RASA, GX_CA_ZERO);
				GXSetTevSwapMode(GX_TEVSTAGE2, GX_TEV_SWAP0, GX_TEV_SWAP0);
				tevStage = GX_TEVSTAGE3;
				break;

			case 4:
				GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR_NULL);
				GXSetTevColorOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
				GXSetTevAlphaOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
				GXSetTevColorIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_ZERO, GX_CC_ZERO, GX_CC_TEXC);
				GXSetTevAlphaIn(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_TEXA);
				GXSetTevSwapMode(GX_TEVSTAGE0, GX_TEV_SWAP0, GX_TEV_SWAP0);
				GXSetTevOrder(GX_TEVSTAGE1, GX_TEXCOORD1, GX_TEXMAP1, GX_COLOR_NULL);
				GXSetTevColorOp(GX_TEVSTAGE1, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
				GXSetTevAlphaOp(GX_TEVSTAGE1, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
				GXSetTevColorIn(GX_TEVSTAGE1, GX_CC_ZERO, GX_CC_CPREV, GX_CC_TEXC, GX_CC_ZERO);
				GXSetTevAlphaIn(GX_TEVSTAGE1, GX_CA_ZERO, GX_CA_APREV, GX_CA_TEXA, GX_CA_ZERO);
				GXSetTevSwapMode(GX_TEVSTAGE1, GX_TEV_SWAP0, GX_TEV_SWAP0);
				GXSetTevOrder(GX_TEVSTAGE2, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR_NULL);
				GXSetTevColorOp(GX_TEVSTAGE2, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
				GXSetTevAlphaOp(GX_TEVSTAGE2, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
				GXSetTevColorIn(GX_TEVSTAGE2, GX_CC_TEXC, GX_CC_CPREV, GX_CC_APREV, GX_CC_ZERO);
				GXSetTevAlphaIn(GX_TEVSTAGE2, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_TEXA);
				GXSetTevSwapMode(GX_TEVSTAGE2, GX_TEV_SWAP0, GX_TEV_SWAP0);
				tevStage = GX_TEVSTAGE3;
				break;

			case 5:
				GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR_NULL);
				GXSetTevColorOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVREG0);
				GXSetTevAlphaOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVREG0);
				GXSetTevColorIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_ZERO, GX_CC_ZERO, GX_CC_TEXC);
				GXSetTevAlphaIn(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_TEXA);
				GXSetTevSwapMode(GX_TEVSTAGE0, GX_TEV_SWAP0, GX_TEV_SWAP0);
				GXSetTevOrder(GX_TEVSTAGE1, GX_TEXCOORD1, GX_TEXMAP1, GX_COLOR_NULL);
				GXSetTevColorOp(GX_TEVSTAGE1, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVREG1);
				GXSetTevAlphaOp(GX_TEVSTAGE1, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVREG1);
				GXSetTevColorIn(GX_TEVSTAGE1, GX_CC_ZERO, GX_CC_ZERO, GX_CC_ZERO, GX_CC_TEXC);
				GXSetTevAlphaIn(GX_TEVSTAGE1, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_TEXA);
				GXSetTevSwapMode(GX_TEVSTAGE1, GX_TEV_SWAP0, GX_TEV_SWAP0);
				GXSetTevOrder(GX_TEVSTAGE2, GX_TEXCOORD2, GX_TEXMAP2, GX_COLOR_NULL);
				GXSetTevColorOp(GX_TEVSTAGE2, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
				GXSetTevAlphaOp(GX_TEVSTAGE2, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
				GXSetTevColorIn(GX_TEVSTAGE2, GX_CC_C0, GX_CC_C1, GX_CC_TEXA, GX_CC_ZERO);
				GXSetTevAlphaIn(GX_TEVSTAGE2, GX_CA_A0, GX_CA_A1, GX_CA_TEXA, GX_CA_ZERO);
				GXSetTevSwapMode(GX_TEVSTAGE2, GX_TEV_SWAP0, GX_TEV_SWAP0);
				GXSetTevOrder(GX_TEVSTAGE3, GX_TEXCOORD_NULL, GX_TEXMAP_NULL, GX_COLOR0A0);
				GXSetTevColorOp(GX_TEVSTAGE3, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
				GXSetTevAlphaOp(GX_TEVSTAGE3, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
				GXSetTevColorIn(GX_TEVSTAGE3, GX_CC_ZERO, GX_CC_CPREV, GX_CC_RASC, GX_CC_ZERO);
				GXSetTevAlphaIn(GX_TEVSTAGE3, GX_CA_ZERO, GX_CA_APREV, GX_CA_RASA, GX_CA_ZERO);
				GXSetTevSwapMode(GX_TEVSTAGE3, GX_TEV_SWAP0, GX_TEV_SWAP0);
				tevStage = GX_TEVSTAGE4;
				break;

			case 6:
				GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR_NULL);
				GXSetTevColorOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
				GXSetTevAlphaOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
				GXSetTevColorIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_ZERO, GX_CC_ZERO, GX_CC_TEXC);
				GXSetTevAlphaIn(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_TEXA);
				GXSetTevSwapMode(GX_TEVSTAGE0, GX_TEV_SWAP0, GX_TEV_SWAP0);
				GXSetTevOrder(GX_TEVSTAGE1, GX_TEXCOORD1, GX_TEXMAP1, GX_COLOR_NULL);
				GXSetTevColorOp(GX_TEVSTAGE1, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
				GXSetTevAlphaOp(GX_TEVSTAGE1, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
				GXSetTevColorIn(GX_TEVSTAGE1, GX_CC_CPREV, GX_CC_TEXC, GX_CC_APREV, GX_CC_ZERO);
				GXSetTevAlphaIn(GX_TEVSTAGE1, GX_CA_ZERO, GX_CA_TEXA, GX_CA_APREV, GX_CA_ZERO);
				GXSetTevSwapMode(GX_TEVSTAGE1, GX_TEV_SWAP0, GX_TEV_SWAP0);
				GXSetTevOrder(GX_TEVSTAGE2, GX_TEXCOORD_NULL, GX_TEXMAP_NULL, GX_COLOR0A0);
				GXSetTevColorOp(GX_TEVSTAGE2, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
				GXSetTevAlphaOp(GX_TEVSTAGE2, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
				GXSetTevColorIn(GX_TEVSTAGE2, GX_CC_ZERO, GX_CC_CPREV, GX_CC_RASC, GX_CC_ZERO);
				GXSetTevAlphaIn(GX_TEVSTAGE2, GX_CA_ZERO, GX_CA_APREV, GX_CA_RASA, GX_CA_ZERO);
				GXSetTevSwapMode(GX_TEVSTAGE2, GX_TEV_SWAP0, GX_TEV_SWAP0);
				tevStage = GX_TEVSTAGE3;
				break;

			case 7:
				GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR_NULL);
				GXSetTevColorOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
				GXSetTevAlphaOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
				GXSetTevColorIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_ZERO, GX_CC_ZERO, GX_CC_TEXC);
				GXSetTevAlphaIn(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_TEXA);
				GXSetTevSwapMode(GX_TEVSTAGE0, GX_TEV_SWAP0, GX_TEV_SWAP0);
				GXSetTevOrder(GX_TEVSTAGE1, GX_TEXCOORD1, GX_TEXMAP1, GX_COLOR_NULL);
				GXSetTevColorOp(GX_TEVSTAGE1, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
				GXSetTevAlphaOp(GX_TEVSTAGE1, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
				GXSetTevColorIn(GX_TEVSTAGE1, GX_CC_CPREV, GX_CC_TEXC, GX_CC_TEXA, GX_CC_ZERO);
				GXSetTevAlphaIn(GX_TEVSTAGE1, GX_CA_APREV, GX_CA_ZERO, GX_CA_TEXA, GX_CA_TEXA);
				GXSetTevSwapMode(GX_TEVSTAGE1, GX_TEV_SWAP0, GX_TEV_SWAP0);
				GXSetTevOrder(GX_TEVSTAGE2, GX_TEXCOORD_NULL, GX_TEXMAP_NULL, GX_COLOR0A0);
				GXSetTevColorOp(GX_TEVSTAGE2, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
				GXSetTevAlphaOp(GX_TEVSTAGE2, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
				GXSetTevColorIn(GX_TEVSTAGE2, GX_CC_ZERO, GX_CC_CPREV, GX_CC_RASC, GX_CC_ZERO);
				GXSetTevAlphaIn(GX_TEVSTAGE2, GX_CA_ZERO, GX_CA_APREV, GX_CA_RASA, GX_CA_ZERO);
				GXSetTevSwapMode(GX_TEVSTAGE2, GX_TEV_SWAP0, GX_TEV_SWAP0);
				tevStage = GX_TEVSTAGE3;
				break;

			case 8:
				GXSetTevColor(GX_TEVREG0, (GXColor) {
					0, (u8)((127.0f * sinf((TWO_PI * (f32)(2 * gp->retraceCount)) / 360.0f)) + 127.0f),
					0, 0}); //TODO: check
				GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR0A0);
				GXSetTevColorOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
				GXSetTevAlphaOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
				GXSetTevColorIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_RASC, GX_CC_TEXC, GX_CC_C0);
				GXSetTevAlphaIn(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_RASA, GX_CA_TEXA, GX_CA_A0);
				GXSetTevSwapMode(GX_TEVSTAGE0, GX_TEV_SWAP0, GX_TEV_SWAP0);
				tevStage = GX_TEVSTAGE1;
				break;

			case 9:
				scrl_uv[0] += queen_uv[0];
				if (scrl_uv[0] > 100.0f) {
					scrl_uv[0] -= 100.0f;
				}
				if (scrl_uv[0] < -100.0f) {
					scrl_uv[0] += 100.0f;
				}

				scrl_uv[1] += queen_uv[1];
				if (scrl_uv[1] > 100.0f) {
					scrl_uv[1] -= 100.0f;
				}
				if (scrl_uv[1] < -100.0f) {
					scrl_uv[1] += 100.0f;
				}

				scrl_uv[2] += queen_uv[2];
				if (scrl_uv[2] > 100.0f) {
					scrl_uv[2] -= 100.0f;
				}
				if (scrl_uv[2] < -100.0f) {
					scrl_uv[2] += 100.0f;
				}

				scrl_uv[3] += queen_uv[3];
				if (scrl_uv[3] > 100.0f) {
					scrl_uv[3] -= 100.0f;
				}
				if (scrl_uv[3] < -100.0f) {
					scrl_uv[3] += 100.0f;
				}

				MTXTrans(queen_mtx, scrl_uv[0], scrl_uv[1], 0.0f);
				GXLoadTexMtxImm(queen_mtx, 0x1E, GX_MTX2x4);
				GXSetTexCoordGen2(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, GX_TEXMTX0, GX_FALSE, GX_PTIDENTITY);
				MTXTrans(queen_mtx, scrl_uv[2], scrl_uv[3], 0.0f);
				GXLoadTexMtxImm(queen_mtx, 0x21, GX_MTX2x4);
				GXSetTexCoordGen2(GX_TEXCOORD1, GX_TG_MTX2x4, GX_TG_TEX1, GX_TEXMTX1, GX_FALSE, GX_PTIDENTITY);
				GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR_NULL);
				GXSetTevColorOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
				GXSetTevAlphaOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
				GXSetTevColorIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_ZERO, GX_CC_ZERO, GX_CC_TEXC);
				GXSetTevAlphaIn(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_TEXA);
				GXSetTevSwapMode(GX_TEVSTAGE0, GX_TEV_SWAP0, GX_TEV_SWAP0);
				GXSetTevOrder(GX_TEVSTAGE1, GX_TEXCOORD1, GX_TEXMAP1, GX_COLOR_NULL);
				GXSetTevColorOp(GX_TEVSTAGE1, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
				GXSetTevAlphaOp(GX_TEVSTAGE1, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
				GXSetTevColorIn(GX_TEVSTAGE1, GX_CC_CPREV, GX_CC_TEXC, GX_CC_TEXA, GX_CC_ZERO);
				GXSetTevAlphaIn(GX_TEVSTAGE1, GX_CA_APREV, GX_CA_ZERO, GX_CA_TEXA, GX_CA_TEXA);
				GXSetTevSwapMode(GX_TEVSTAGE1, GX_TEV_SWAP0, GX_TEV_SWAP0);
				GXSetTevOrder(GX_TEVSTAGE2, GX_TEXCOORD_NULL, GX_TEXMAP_NULL, GX_COLOR0A0);
				GXSetTevColorOp(GX_TEVSTAGE2, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
				GXSetTevAlphaOp(GX_TEVSTAGE2, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
				GXSetTevColorIn(GX_TEVSTAGE2, GX_CC_ZERO, GX_CC_CPREV, GX_CC_RASC, GX_CC_ZERO);
				GXSetTevAlphaIn(GX_TEVSTAGE2, GX_CA_ZERO, GX_CA_APREV, GX_CA_RASA, GX_CA_ZERO);
				GXSetTevSwapMode(GX_TEVSTAGE2, GX_TEV_SWAP0, GX_TEV_SWAP0);
				tevStage = GX_TEVSTAGE3;
				break;

			default:
				GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR0A0);
				GXSetTevColorOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
				GXSetTevAlphaOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
				GXSetTevColorIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_RASC, GX_CC_TEXC, GX_CC_ZERO);
				GXSetTevAlphaIn(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_RASA, GX_CA_TEXA, GX_CA_ZERO);
				GXSetTevSwapMode(GX_TEVSTAGE0, GX_TEV_SWAP0, GX_TEV_SWAP0);
				tevStage = GX_TEVSTAGE1;
				break;
		}
	}
	else {
		GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD_NULL, GX_TEXMAP_NULL, GX_COLOR0A0);
		GXSetTevColorOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
		GXSetTevAlphaOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
		GXSetTevColorIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_ZERO, GX_CC_ZERO, GX_CC_RASC);
		GXSetTevAlphaIn(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_RASA);
		GXSetTevSwapMode(GX_TEVSTAGE0, GX_TEV_SWAP0, GX_TEV_SWAP0);
		tevStage = GX_TEVSTAGE1;
	}

	if (!(flags & 0x10000000)) {
		GXSetTevKColorSel(tevStage, GX_TEV_KCSEL_K0);
		GXSetTevKAlphaSel(tevStage, GX_TEV_KASEL_K0_A);
		GXSetTevOrder(tevStage, GX_TEXCOORD_NULL, GX_TEXMAP_NULL, GX_COLOR_NULL);
		GXSetTevColorOp(tevStage, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
		GXSetTevAlphaOp(tevStage, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
		GXSetTevColorIn(tevStage, GX_CC_ZERO, GX_CC_CPREV, GX_CC_KONST, GX_CC_ZERO);
		GXSetTevAlphaIn(tevStage, GX_CA_ZERO, GX_CA_APREV, GX_CA_KONST, GX_CA_ZERO);
		GXSetTevSwapMode(tevStage++, GX_TEV_SWAP0, GX_TEV_SWAP0);
	}

}






void mapMain(void) {

}

void mapSetPolygonVtxDesc(MapObject* obj, MapFileMesh* mesh) {
	MapWork* wp;
	MapEntry* entry;
	u8* dataBase;
	u8* vcdBase;
	u32 posOff, nrmOff, posFrac, posCount, nrmCount;
	int c, t;
	u32 clrOff, clrCount, texFrac, texOff, texCount;

	wp = mapGetWork();
	entry = &wp->entries[0];
	dataBase = (u8*)entry->header.data;
	if (!dataBase || !mesh || mesh->vcdTableOffset == 0) return;

	vcdBase = dataBase + mesh->vcdTableOffset;
	posOff = swap32_m(*(u32*)(vcdBase + 0x00));
	nrmOff = swap32_m(*(u32*)(vcdBase + 0x04));
	posFrac = swap32_m(*(u32*)(vcdBase + 0x44)) & 0xFF;

	GXClearVtxDesc();

	// Position: Always present in map meshes
	if (posOff != 0) {
		posCount = swap32_m(*(u32*)(dataBase + posOff));
		GXSetVtxDesc(GX_VA_POS, GX_INDEX16);
		GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_POS, GX_POS_XYZ, GX_S16, (u8)posFrac);
		GXSetArray5(GX_VA_POS, dataBase + posOff + 4, posCount * 6, 6, FALSE);
	}

	// Normal: Always present in map meshes
	if (nrmOff != 0) {
		nrmCount = swap32_m(*(u32*)(dataBase + nrmOff));
		GXSetVtxDesc(GX_VA_NRM, GX_INDEX16);
		GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_NRM, GX_NRM_XYZ, GX_S8, 6);
		GXSetArray5(GX_VA_NRM, dataBase + nrmOff + 4, nrmCount * 3, 3, FALSE);
	}

	// Colors (0 and 1): Checked via elementMask bits 2 and 3
	for (c = 0; c < 2; c++) {
		if (mesh->elementMask & (1 << (c + 2))) {
			clrOff = swap32_m(*(u32*)(vcdBase + 0x0C + c * 4));
			if (clrOff != 0) {
				clrCount = swap32_m(*(u32*)(dataBase + clrOff));
				GXSetVtxDesc((GXAttr)(GX_VA_CLR0 + c), GX_INDEX16);
				GXSetVtxAttrFmt(GX_VTXFMT0, (GXAttr)(GX_VA_CLR0 + c), GX_CLR_RGBA, GX_RGBA8, 0);
				GXSetArray5((GXAttr)(GX_VA_CLR0 + c), dataBase + clrOff + 4, clrCount * 4, 4, FALSE);
			}
		}
	}

	// Texcoords (0 through 7): Checked via elementMask bits 4 through 11
	for (t = 0; t < 8; t++) {
		if (mesh->elementMask & (1 << (t + 4))) {
			texFrac = swap32_m(*(u32*)(vcdBase + 0x48 + t * 4)) & 0xFF;
			texOff = swap32_m(*(u32*)(vcdBase + 0x18 + t * 4));
			if (texOff != 0) {
				texCount = swap32_m(*(u32*)(dataBase + texOff));
				GXSetVtxDesc((GXAttr)(GX_VA_TEX0 + t), GX_INDEX16);
				GXSetVtxAttrFmt(GX_VTXFMT0, (GXAttr)(GX_VA_TEX0 + t), GX_TEX_ST, GX_S16, (u8)texFrac);
				GXSetArray5((GXAttr)(GX_VA_TEX0 + t), dataBase + texOff + 4, texCount * 4, 4, FALSE);
			}
		}
	}
}

void mapDispMapObj(CameraId cameraId, void* param) {
	MapObject* obj;
	MapFileJoint* joint;
	CameraEntry* cam;
	Mtx mvMtx, invMtx;
	GXColor white;
	int p, d;

	obj = (MapObject*)param;
	if (!obj || !obj->joints) return;

	joint = obj->joints;
	if (joint->partCount <= 0) return;

	cam = camGetPtr(cameraId);
	if (!cam) return;

	PSMTXConcat(cam->view, obj->modelWorldMtx, mvMtx);
	GXLoadPosMtxImm(mvMtx, GX_PNMTX0);

	PSMTXInvXpose(mvMtx, invMtx);
	GXLoadNrmMtxImm(invMtx, GX_PNMTX0);
	GXSetCurrentMtx(GX_PNMTX0);

	if (joint->drawMode) {
		GXSetCullMode((GXCullMode)joint->drawMode->cullMode);
	} else {
		GXSetCullMode(GX_CULL_NONE);
	}

	GXSetZMode(GX_TRUE, GX_LEQUAL, GX_TRUE);
	GXSetZCompLoc(GX_TRUE);
	GXSetAlphaCompare(GX_GREATER, 0, GX_AOP_AND, GX_ALWAYS, 0);
	GXSetBlendMode(GX_BM_BLEND, GX_BL_SRCALPHA, GX_BL_INVSRCALPHA, GX_LO_CLEAR);

	white.r = 255;
	white.g = 255;
	white.b = 255;
	white.a = 255;
	GXSetChanAmbColor(GX_COLOR0A0, white);
	GXSetChanMatColor(GX_COLOR0A0, white);

	for (p = 0; p < joint->partCount; p++) {
		MapFileJointPart* part = &joint->parts[p];
		MapFileMesh* mesh = part->mesh;
		if (!mesh) continue;

		mapSetPolygonVtxDesc(obj, mesh);

		if (mesh->elementMask & 4) {
			GXSetChanCtrl(GX_COLOR0A0, GX_FALSE, GX_SRC_REG, GX_SRC_VTX, 0, GX_DF_NONE, GX_AF_NONE);
		} else {
			GXSetChanCtrl(GX_COLOR0A0, GX_FALSE, GX_SRC_REG, GX_SRC_REG, 0, GX_DF_NONE, GX_AF_NONE);
		}

		if (part->hasTex) {
			GXLoadTexObj(&part->texObj, GX_TEXMAP0);
			GXSetNumTexGens(1);
			GXSetTexCoordGen2(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, GX_IDENTITY, GX_FALSE, GX_PTIDENTITY);
			GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR0A0);
			GXSetTevOp(GX_TEVSTAGE0, GX_MODULATE);
			GXSetNumTevStages(1);
		} else {
			GXSetNumTexGens(0);
			GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD_NULL, GX_TEXMAP_NULL, GX_COLOR0A0);
			GXSetTevOp(GX_TEVSTAGE0, GX_PASSCLR);
			GXSetNumTevStages(1);
		}

		for (d = 0; d < mesh->displayListCount; d++) {
			PackedDisplayList* dl = &mesh->displayLists.packedLists[d];
			if (dl->offset && dl->size > 0) {
				GXCallDisplayList((void*)(uintptr_t)dl->offset, dl->size);
			}
		}
	}
	GXClearVtxDesc();
}

void mapDisp(void) {
	MapWork* wp;
	MapEntry* entry;
	int i;

	if (disableMapDraw) return;
	wp = &mapWork[activeGroup];
	if (!wp) return;
	entry = &wp->entries[0];
	if (!entry || !entry->objects || entry->numJoints <= 0) return;

	for (i = 0; i < entry->numJoints; i++) {
		MapObject* obj = &entry->objects[i];
		if (obj->joints && obj->joints->partCount > 0) {
			dispEntry(obj->cameraId ? obj->cameraId : CAMERA_3D, 0.0f, 0, mapDispMapObj, obj);
		}
	}
}

MapObject* mapGetMapObj(const char* name) {
    MapWork* wp;
    MapEntry* entry;
    int j;
    int i;
    MapObject* obj;

    wp = &mapWork[activeGroup];
    if (!name) {
        obj = NULL;
        goto done;
    }
    for (i = 0, entry = wp->entries; i < wp->entries[0].count; entry++, i++) {
        for (j = 0, obj = entry->objects; j < entry->numJoints; j++, obj++) {
            if (strcmp(obj->joints->name, name) == 0) {
                goto done;
            }
        }
    }
    obj = NULL;
done:
    return obj;
}

void mapFlgOn(MapObject* obj, u32 flags, BOOL bChild) {
    BOOL check_field = (flags & 0x100) != 0;
    if (!check_field || obj->field_0x114) {
        obj->flags |= flags;
    }
    if (obj->child) {
        if (!check_field || obj->child->field_0x114) {
            obj->child->flags |= flags;
        }
        if (obj->child->child) {
            mapFlgOn(obj->child->child, flags, 1);
        }
        if (obj->child->next) {
            mapFlgOn(obj->child->next, flags, 1);
        }
    }
    if (bChild && obj->next) {
        if (!check_field || obj->next->field_0x114) {
            obj->next->flags |= flags;
        }
        if (obj->next->child) {
            mapFlgOn(obj->next->child, flags, 1);
        }
        if (obj->next->next) {
            mapFlgOn(obj->next->next, flags, 1);
        }
    }
}

void mapFlgOff(MapObject* obj, u32 flags, BOOL bChild) {
    u32 mask = ~flags;
    obj->flags &= mask;
    if (obj->child) {
        obj->child->flags &= mask;
        if (obj->child->child) {
            mapFlgOff(obj->child->child, flags, 1);
        }
        if (obj->child->next) {
            mapFlgOff(obj->child->next, flags, 1);
        }
    }
    if (bChild && obj->next) {
        obj->next->flags &= mask;
        if (obj->next->child) {
            mapFlgOff(obj->next->child, flags, 1);
        }
        if (obj->next->next) {
            mapFlgOff(obj->next->next, flags, 1);
        }
    }
}

void mapObjFlagOn(const char* name, u32 flags) {
    MapObject* obj = mapGetMapObj(name);
    if (!obj) {
        mapErrorEntry(0, name);
    } else {
        if ((flags & 0x100) && !obj->field_0x114) {
            return;
        }
        obj->flags |= flags;
    }
}

void mapObjFlagOff(const char* name, u32 flags) {
    MapObject* obj = mapGetMapObj(name);
    if (!obj) {
        mapErrorEntry(0, name);
    } else {
        obj->flags &= ~flags;
    }
}

void mapGrpFlagOn(const char* name, u32 flags) {
    MapObject* obj = mapGetMapObj(name);
    if (!obj) {
        mapErrorEntry(0, name);
    } else {
        mapFlgOn(obj, flags, 0);
    }
}

void mapGrpFlagOff(const char* name, u32 flags) {
    MapObject* obj = mapGetMapObj(name);
    if (!obj) {
        mapErrorEntry(0, name);
    } else {
        mapFlgOff(obj, flags, 0);
    }
}

void _setOffScrnId(MapObject* obj, s32 id, BOOL bChild) {
    obj->offscreenId = (s16)id;
    if (id != -1) {
        obj->flags |= 0x80;
    } else {
        obj->flags &= ~0x80;
    }
    if (obj->child) {
        obj->child->offscreenId = (s16)id;
        if (id != -1) {
            obj->child->flags |= 0x80;
        } else {
            obj->child->flags &= ~0x80;
        }
        if (obj->child->child) {
            _setOffScrnId(obj->child->child, id, 1);
        }
        if (obj->child->next) {
            _setOffScrnId(obj->child->next, id, 1);
        }
    }
    if (bChild && obj->next) {
        obj->next->offscreenId = (s16)id;
        if (id != -1) {
            obj->next->flags |= 0x80;
        } else {
            obj->next->flags &= ~0x80;
        }
        if (obj->next->child) {
            _setOffScrnId(obj->next->child, id, 1);
        }
        if (obj->next->next) {
            _setOffScrnId(obj->next->next, id, 1);
        }
    }
}

void mapObjSetOffScreen(const char* name, const char* offscreenName) {
    MapObject* obj;

    obj = mapGetMapObj(name);
    if (obj) {
        obj->offscreenId = offscreenNameToId(offscreenName);
        obj->flags |= 0x80;
    }
}

void mapGrpSetOffScreen(const char* name, const char* offscreenName) {
    MapObject* obj;
    s32 id;

    obj = mapGetMapObj(name);
    if (obj) {
        id = offscreenNameToId(offscreenName);
        _setOffScrnId(obj, id, 0);
    }
}

void mapObjClearOffScreen(const char* name) {
    MapObject* obj = mapGetMapObj(name);
    if (obj) {
        obj->offscreenId = -1;
        obj->flags &= ~0x80;
    }
}

void mapGrpClearOffScreen(const char* name) {
    MapObject* obj = mapGetMapObj(name);
    if (obj) {
        _setOffScrnId(obj, -1, 0);
    }
}

void mapFogOn(void) {
    mapWork[activeGroup].entries[0].flags |= 1;
}

void mapFogOff(void) {
    mapWork[activeGroup].entries[0].flags &= ~1;
}

void mapGetFog(s32* type, f32* start, f32* end, GXColor* color) {
    MapEntry* entry = &mapWork[activeGroup].entries[0];
    *type = entry->fogType;
    *start = entry->fogStart;
    *end = entry->fogEnd;
    *color = entry->fogColor;
}

void mapSetFog(s32 type, f32 start, f32 end, GXColor* color) {
    MapEntry* entry = &mapWork[activeGroup].entries[0];
    entry->fogType = type;
    entry->fogStart = start;
    entry->fogEnd = end;
    entry->fogColor = *color;
}

void mapBlendOff(void) {
    mapWork[activeGroup].entries[0].flags &= ~2;
}

void mapBlendOff2(void) {
    mapWork[activeGroup].entries[0].flags &= ~4;
}

#ifndef TARGET_PC
extern const GXColor lbl_8042C3A0;
extern const GXColor lbl_8042C3A4;
#define defaultBlendColor lbl_8042C3A0
#define defaultBlendColor2 lbl_8042C3A4
#else
static const GXColor defaultBlendColor = {0, 0, 0, 0};
#define defaultBlendColor2 defaultBlendColor
#endif

void mapGetBlend(GXColor* color) {
    MapEntry* entry = &mapWork[activeGroup].entries[0];
    if (entry->flags & 2) {
        *color = entry->blendColor;
    } else {
        *color = defaultBlendColor;
    }
}

void mapGetBlend2(GXColor* color) {
    MapEntry* entry = &mapWork[activeGroup].entries[0];
    if (entry->flags & 4) {
        *color = entry->blendColor2;
    } else {
        *color = defaultBlendColor2;
    }
}

void mapSetBlend(GXColor* color) {
    MapEntry* entry = &mapWork[activeGroup].entries[0];
    entry->blendColor = *color;
    entry->flags |= 2;
}

void mapSetBlend2(GXColor* color) {
    MapEntry* entry = &mapWork[activeGroup].entries[0];
    entry->blendColor2 = *color;
    entry->flags |= 4;
}

void mapSetMaterialLight(s32 flags, Vec position) {

}

void mapSetMaterialLastStageBlend(s32 flags, GXColor color1, GXColor color2) {

}

void mapSetMaterialFog(void) {

}

static void setColor(MapObject* obj, const GXColor* color, BOOL bChild) {
    if (!obj) return;
    obj->color = *color;
    obj->flags |= 0x40;
    if (obj->child) {
        obj->child->color = *color;
        obj->child->flags |= 0x40;
        if (obj->child->child) {
            setColor(obj->child->child, color, 1);
        }
        if (obj->child->next) {
            setColor(obj->child->next, color, 1);
        }
    }
    if (bChild && obj->next) {
        obj->next->color = *color;
        obj->next->flags |= 0x40;
        if (obj->next->child) {
            setColor(obj->next->child, color, 1);
        }
        if (obj->next->next) {
            setColor(obj->next->next, color, 1);
        }
    }
}

void mapObjSetColor(const char* name, GXColor color) {
    MapObject* obj = mapGetMapObj(name);
    if (obj) {
        obj->color = color;
        obj->flags |= 0x40;
    }
}

void mapGrpSetColor(const char* name, GXColor color) {
    MapObject* obj = mapGetMapObj(name);
    if (obj) {
        setColor(obj, &color, 0);
    }
}

static void mapObjGetPosSub(MapObject* obj, Vec* accumPos, s32* count, BOOL bChild) {
    Vec worldPos;
    if (!obj->child) {
        PSMTXMultVec(obj->modelWorldMtx, &obj->localCenter, &worldPos);
        PSVECAdd(accumPos, &worldPos, accumPos);
        (*count)++;
    } else {
        if (obj->child->child) {
            mapObjGetPosSub(obj->child->child, accumPos, count, 1);
        } else {
            PSMTXMultVec(obj->child->modelWorldMtx, &obj->child->localCenter, &worldPos);
            PSVECAdd(accumPos, &worldPos, accumPos);
            (*count)++;
        }
        if (obj->child->next) {
            mapObjGetPosSub(obj->child->next, accumPos, count, 1);
        }
    }
    if (bChild && obj->next) {
        if (obj->next->child) {
            mapObjGetPosSub(obj->next->child, accumPos, count, 1);
        } else {
            PSMTXMultVec(obj->next->modelWorldMtx, &obj->next->localCenter, &worldPos);
            PSVECAdd(accumPos, &worldPos, accumPos);
            (*count)++;
        }
        if (obj->next->next) {
            mapObjGetPosSub(obj->next->next, accumPos, count, 1);
        }
    }
}

void mapObjGetPos(const char* name, Vec* pos) {
    MapObject* obj;
    Vec accumPos;
    s32 count;

    accumPos.x = 0.0f;
    accumPos.y = 0.0f;
    accumPos.z = 0.0f;
    obj = mapGetMapObj(name);
    if (!obj) {
        pos->x = 0.0f;
        pos->y = 0.0f;
        pos->z = 0.0f;
    } else {
        count = 0;
        mapObjGetPosSub(obj, &accumPos, &count, 0);
        PSVECScale(&accumPos, pos, 10.0f / (f32)count);
    }
}

void mapObjScale(const char* name, f32 x, f32 y, f32 z) {
    MapObject* obj;
    Mtx m;
    obj = mapGetMapObj(name);
    if (obj) {
        if (!(obj->flags & 0x200)) {
            MTXIdentity(obj->unkAC);
        }
        MTXScale(m, x, y, z);
        MTXConcat(obj->unkAC, m, obj->unkAC);
        obj->flags |= 0x200200;
    }
}

void mapObjTranslate(const char* name, f32 x, f32 y, f32 z) {
    MapObject* obj;
    Mtx m;
    obj = mapGetMapObj(name);
    if (obj) {
        if (!(obj->flags & 0x200)) {
            MTXIdentity(obj->unkAC);
        }
        MTXTrans(m, x / 10.0f, y / 10.0f, z / 10.0f);
        MTXConcat(obj->unkAC, m, obj->unkAC);
        obj->flags |= 0x200200;
    }
}

void mapObjRotate(const char* name, f32 x, f32 y, f32 z) {
    MapObject* obj;
    Mtx mX, mY, mZ;

    obj = mapGetMapObj(name);
    if (obj) {
        if (!(obj->flags & 0x200)) {
            MTXIdentity(obj->unkAC);
        }
        MTXRotRad(mX, 'x', MTXDegToRad(x));
        MTXRotRad(mY, 'y', MTXDegToRad(y));
        MTXRotRad(mZ, 'z', MTXDegToRad(z));
        MTXConcat(obj->unkAC, mZ, obj->unkAC);
        MTXConcat(obj->unkAC, mY, obj->unkAC);
        MTXConcat(obj->unkAC, mX, obj->unkAC);
        obj->flags = obj->flags | 0x200200;
    }
}

void mapObjGetFlushColor(const char* name, u8* r, u8* g, u8* b, u8* a) {
    MapObject* obj;

    obj = _mapGetMapObj(name);
    if (!obj) {
        _mapErrorEntry(0, name);
    } else {
        *r = obj->color.r;
        *g = obj->color.g;
        *b = obj->color.b;
        *a = obj->color.a;
    }
}

void mapGrpSetFlushColor(const char* name, u8 r, u8 g, u8 b, u8 a) {
    GXColor color;
    GXColor temp;

    temp = (GXColor){r, g, b, a};
    color = temp;
    mapGrpSetColor(name, color);
    mapGrpFlagOff(name, 0x40);
}

void mapObjSetFlushColor(const char* name, u8 r, u8 g, u8 b, u8 a) {
    GXColor color = {0};
    MapObject* obj;

    color.r = r;
    color.g = g;
    color.b = b;
    color.a = a;

    obj = _mapGetMapObj(name);
    if (obj) {
        obj->color = color;
        obj->flags |= 0x40;
    }

    obj = _mapGetMapObj(name);
    if (!obj) {
        _mapErrorEntry(0, name);
    } else {
        obj->flags &= ~0x40;
    }
}

void mapGrpFlushOff(const char* name) {
    mapGrpFlagOff(name, 0x100000);
}

void mapGrpFlushOn(const char* name) {
    mapGrpFlagOn(name, 0x100000);
}

void mapObjFlushOff(const char* name) {
    register MapObject* obj;

    obj = _mapGetMapObj(name);
    if (!obj) {
        _mapErrorEntry(0, name);
    } else {
        obj->flags &= ~0x100000;
    }
}

void mapObjFlushOn(const char* name) {
    register MapObject* obj;

    obj = _mapGetMapObj(name);
    if (!obj) {
        _mapErrorEntry(0, name);
    } else {
        obj->flags |= 0x100000;
    }
}

BOOL mapTestXLU(u32 flags, GXColor* color1, GXColor* color2) {
    MapWork* wp;

    if ((flags & 0x40) && color1->a != 0xFF) {
        return TRUE;
    }
    if (color2->a != 0xFF) {
        return TRUE;
    }
    if (!(flags & 0x800)) {
        wp = &mapWork[activeGroup];
        if ((wp->entries[0].flags & 2) && wp->entries[0].blendColor.a != 0xFF) {
            return TRUE;
        }
    }
    if (!(flags & 0x1000)) {
        wp = &mapWork[activeGroup];
        if ((wp->entries[0].flags & 4) && wp->entries[0].blendColor2.a != 0xFF) {
            return TRUE;
        }
    }
    return FALSE;
}

void mapSetProjMtx(Mtx mtx) {
    MTXCopy(mtx, projMtx);
}

void mapSetProjTexObj(GXTexObj* texObj, void* imgRef) {
    paperCraft_ambient = *texObj;
    projTexImgRef = imgRef;
}
