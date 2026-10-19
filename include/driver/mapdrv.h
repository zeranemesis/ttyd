#pragma once

#include <dolphin/gx.h>
#include <dolphin/mtx.h>
#include "driver/camdrv.h"

typedef struct DisplayList {
    uintptr_t offset; // 0x00 - Pointer to vertex data
    u32 vertexCount;  // 0x08 - Number of vertices
} DisplayList;

typedef struct PackedDisplayList {
    uintptr_t offset; // 0x00 - Pointer to packed data
    u32 size;         // 0x08 - Byte size of display list
    u8 padding[4];    // 0x0C
} PackedDisplayList;

typedef struct MapFileMesh {
    u8 unk0;              // 0x0
    u8 padding[2];        // 0x1
    u8 isPackedDisplay;   // 0x3, BOOL
    s32 displayListCount; // 0x4
    u32 elementMask;      // 0x8
    u32 vcdTableOffset;   // 0xC
  union {
    u32* normalLists[1];               // unpacked: stride 4, count = *normalLists[j]
    PackedDisplayList packedLists[1];  // packed:   stride 8, count = *(u16*)((u8*)packedLists[j].offset + 1)
} displayLists;  // at 0x10
} MapFileMesh;

typedef struct MapFileJointPart {
    void* material;    // 0x0
    MapFileMesh* mesh; // 0x4
    BOOL hasTex;
    s32 texIdx;
    GXTexObj texObj;
} MapFileJointPart;

typedef struct MapJointDrawMode {
    u8 unk0[0x1 - 0x0];  // 0x0
    u8 cullMode;         // 0x1
    u8 unk2[0x8 - 0x2];  // 0x2
    s32 unk8;            // 0x8
    u8 unkC[0x10 - 0xC]; // 0xC
} MapJointDrawMode;

typedef struct MapFileJoint {
    const char* name;            // 0x0
    const char* type;            // 0x4
    struct MapFileJoint* parent; // 0x8
    struct MapFileJoint* child;  // 0xC
    struct MapFileJoint* next;   // 0x10
    struct MapFileJoint* prev;   // 0x14
    Vec scale;                   // 0x18
    Vec rotation;                // 0x24
    Vec translation;             // 0x30
    Vec bboxMin;                 // 0x3C
    Vec bboxMax;                 // 0x48
    u8 field_0x54[0x58 - 0x54];  // 0x54
    MapJointDrawMode* drawMode;  // 0x58
    s32 partCount;               // 0x5C
    MapFileJointPart parts[];    // 0x60+
} MapFileJoint;

typedef struct MapFileHeader {
    s32 fileSize;     // 0x0
    s32 dataSize;     // 0x4
    s32 relCount;     // 0x8, TODO: rename?
    s32 chunkCount;   // 0xC, TODO: rename?
    s32 unused;       // 0x10
    u8 reserved[0xC]; // 0x14
} MapFileHeader;

// entry in the chunk table
typedef struct MapFileChunk {
    u32 offset; // 0x0, relative offset into data
    u32 string; // 0x4, relative offset into strings
} MapFileChunk;

typedef struct MapFileInfo {
    const char* version; // 0x0
    MapFileJoint* joint; // 0x4
    const char* mapName; // 0x8
    const char* hitName; // 0xC
} MapFileInfo;

typedef struct MapHeader {
    MapFileHeader file;         // 0x0
    void* data;                 // 0x20, TODO re-type?
    void* rel;                  // 0x24, TODO: re-type?
    MapFileChunk* chunks;       // 0x28
    void* unused;               // 0x2C
    const char* strings;        // 0x30
    u8 field_0x34[0x3C - 0x34]; // 0x34
    s32 field_0x3C;             // 0x3C, dispFlags something?
    u8 field_0x40[0x44 - 0x40]; // 0x40
} MapHeader;

typedef struct MapObject {
    u32 flags;                   // 0x0
    u8 unk4;                     // 0x4
    u8 cameraId;                 // 0x5
    u8 unk6[2];                  // 0x6
    MapFileJoint* joints;        // 0x8
    GXColor color;               // 0xC
    Vec localCenter;             // 0x10
    Mtx modelWorldMtx;           // 0x1C
    u8 field_0x4C[0xAC - 0x4C];  // 0x4C
    Mtx unkAC;                   // 0xAC
    s16 offscreenId;             // 0xDC
    u8 field_0xDE[2];            // 0xDE
    struct MapObject* parent;    // 0xE0
    struct MapObject* child;     // 0xE4
    struct MapObject* next;      // 0xE8
    u8 field_0xEC[0x114 - 0xEC]; // 0xEC
    u32 field_0x114;             // 0x114
    u8 field_0x118[0x134 - 0x118]; // 0x118
} MapObject;

typedef struct MapError {
    char field_0x0[32]; // 0x0
    s32 field_0x20;     // 0x20
} MapError;

// one animation binding of MapEntryAnimData, ties an animation track to a hit joint
typedef struct MapEntryAnimBind {
    const char* name; // 0x0, name of the hit joint this animation is bound to
    // TODO: rest unknown, used by mapCalcAnimMatrix (strided array)
} MapEntryAnimBind;

// list of animation bindings, pointed to by MapEntryAnimFile::bindList
typedef struct MapEntryAnimBindList {
    s32 count;                        // 0x0
    MapEntryAnimBind* entries[];      // 0x4, stride 4
} MapEntryAnimBindList;

// pointed to by MapEntryAnimData::file
typedef struct MapEntryAnimFile {
    u8 field_0x0[0xC - 0x0];          // 0x0
    MapEntryAnimBindList* bindList;   // 0xC
    // TODO: rest unknown
} MapEntryAnimFile;

typedef struct MapEntryAnimData {
    u16 flags;                        // 0x0
    u8 field_0x2[0x18 - 0x2];         // 0x2
    MapEntryAnimFile* file;           // 0x18
    u8 field_0x1C[0x20 - 0x1C];       // 0x1C
} MapEntryAnimData;

typedef struct MapEntry {
    s32 count;                     // 0x0
    u16 flags;                     // 0x4
    char name[16];                 // 0x6
    GXColor blendColor;            // 0x16
    GXColor blendColor2;           // 0x1A
    u8 field_0x1E[0x28 - 0x1E];    // 0x1E
    s32 fogType;                   // 0x28
    f32 fogStart;                  // 0x2C
    f32 fogEnd;                    // 0x30
    GXColor fogColor;              // 0x34
    MapHeader header;              // 0x38
    void* dat;                     // 0x7C, ./map/%s/map.dat
    u32 datSize;                   // 0x80
    void* tpl;                     // 0x84, ./map/%s/texture.tpl
    u32 tplSize;                   // 0x88
    u8 field_0x8C[0xA8 - 0x8C];    // 0x8C
    MapObject* rootMapObj;         // 0xA8
    struct HitEntry* rootHitObj;   // 0xAC
    Vec bbox[8];                   // 0xB0
    u8 field_110[0x150 - 0x110];   // 0x110
    s32 numJoints;                 // 0x150, TODO: rename? see: hitNumJoints
    MapObject* objects;            // 0x154, TODO: rename? see: hitNumJoints
    s32 hitNumJoints;              // 0x158
    struct HitEntry* hitEntries;   // 0x15C
    s32 animCount;                 // 0x160, number of MapEntryAnimData entries
    MapEntryAnimData* animData;    // 0x164
    u8 field_0x168[0x178 - 0x168]; // 0x168
} MapEntry;

typedef struct MapWork {
    MapEntry entries[2]; // 0x0
    s32 field_2F0;       // 0x2F0
} MapWork;

void mapErrorEntry(s32 arg0, const char* arg1);
const char* getMapDataDvdRoot(void);
void mapDispOff(void);
void mapDispOn(void);
MapWork* mapGetWork(void);
s32 mapGetActiveGroup(void);
s32 mapGetJoints(MapFileJoint* joint);
void mapInit(void);
MapEntryAnimData* mapSearchAnmObj(const char* animName);
void mapCheckAnimation(const char* animName, BOOL* valid, f32* a3);
void mapCalcAnimMatrix(Mtx mtx, Mtx localMtx, MapEntryAnimData* anim, MapEntryAnimBind* bind);
// more animation functions?
void mapUnLoad(void);
BOOL mapPreLoad(void);
void mapLoad(const char* map);

#ifdef TARGET_PC
void mapMain(void);
#else
void fn_80026D7C(void);
#define mapMain fn_80026D7C
#endif
void mapDisp(void);

void mapSetMaterialLight(s32 flags, Vec position);
void mapSetMaterialLastStageBlend(s32 flags, GXColor color1, GXColor color2);
void mapSetMaterialFog(void);
void mapSetMaterialTev(s32 texCount, s32 drawMode, s32 flags, Mtx mtx);

MapObject* mapGetMapObj(const char* name);
void mapDispMapObj(CameraId cameraId, void* param);
void mapSetPolygonVtxDesc(MapObject* obj, MapFileMesh* mesh);

void mapFlgOn(MapObject* obj, u32 flags, BOOL bChild);
void mapFlgOff(MapObject* obj, u32 flags, BOOL bChild);
void mapObjFlagOn(const char* name, u32 flags);
void mapObjFlagOff(const char* name, u32 flags);
void mapGrpFlagOn(const char* name, u32 flags);
void mapGrpFlagOff(const char* name, u32 flags);
void _setOffScrnId(MapObject* obj, s32 id, BOOL bChild);
void mapObjSetOffScreen(const char* name, const char* offscreenName);
void mapGrpSetOffScreen(const char* name, const char* offscreenName);
void mapObjClearOffScreen(const char* name);
void mapGrpClearOffScreen(const char* name);
void mapFogOn(void);
void mapFogOff(void);
void mapGetFog(s32* type, f32* start, f32* end, GXColor* color);
void mapSetFog(s32 type, f32 start, f32 end, GXColor* color);
void mapBlendOff(void);
void mapBlendOff2(void);
void mapGetBlend(GXColor* color);
void mapGetBlend2(GXColor* color);
void mapSetBlend(GXColor* color);
void mapSetBlend2(GXColor* color);
void mapObjRotate(const char* name, f32 x, f32 y, f32 z);
void mapObjTranslate(const char* name, f32 x, f32 y, f32 z);
void mapObjScale(const char* name, f32 x, f32 y, f32 z);
void mapObjGetPos(const char* name, Vec* pos);
void mapObjSetColor(const char* name, GXColor color);
void mapGrpSetColor(const char* name, GXColor color);

void mapObjGetFlushColor(const char* name, u8* r, u8* g, u8* b, u8* a);
void mapGrpSetFlushColor(const char* name, u8 r, u8 g, u8 b, u8 a);
void mapObjSetFlushColor(const char* name, u8 r, u8 g, u8 b, u8 a);
void mapGrpFlushOff(const char* name);
void mapGrpFlushOn(const char* name);
void mapObjFlushOff(const char* name);
void mapObjFlushOn(const char* name);
BOOL mapTestXLU(u32 flags, GXColor* color1, GXColor* color2);
void mapSetProjMtx(Mtx mtx);
void mapSetProjTexObj(GXTexObj* texObj, void* imgRef);
