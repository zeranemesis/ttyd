/*
 * Warning, inlining hell
 */
#include "driver/animdrv.h"
#include "driver/dispdrv.h"
#include "driver/mapdrv.h"
#include "mario/mariost.h"
#include "memory.h"
#include "system.h"
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define TESTHEAP_SIZE (u32)(1.5 * 1024 * 1024)
#define PI 3.14159265358979323846264338327950288419716939937510f

#ifdef TARGET_PC
extern GlobalWork* gp;
#else
extern GlobalWork* wp_80422810;
#define gp wp_80422810
#endif
extern int sprintf(char* str, const char* format, ...);
extern double tan(double arg);
#ifndef TARGET_PC
extern const f32 lbl_8042C5A8;
extern const f32 lbl_8042C5E4;
extern const double lbl_802CCD20;
#endif

//.data
u8 _vivihimoData[0xC00]; // TODO: embed the actual data? gets overwritten by a/vivian.bin in animInit()

//.bss
static AnimationWork work;
f32 tanfTbl[0xB2];

//.sdata
s32 vivihimoTailGroupNo[4] = { 0, 2, -1, -1 };
void* vivihimoData[4] = {
    NULL,
    &_vivihimoData[0],
    &_vivihimoData[0x400],
    &_vivihimoData[0x800],
};
static AnimationWork* wp = &work;
s32 _dummy = -1;
static s32 tbl[4] = { 2, 1, 3, 0 };

//.sbss
u32 anim_ext_tex_size;
Mtx mtx; // TODO: make sure it's put in .sbss
s32 g_modeling_mtx_lv;
Mtx* g_modeling_mtx;

// local declarations
void animPoseRefresh(void);
void animPaperPoseDisp(CameraId camId, void* param);
void animPoseDrawMtx(s32 poseId, Mtx matrix, s32 xluMode, f32 rotY, f32 scale);
void animPaperPoseDispSub(s32 unused, AnimationPose* pose);

AnimationWork* animGetPtr(void) {
    return wp;
}

OSTime animTimeGetTime(BOOL globalTime) {
    if (globalTime == FALSE) {
        return OSTicksToMilliseconds(gp->renderFieldTime);
    }
    return OSTicksToMilliseconds(gp->renderTime);
}

void initTestHeap(void) {
    if (!wp->testHeap) { // allocate 1.5MiB test heap
        wp->testHeap = __memAlloc(HEAP_DEFAULT, TESTHEAP_SIZE);
    }
    wp->testAlloc = (uintptr_t)wp->testHeap;
}

// heavily inlined
void* testAlloc(u32 size) {
    void* alloc = (void*)wp->testAlloc;

    if (size & 31) {
        size += 32 - (size & 31);
    }
    if (size >= TESTHEAP_SIZE) {
        return __memAlloc(HEAP_DEFAULT, size);
    }
    wp->testAlloc += size;
    if (wp->testAlloc >= (uintptr_t)wp->testHeap + TESTHEAP_SIZE) { // need to allocate more
        animPoseRefresh();
        alloc = (void*)wp->testAlloc;
        wp->testAlloc += size;
    }
    return alloc;
}

void animInit(void) {
    int i;
    FileEntry* file;

#ifdef TARGET_PC
    printf("[animInit] Starting animInit...\n"); fflush(stdout);
#endif
    wp->totalFiles = 64; // TODO: #define these?
    wp->totalTextures = 64;
    wp->totalPoses = 256;
    wp->mFloatScratchRP = 1024;
    wp->mFloatScratchWP = 0;
    wp->files = __memAlloc(HEAP_DEFAULT, sizeof(AnimationPoseFile) * wp->totalFiles);
    wp->textures = __memAlloc(HEAP_DEFAULT, sizeof(AnimationTextureFile) * wp->totalTextures);
    wp->poses = __memAlloc(HEAP_DEFAULT, sizeof(AnimationPose) * wp->totalPoses);
    wp->mbUseFloatScratch = 0;
    wp->mpFloatScratch = __memAlloc(HEAP_DEFAULT, sizeof(Vec) * wp->mFloatScratchRP);

    wp->mpTexObjs[0] = NULL;
    wp->mpTexObjs[1] = NULL;
    wp->mpCurTexObj = NULL;

    wp->mpTexDatas[0] = NULL;
    wp->mpTexDatas[1] = NULL;
    wp->mpCurTexData = NULL;

    wp->paperMtxPtr[0] = NULL;
    wp->paperMtxPtr[1] = NULL;
    wp->paperMtxPtr[2] = NULL;

    wp->field_0xD8 = 1;
    wp->field_0xDC = 1;

    wp->testHeap = NULL;
    wp->inBattle = FALSE;
    initTestHeap(); // inlined
    printf("[animInit] Test heap initialized.\n"); fflush(stdout);

    for (i = 0; i < 0xB2; i++) {
        tanfTbl[i] = (f32)tan(MTXDegToRad(i - 0x59));
    }
    printf("[animInit] Loading a/vivian.bin...\n"); fflush(stdout);
    file = fileAllocf(0, "a/vivian.bin");
    printf("[animInit] a/vivian.bin result=%p\n", file); fflush(stdout);
    if (file) {
        printf("[animInit] file->data=%p, *file->data=%p\n", file->data, file->data ? *file->data : NULL); fflush(stdout);
        if (file->data && *file->data) {
            memcpy_as4(vivihimoData[1], *file->data, sizeof(_vivihimoData));
            memcpy_as4(vivihimoTailGroupNo, (void*)((uintptr_t)*file->data + sizeof(_vivihimoData)), sizeof(vivihimoTailGroupNo));
        }
        fileFree(file);
    }
    fileSetCurrentArchiveType(0);
    printf("[animInit] Loading a/ag2tg.bin...\n"); fflush(stdout);
    wp->mp_ag2tg = fileAllocf(0, "a/ag2tg.bin");
    printf("[animInit] animInit completed.\n"); fflush(stdout);
}

void animMain(void) {
#ifdef TARGET_PC
    dispEntry(CAMERA_OFFSCREEN, 0.0f, 1, animPaperPoseDisp, NULL);
#else
    dispEntry(CAMERA_OFFSCREEN, lbl_8042C5A8, 1, animPaperPoseDisp, NULL);
#endif
}

inline AnimationPoseData* animPoseGetBaseData(AnimationPose* pose) {
    return *wp->files[pose->fileId].handle->data;
}

#define animPoseGetBaseData(pose) (*(AnimationPoseData**)(wp->files[(pose)->fileId].handle->data))

// heavily inlined, TODO helper function for that AnimPoseData* data, copy of animPoseGetAnimBaseDataPtr with AnimPose*
// arg
void animPose_AllocBuffer(AnimationPose* pose) {
    pose->mpBufferVtxPos = testAlloc(sizeof(Vec) * animPoseGetBaseData(pose)->mBufferPosNum);
    pose->mpVtxArrayPos = testAlloc(sizeof(Vec) * animPoseGetBaseData(pose)->mBufferPosNum);

    pose->mpBufferVtxNrm = testAlloc(sizeof(Vec) * animPoseGetBaseData(pose)->mBufferNrmNum);
    pose->mpVtxArrayNrm = testAlloc(sizeof(Vec) * animPoseGetBaseData(pose)->mBufferNrmNum);

    pose->mpBufferGroupVisibility = testAlloc(sizeof(u8) * animPoseGetBaseData(pose)->mBufferVisibilityNum);
    pose->mpGroupVisibility = pose->mpBufferGroupVisibility;

    pose->mpBufferNode = testAlloc(sizeof(AnimPoseNode*) * animPoseGetBaseData(pose)->mBufferNodeNum);
    pose->mpNodes = testAlloc(sizeof(AnimPoseNode*) * animPoseGetBaseData(pose)->mBufferNodeNum);

    pose->mpBufferTexAnimEntries = testAlloc(sizeof(AnimTexMtx) * animPoseGetBaseData(pose)->mBufferTexAnimNum);
    pose->mpTexAnimEntries = pose->mpBufferTexAnimEntries;

    pose->lastFrameZero = -1;
}

// heavily inlined
void animPoseRefresh(void) {
    int i;

    initTestHeap(); // inline
    for (i = 0; i < wp->totalPoses; i++) {
        AnimationPose* pose = &wp->poses[i]; // inline
        if (pose->flags) {
            // TODO: cleanup in rewrite?
            if ((!wp->inBattle || pose->type != 0) && (wp->inBattle || pose->type != 1)) {
                animPose_AllocBuffer(pose);
                pose->lastFrameZero = -1; // TODO: redundant?
            }
        }
    }
}

void animPoseBattleInit(void) {
    wp->inBattle = TRUE;
    animPoseRefresh();
}

static inline u32 anim_swap32(u32 v) {
#if defined(_MSC_VER)
    return _byteswap_ulong(v);
#else
    return __builtin_bswap32(v);
#endif
}

static inline u16 anim_swap16(u16 v) {
#if defined(_MSC_VER)
    return _byteswap_ushort(v);
#else
    return __builtin_bswap16(v);
#endif
}

#ifdef TARGET_PC
static AnimationPoseData* UnpackAnimationPoseData(void* rawData) {
    u8* raw;
    u32 rawBufferPosNum;
    AnimationPoseData* native;
    int j;
    s32 a;
    u32 animsOff;

    if (!rawData) return NULL;
    raw = (u8*)rawData;

    rawBufferPosNum = *(u32*)(raw + 0xF0);
    if (rawBufferPosNum <= 0x10000 && rawBufferPosNum > 0) {
        return (AnimationPoseData*)rawData;
    }

    native = (AnimationPoseData*)malloc(sizeof(AnimationPoseData));
    if (!native) return (AnimationPoseData*)rawData;
    memset(native, 0, sizeof(AnimationPoseData));

    native->mSize = anim_swap32(*(u32*)(raw + 0x0));
    memcpy(native->fileName, raw + 0x4, 64);
    memcpy(native->textureName, raw + 0x44, 64);
    memcpy(native->mBuildTime, raw + 0x84, 64);

    native->shapeCount = anim_swap32(*(u32*)(raw + 0xE8));
    native->mBufferPosNum = anim_swap32(*(u32*)(raw + 0xF0));
    native->mBufferPosIdxNum = anim_swap32(*(u32*)(raw + 0xF4));
    native->mBufferNrmNum = anim_swap32(*(u32*)(raw + 0xF8));
    native->mBufferNrmIdxNum = anim_swap32(*(u32*)(raw + 0xFC));
    native->mBufferTexAnimNum = anim_swap32(*(u32*)(raw + 0x12C));
    native->texDataCount = anim_swap32(*(u32*)(raw + 0x130));
    native->texBindCount = anim_swap32(*(u32*)(raw + 0x134));
    native->mBufferVisibilityNum = anim_swap32(*(u32*)(raw + 0x13C));
    native->mBufferNodeNum = anim_swap32(*(u32*)(raw + 0x140));
    native->groupCount = anim_swap32(*(u32*)(raw + 0x144));
    native->mAnimCount = anim_swap32(*(u32*)(raw + 0x148));

#define RESOLVE_PTR(type, off) ((off != 0) ? (type*)(raw + anim_swap32(off)) : NULL)

    native->shapes = RESOLVE_PTR(AnimPoseShape, *(u32*)(raw + 0x14C));
    native->drawCalls = RESOLVE_PTR(AnimDrawCallEntry, *(u32*)(raw + 0x150));
    native->bufferVtxPos = RESOLVE_PTR(Vec, *(u32*)(raw + 0x154));
    native->posIndex = RESOLVE_PTR(u16, *(u32*)(raw + 0x158));
    native->nrmVtxBuffer = RESOLVE_PTR(Vec, *(u32*)(raw + 0x15C));
    native->nrmIndex = RESOLVE_PTR(u16, *(u32*)(raw + 0x160));
    native->clr0VtxBuffer = RESOLVE_PTR(GXColor, *(u32*)(raw + 0x164));
    native->clr0Index = RESOLVE_PTR(u16, *(u32*)(raw + 0x168));

    for (j = 0; j < 8; j++) {
        native->texIndex[j] = RESOLVE_PTR(u16, *(u32*)(raw + 0x16C + j * 4));
    }

    native->texVtxBuffer = RESOLVE_PTR(f32, *(u32*)(raw + 0x18C));
    native->texData = RESOLVE_PTR(AnimPoseTexEntry, *(u32*)(raw + 0x194));
    native->texBinds = RESOLVE_PTR(AnimTexBind, *(u32*)(raw + 0x198));
    native->draws = RESOLVE_PTR(AnimPoseShapeDraw, *(u32*)(raw + 0x19C));
    native->groups = RESOLVE_PTR(AnimPoseGroup, *(u32*)(raw + 0x1A8));

    animsOff = anim_swap32(*(u32*)(raw + 0x1AC));
    if (animsOff != 0 && native->mAnimCount > 0 && native->mAnimCount < 2000) {
        native->mpAnims = (AnimTableEntry*)malloc(sizeof(AnimTableEntry) * native->mAnimCount);
        for (a = 0; a < native->mAnimCount; a++) {
            u8* rawEntry = raw + animsOff + a * 64;
            memcpy(native->mpAnims[a].name, rawEntry, 60);
            u32 dataOff = anim_swap32(*(u32*)(rawEntry + 60));
            native->mpAnims[a].data = (dataOff != 0) ? (AnimData*)(raw + dataOff) : NULL;
        }
    }
#undef RESOLVE_PTR

    return native;
}
#endif

s32 animPoseEntry(const char* animName, s32 type) {
    AnimationPose *pose, *temp;
    AnimationPoseFile *file, *tempfile;
    AnimationTextureFile *texture, *temptex;
    AnimationPoseData* data;
    s32 poseId, fileId, texId;
    s32 check, i;
    char textureName[256];

    switch (type) {
        case 0:
            fileSetCurrentArchiveType(1);
            break;
        case 1:
            fileSetCurrentArchiveType(2);
            break;
        case 2:
            fileSetCurrentArchiveType(0);
            break;
    }

    for (i = 0, poseId = 0; i < wp->totalPoses; i++, poseId++) {
        temp = &wp->poses[i];
        if (!temp->flags) {
            pose = temp;
            goto label_1;
        }
    }
    pose = NULL;
    poseId = -1;
label_1:
    pose->flags |= 1;
    pose->typeFlags = 0;
    pose->references = 0;

    check = wp->totalFiles;
    for (i = 0; i < check; i++) {
        file = &wp->files[i];
        if (file->active && file->handle && file->handle->data) {
            data = *file->handle->data;
            if (data && !strcmp(data->fileName, animName)) {
                file->references++;
                fileId = i;
                goto label_5;
            }
        }
    }

    for (i = 0, fileId = 0; i < wp->totalFiles; i++, fileId++) {
        tempfile = &wp->files[i];
        if (!tempfile->active) {
            file = tempfile;
            goto label_2;
        }
    }
    file = NULL;
    fileId = -1;
label_2:
    file->active = TRUE;
    file->references++;
    printf("[animPoseEntry] Loading '%s' via fileAllocf...\n", animName); fflush(stdout);
    file->handle = fileAllocf(5, "%s/%s", "a", animName);
    printf("[animPoseEntry] file->handle=%p\n", file->handle); fflush(stdout);

    if (!file->handle) {
        fileId = -2;
        file->active = FALSE;
        file->references--;
    } else {
        data = (file->handle && file->handle->data) ? *file->handle->data : NULL;
#ifdef TARGET_PC
        printf("[animPoseEntry] data=%p\n", data); fflush(stdout);
#endif
        if (!data) {
            fileFree(file->handle);
            fileId = -2;
            file->active = FALSE;
            file->references--;
        } else {
#ifdef TARGET_PC
            data = UnpackAnimationPoseData(data);
            file->handle->field_0x4 = data;
            file->handle->data = &file->handle->field_0x4;
            printf("[animPoseEntry] Unpacked data=%p, mBufferPosNum=%u, mAnimCount=%u\n", data, data->mBufferPosNum, data->mAnimCount); fflush(stdout);
#endif
            sprintf(textureName, "%s/%s-", "a", data->textureName);
            check = wp->totalTextures;
            for (i = 0; i < check; i++) {
                texture = &wp->textures[i];
                if (texture->active) {
                    if (texture->handle) {
                        if (!strcmp(texture->handle->filename, textureName)) {
                            texture->references++;
                            goto label_4;
                        }
                    }
                }
            }

            for (i = 0, texId = 0; i < wp->totalTextures; i++, texId++) {
                temptex = &wp->textures[i];
                if (!temptex->active) {
                    texture = temptex;
                    goto label_3;
                }
            }
            texture = NULL;
            texId = -1;
        label_3:
            texture->active = TRUE;
            texture->references++;
            texture->handle = fileAlloc(textureName, 4);
        label_4:
            file->textureId = texId;
        }
    }
label_5:
    pose->fileId = fileId;
    if (pose->fileId == -2) {
        pose->flags = 0;
        return -2;
    }
    pose->animId = 0;
    pose->localTime = animTimeGetTime(type); // inline
    pose->lastFrameZero = -1;
    pose->lastFrameTime = 0.0f;
    pose->field_0x80 = 0;
    pose->loopTime = 0.0f;
    pose->field_0x70 = 0.0f;
    pose->rotationY = 0.0f;
    pose->field_0x7C = 0.0f;
    pose->type = type;
    pose->paperId = -1;
    printf("[animPoseEntry] Calling animPose_AllocBuffer for poseId=%d, fileId=%d, mBufferPosNum=0x%X...\n", poseId, pose->fileId, animPoseGetBaseData(pose)->mBufferPosNum); fflush(stdout);
    animPose_AllocBuffer(pose);
    printf("[animPoseEntry] animPose_AllocBuffer done!\n"); fflush(stdout);
    pose->gxCallback = NULL;
    pose->disableDraw = 0;
    pose->materialFlag = 0;
    pose->materialLightFlag = 0;
    pose->materialColor = (GXColor){ 0xFF, 0xFF, 0xFF, 0xFF };
    pose->materialColor2 = (GXColor){ 0xFF, 0xFF, 0xFF, 0xFF };
    pose->field_0xFC = 1.0f;
    pose->field_0xF8 = 1.0f;
    pose->field_0x100 = 0.0f;
    pose->localTimeRate = 1.0f;
    MTXIdentity(pose->matrix);
    if (!strcmp(animName, "tst_vivi")) {
        pose->vivianType = 1;
        pose->vivianGroupNo = &vivihimoTailGroupNo[0];
    } else if (!strcmp(animName, "c_vivian")) {
        pose->vivianType = 1;
        pose->vivianGroupNo = &vivihimoTailGroupNo[1];
    } else if (!strcmp(animName, "c_maririn")) {
        pose->vivianType = 2;
        pose->vivianGroupNo = &vivihimoTailGroupNo[2];
    } else if (!strcmp(animName, "c_majyorin")) {
        pose->vivianType = 3;
        pose->vivianGroupNo = &vivihimoTailGroupNo[3];
    } else { // if none match
        pose->vivianType = 0;
        pose->vivianGroupNo = &_dummy;
    }
    pose->dispCallback = NULL;
    pose->dispUserDataCallback = NULL;
    return poseId;
}

s32 animPaperPoseEntry(const char* animName, s32 group) {
    AnimationPoseData* data;
    AnimationPose* pose;
    s32 i, poseId;

    for (i = 0, poseId = 0; i < wp->totalPoses; i++, poseId++) {
        pose = &wp->poses[i];
        if (pose->flags & 1) {
            if (pose->typeFlags & 1 && pose->typeFlags & 2 && pose->type == group) {
                data = *wp->files[pose->fileId].handle->data;
                if (!strcmp(data->fileName, animName)) {
                    break;
                }
            }
        }
    }
    if (poseId == wp->totalPoses) {
        poseId = -1;
    }
    if (poseId < 0) {
        poseId = animPoseEntry(animName, group);
        if (poseId != -2) {
            pose = &wp->poses[poseId];
            pose->typeFlags = 3;
            pose->references = 0;
            pose->flags |= 1;
        }
    } else {
        wp->poses[poseId].references++;
    }
    return poseId;
}

BOOL animEffectAsync(const char* animName, u32 group) {
    s32 model, anim;
    void** ag2tg;

    switch (group) {
        case 0:
            fileSetCurrentArchiveType(1);
            break;
        case 1:
            fileSetCurrentArchiveType(2);
            break;
        case 2:
            fileSetCurrentArchiveType(0);
            break;
    }
    model = fileAsyncf(5, NULL, "%s/%s", "a", animName);

    return FALSE;
}

void animPosePeraOn(s32 poseId) {
    wp->poses[poseId].flags |= 8;
}

void animPosePeraOff(s32 poseId) {
    wp->poses[poseId].flags &= ~8;
}

void animPosePaperPeraOn(s32 poseId) {
    AnimationPose *src, *dst;

    src = animPoseGetAnimPosePtr(poseId);
    dst = &wp->poses[src->paperId];
    dst->flags |= 8;
    dst->field_0x70 = src->field_0x70;
    dst->rotationY = src->rotationY;
    dst->field_0x7C = src->field_0x7C;
}

void animPosePaperPeraOff(s32 poseId) {
    AnimationPose* src;

    src = animPoseGetAnimPosePtr(poseId);
    wp->poses[src->paperId].flags &= ~8;
}

void animPoseSetLocalTimeRate(s32 poseId, f32 localTimeRate) {
    wp->poses[poseId].localTimeRate = localTimeRate;
}

// TODO: get animTimeGetTime to inline here
void animPoseSetLocalTime(s32 poseId, f32 localTimeFrames) {
    AnimationPose *pose, *pera;

    pose = &wp->poses[poseId];
    pose->localTime = animTimeGetTime(pose->type) - (localTimeFrames * 16.666666f);
    pose->mLocalTimeInit = pose->localTime;
    if (pose->paperId != -1) {
        pera = &wp->poses[pose->paperId];
        pera->localTime = pose->localTime;
    }
}

void animPoseSetStartTime(s32 poseId, OSTime startTime) {
    wp->poses[poseId].localTime = startTime;
}

void animPoseSetAnim(s32 poseId, const char* animName, BOOL reset) {
    AnimationPose* pose = &wp->poses[poseId];
    AnimationPoseData* data = *wp->files[pose->fileId].handle->data;
    s32 animCount = data->mAnimCount;
    s32 i = (s32)animName;

    if (i <= 0) {
        for (i = 0; i < animCount; i++) {
            if (!strcmp(data->mpAnims[i].name, animName)) {
                break;
            }
            continue;
        }
        goto not_found;
    } else if (i >= animCount) {
not_found:
        i = 0;
    }

    if (reset || i != pose->animId) {
        pose->animId = i;
        if (pose->paperId != -1 && (pose->flags & 2)) {
            pose->flags |= 4;
        }
        pose->localTime = animTimeGetTime(pose->type);
        pose->lastFrameZero = -1;
        pose->lastFrameTime = 0.0f;
        pose->field_0x80 = 0;
        pose->loopTime = 0.0f;
    }
}

s32 animPaperPoseGetId(const char* animName, s32 group) {
    AnimationPose* pose;
    AnimationPoseData* data;
    s32 i;

    for (i = 0; i < wp->totalPoses; i++) {
        pose = &wp->poses[i];
        if (pose->flags & 1) {
            if (pose->typeFlags & 1 && pose->typeFlags & 2 && pose->type == group) {
                data = *wp->files[pose->fileId].handle->data;
                if (!strcmp(data->fileName, animName)) {
                    break;
                }
            }
        }
    }
    if (i == wp->totalPoses) {
        i = -1;
    }
    return i;
}

void animPoseSetPaperAnimGroup(s32 poseId, const char* animName, BOOL someflag) {
    AnimationPose* pose = &wp->poses[poseId];
    if (!animName) {
        pose->paperId = -1;
        animPoseSetMaterialLightFlagOff(poseId, 1);
    } else {
        pose->paperId = animPaperPoseGetId(animName, pose->type);
        if (pose->paperId < 0) {
            pose->paperId = animPaperPoseGetId(animName, 2);
        }
        *(s32*)&pose->field_0x94[0] = -1;
        pose->mLocalTimeInit = animTimeGetTime(pose->type);
        if (someflag) {
            pose->flags |= 2;
            pose->flags |= 4;
        } else {
            pose->flags &= ~2;
            pose->flags &= ~4;
        }
        animPoseSetMaterialLightFlagOn(poseId, 1);
    }
}

void animPoseSetPaperAnim(s32 poseId, const char* animName) {
    AnimationPose* pose = &wp->poses[poseId];
    AnimationPoseData* data = *wp->files[wp->poses[pose->paperId].fileId].handle->data;
    s32 animCount = data->mAnimCount;
    s32 i = (s32)animName;

    if (i <= 0) {
        for (i = 0; i < animCount; i++) {
            if (!strcmp(data->mpAnims[i].name, animName)) {
                break;
            }
            continue;
        }
        goto not_found;
    } else if (i >= animCount) {
not_found:
        i = 0;
    }

    *(s32*)&pose->field_0x94[0] = i;
    pose->mLocalTimeInit = animTimeGetTime(pose->type);
    if (pose->paperId != -1) {
        if (!(wp->poses[pose->paperId].typeFlags & 2)) {
            animPoseMain(pose->paperId);
        }
    }
}

void animPoseSetEffect(s32 poseId, const char* animName, BOOL someflag) {
    AnimationPose* pose = &wp->poses[poseId];
    if (pose->paperId != -1) {
        if (animName) {
            AnimationPoseData* data = *wp->files[wp->poses[pose->paperId].fileId].handle->data;
            if (!strcmp(data->fileName, animName)) {
                if ((s32)((pose->flags >> 1) & 1) == someflag) {
                    return;
                }
            }
        }
        {
            AnimationPose* effPose = &wp->poses[pose->paperId];
            effPose->references--;
            if (effPose->references < 0) {
                AnimationPoseFile* file = &wp->files[effPose->fileId];
                file->references--;
                if (file->references <= 0) {
                    file->active = FALSE;
                    {
                        AnimationTextureFile* tex = &wp->textures[file->textureId];
                        tex->references--;
                        if (tex->references <= 0) {
                            fileFree(tex->handle);
                            tex->active = FALSE;
                        }
                    }
                    fileFree(file->handle);
                }
                effPose->flags = 0;
            }
        }
    }
    if (!animName) {
        pose->paperId = -1;
        animPoseSetMaterialLightFlagOff(poseId, 1);
    } else {
        s32 effId = animPoseEntry(animName, pose->type);
        if (effId != -2) {
            AnimationPose* effPose = &wp->poses[effId];
            effPose->typeFlags = 1;
            effPose->references = 0;
            effPose->flags |= 1;
        }
        pose->paperId = effId;
        *(s32*)&pose->field_0x94[0] = -1;
        pose->mLocalTimeInit = animTimeGetTime(pose->type);
        if (someflag) {
            pose->flags |= 2;
            pose->flags |= 4;
        } else {
            pose->flags &= ~2;
        }
        animPoseSetMaterialLightFlagOn(poseId, 1);
    }
}

void animPoseSetEffectAnim(s32 poseId, const char* animName, BOOL reset) {
    AnimationPose* pose = &wp->poses[poseId];
    if (pose->paperId != -1) {
        animPoseSetAnim(pose->paperId, animName, reset);
    }
}

void animPoseSetGXFunc(s32 poseId, void (*callback)(s32 wXluStage), BOOL disableDraw) {
    AnimationPose* pose = &wp->poses[poseId];
    pose->gxCallback = callback;
    pose->disableDraw = disableDraw;
}

f32 animPoseGetRadius(s32 poseId) {
#ifdef TARGET_PC
	return 2.0f * (f32)(*(s32*)&animPoseGetAnimBaseDataPtr(poseId)->field_0xC4[4]);
#else
	return lbl_8042C5E4 * (f32)(*(s32*)&animPoseGetAnimBaseDataPtr(poseId)->field_0xC4[4]);
#endif
}

f32 animPoseGetHeight(s32 poseId) {
#ifdef TARGET_PC
	return 2.0f * (f32)(*(s32*)&animPoseGetAnimBaseDataPtr(poseId)->field_0xC4[8]);
#else
	return lbl_8042C5E4 * (f32)(*(s32*)&animPoseGetAnimBaseDataPtr(poseId)->field_0xC4[8]);
#endif
}

f32 animPoseGetLoopTimes(s32 id) {
    return wp->poses[id].loopTime;
}

BOOL animPoseGetPeraEnd(s32 poseId) {
    AnimationPose* pose = animPoseGetAnimPosePtr(poseId);
    if ((pose->flags & 8) == 0) {
        return TRUE;
    } else {
#ifdef TARGET_PC
        return fabs(pose->field_0x7C - pose->rotationY) < 10.0;
#else
        return fabs(pose->field_0x7C - pose->rotationY) < lbl_802CCD20;
#endif
    }
}

void animPoseSetMaterialFlagOn(s32 poseId, u32 mask) {
    wp->poses[poseId].materialFlag |= mask;
}

void animPoseSetMaterialFlagOff(s32 poseId, u32 mask) {
    wp->poses[poseId].materialFlag &= ~mask;
}

void animPoseSetMaterialLightFlagOn(s32 poseId, u32 mask) {
    wp->poses[poseId].materialLightFlag |= mask;
}

void animPoseSetMaterialLightFlagOff(s32 poseId, u32 mask) {
    wp->poses[poseId].materialLightFlag &= ~mask;
}

void animPoseSetMaterialEvtColor(s32 poseId, GXColor color) {
    wp->poses[poseId].materialColor = color;
}

u32 animPoseGetMaterialFlag(s32 poseId) {
    return animPoseGetAnimPosePtr(poseId)->materialFlag;
}

u32 animPoseGetMaterialLightFlag(s32 poseId) {
    return animPoseGetAnimPosePtr(poseId)->materialLightFlag;
}

GXColor animPoseGetMaterialEvtColor(s32 poseId) {
    return animPoseGetAnimPosePtr(poseId)->materialColor;
}

void animPoseMain(s32 poseId) {
}

// pushGXModelMtx_TransformNode__
// pushGXModelMtx_JointNode__

void animSetMaterial_Texture(s32 texCount, s32* texIdRemap, AnimPoseTexEntry* pTexEntries, AnimTexMtx* animEntries,
                             s32 texBindsCapacity, AnimTexBind* texBinds, AnimationTextureFile* texFile) {
}

inline BOOL animSetMaterial_ChangeTexture(AnimationPoseData* data, AnimPoseShapeDraw* draw, AnimationPose* pose,
                                          Mtx mtx, void** texData, GXTexObj* texObj, s32 texType) {
    AnimPoseTexEntry* texEntry;
    AnimTexMtx* texAnim;
    AnimTexBind* bind;
    s32 bindCount, texCount;
    s32 dataId, bindId, frameExt;
    GXTexObj* smartObj;
    int i;

    bind = data->texBinds;
    bindCount = data->texBindCount;
    texAnim = pose->mpTexAnimEntries;
    texEntry = data->texData;
    texCount = draw->texCount;

    for (i = 0; i < texCount; i++) {
        dataId = draw->texDataIds[texCount - i - 1];
        frameExt = texAnim[dataId].mTexBindIdx;
        bindId = texEntry[dataId].bindId + frameExt;
        if (bindId < 0 || bindId >= bindCount) {
            bindId = 0;
            //"Error: animSetMaterial_ChangeTexture: テクスチャがありません frameExt=%d\n"
            printf("Error: animSetMaterial_ChangeTexture: "
                   "\x83\x65\x83\x4E\x83\x58\x83\x60\x83\x83\x82\xAA\x82\xA0\x82\xE8\x82\xDC\x82\xB9\x82\xF1 "
                   "frameExt=%d\n",
                   frameExt);
        }
        if (bind[bindId].texType == texType) {
            if (texObj) {
                smartObj = smartTexObj(texObj, texData);
                GXLoadTexObj(smartObj, i);
                if (mtx) {
                    GXLoadTexMtxImm(mtx, GX_TEXMTX0 + (u32)(i * 3), GX_MTX2x4);
                    GXSetTexCoordGen(i, GX_TG_MTX2x4, GX_TG_TEX0 + i, GX_TEXMTX0 + (u32)(i * 3));
                } else {
                    GXSetTexCoordGen(i, GX_TG_MTX2x4, GX_TG_TEX0 + i, GX_IDENTITY);
                }
            }
            return TRUE;
        }
    }
    return FALSE;
}

// materialProc

// I'm so sorry, whoever gets tasked with matching this function
// note: I detect at least 8 inlined functions here, good luck finding the separation
void renderProc(s32 shapeId) {
    CameraEntry* camera;
    AnimationPoseData* data;
    AnimPoseShape *shapes, *shape;
    AnimPoseShapeDraw *draws, *draw;
    s32 dispMode, drawId;
    s32 posFirstVtx, nrmFirstVtx, clr0FirstVtx;

    f32* buffer;
    s32 nrmVtxSize;
    s32 texCount;
    AnimationPose* pose;
    Mtx* mtx;
    GXTexObj* texObj;
    BOOL texChange1, check1, texChange2, check2;
    GXTexObj* currTexObj;
    u8* texIds;
    s32* firstId;
    AnimDrawCallEntry* drawCall;
    s32 drawCallCount;
    GXColor* clr0VtxPtr;
    s32 posFirstId, nrmFirstId, clr0FirstId;
    Vec *nrmVtxPtr, *posVtxPtr;

    s32 count;
    u16 *posIndexPtr, *nrmIndexPtr, *clr0IndexPtr, *texIndexPtr;
    s32 firstDrawId;
    s32 index;
    s32 texIndex;

    int i, j, k, l;

    // stack variables
    s32 clr0VtxSize, posVtxSize; // TODO: make sure stack, +0xB0, +0xAC
    s32 drawCount;               // TODO: make sure stack, +0xA8
    Mtx camMtx, invCamMtx;
    f32* texVtxIndices[8];
    Vec lightPos, lightPos2; // TODO: rename? 2 is probably implicit stack copy

    if (shapeId != -1) {
        camera = camGetCurPtr();
        MTXConcat(camera->view, *g_modeling_mtx, camMtx);
        GXLoadPosMtxImm(camMtx, 0);
        MTXInvXpose(camMtx, invCamMtx);
        GXLoadNrmMtxImm(invCamMtx, 0);
        GXSetCurrentMtx(0);

        data = wp->mpCurPoseData;
        shapes = data->shapes;
        draws = data->draws;
        shape = &shapes[shapeId];
        dispMode = shape->dispMode;
        drawId = shape->shapeDrawStart;
        posFirstVtx = shape->vtxPos.first;
        nrmFirstVtx = shape->vtxNrm.first;
        clr0FirstVtx = shape->vtxClr0.first;
        drawCount = shape->shapeDrawCount;
        draw = &draws[drawId];

        if ((dispMode == 0 && wp->currDispMode == 2) || dispMode == wp->currDispMode ||
            wp->mpCurPose->materialColor.a != 0xFF || wp->mpCurPose->materialColor2.a != 0xFF) {
            GXSetCullMode(tbl[shape->cullMode]);
            data = wp->mpCurPoseData;
            posVtxSize = (s32)sizeof(Vec) * posFirstVtx;
            clr0VtxSize = (s32)sizeof(GXColor) * clr0FirstVtx;
            nrmVtxSize = nrmFirstVtx;
            buffer = data->texVtxBuffer;
            texVtxIndices[0] = &buffer[(sizeof(f32) * 2) * shape->vtxTex[0].first];
            texVtxIndices[1] = &buffer[(sizeof(f32) * 2) * shape->vtxTex[1].first];
            texVtxIndices[2] = &buffer[(sizeof(f32) * 2) * shape->vtxTex[2].first];
            texVtxIndices[3] = &buffer[(sizeof(f32) * 2) * shape->vtxTex[3].first];
            texVtxIndices[4] = &buffer[(sizeof(f32) * 2) * shape->vtxTex[4].first];
            texVtxIndices[5] = &buffer[(sizeof(f32) * 2) * shape->vtxTex[5].first];
            texVtxIndices[6] = &buffer[(sizeof(f32) * 2) * shape->vtxTex[6].first];
            texVtxIndices[7] = &buffer[(sizeof(f32) * 2) * shape->vtxTex[7].first];

            for (i = 0; i < drawCount; i++, draw++) {
                texCount = draw->texCount;
                if (!wp->mpCurPose->disableDraw) {
                    lightPos = (Vec){ 0.0f, 0.0f, 0.0f };
                    MTXMultVec(*g_modeling_mtx, &lightPos, &lightPos);
                    lightPos2 = lightPos;
                    mapSetMaterialLight(wp->mpCurPose->materialLightFlag, lightPos2);
                    animSetMaterial_Texture(draw->texCount, draw->texDataIds, wp->mpCurPoseData->texData,
                                            wp->mpCurPose->mpTexAnimEntries, wp->mpCurPoseData->texBindCount,
                                            wp->mpCurPoseData->texBinds,
                                            &wp->textures[wp->files[wp->mpCurPose->fileId].textureId]);
                    mapSetMaterialLastStageBlend(wp->mpCurPose->materialFlag, wp->mpCurPose->materialColor,
                                                 wp->mpCurPose->materialColor2);
                    camera = camGetPtr(CAMERA_3D);
                    if (camGetCurPtr() == camera) {
                        mapSetMaterialFog();
                    }
                    mapSetMaterialTev(texCount, draw->drawMode, wp->mpCurPose->materialFlag, *g_modeling_mtx);
                }
                pose = wp->mpCurPose;
                if (pose->typeFlags) {
                    texChange1 = animSetMaterial_ChangeTexture(wp->mpCurPoseData, draw, pose, *wp->paperMtxPtr[0],
                                                               wp->mpTexDatas[0], wp->mpTexObjs[0], 1);
                    check1 = texChange1;
                    if (!texChange1) {
                        texChange2 = animSetMaterial_ChangeTexture(wp->mpCurPoseData, draw, pose, *wp->paperMtxPtr[1],
                                                                   wp->mpTexDatas[1], wp->mpTexObjs[1], 2);
                        check1 = texChange2;
                        check2 = texChange2;
                    } else {
                        check2 = FALSE;
                    }

                    if (check1 && (texChange1 && !wp->field_0xD8 || check2 && !wp->field_0xDC) && !draw->drawMode) {
                        GXSetTevAlphaIn(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_RASA);
                    }
                    if (check1 && !draw->drawMode) {
                        currTexObj = wp->mpCurTexObj;
                        if (currTexObj) {
                            smartTexObj(currTexObj, wp->mpCurTexData);
                            if (wp->currDispMode != 3) {
                                GXSetZMode(GX_TRUE, GX_LEQUAL, GX_TRUE);
                                GXSetBlendMode(GX_BM_NONE, GX_BL_SRCALPHA, GX_BL_INVSRCALPHA, GX_LO_CLEAR);
                                GXSetZCompLoc(GX_FALSE);
                                GXSetAlphaCompare(GX_GEQUAL, 0x80u, GX_AOP_OR, GX_NEVER, 0);
                            }
                            texObj = wp->mpTexObjs[texChange1 == 0];
                            if (texObj) {
                                GXLoadTexObj(texObj, GX_TEXMAP0);
                                GXSetTexCoordGen2(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, GX_TEXMTX0, GX_FALSE,
                                                  GX_PTIDENTITY);
                                GXLoadTexMtxImm(*wp->paperMtxPtr[texChange1 == 0], GX_TEXMTX0, GX_MTX2x4);
                            }
                            GXLoadTexObj(currTexObj, GX_TEXMAP1);
                            GXSetTexCoordGen2(GX_TEXCOORD1, GX_TG_MTX2x4, GX_TG_TEX0, GX_TEXMTX1, GX_FALSE,
                                              GX_PTIDENTITY);
                            GXLoadTexMtxImm(*wp->paperMtxPtr[2], GX_TEXMTX1, GX_MTX2x4);
                            mapSetMaterialTev(2, 12, wp->mpCurPose->materialFlag, *g_modeling_mtx);
                        }
                    }
                }

                pose = wp->mpCurPose;
                if (pose->flags & 0x10) {
                    texObj = wp->mpTexObjs[0];
                    if (texObj) {
                        mtx = wp->paperMtxPtr[0];
                        if (mtx) {
                            animSetMaterial_ChangeTexture(wp->mpCurPoseData, draw, pose, *mtx, wp->mpTexDatas[0],
                                                          texObj, 1);
                        }
                    }
                }

                texIds = draw->texArrayIds;
                firstId = draw->firstIdTex;
                data = wp->mpCurPoseData; // inlined function here
                drawCall = &data->drawCalls[draw->firstDrawCall];
                texCount = draw->texCount;
                drawCallCount = draw->drawCallCount;
                clr0VtxPtr = &data->clr0VtxBuffer[clr0VtxSize / 4];
                posFirstId = draw->firstIdPos;
                nrmFirstId = draw->firstIdNrm;
                clr0FirstId = draw->firstIdClr0;

                if (wp->mbUseFloatScratch) {
                    nrmVtxPtr = &wp->vtxArrayNrm[nrmVtxSize];
                    posVtxPtr = &wp->vtxArrayPos[posVtxSize / sizeof(Vec)];
                } else {
                    pose = wp->mpCurPose;
                    posVtxPtr = &pose->mpVtxArrayPos[posVtxSize / sizeof(Vec)];
                    nrmVtxPtr = &pose->mpVtxArrayNrm[nrmVtxSize];
                }
                GXClearVtxDesc();
                GXSetVtxDesc(GX_VA_POS, GX_INDEX16);
                GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_POS, GX_POS_XYZ, GX_F32, 0);
                GXSetArray(GX_VA_POS, posVtxPtr, sizeof(Vec));
                GXSetVtxDesc(GX_VA_NRM, GX_INDEX16);
                GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_NRM, GX_NRM_XYZ, GX_F32, 0);
                GXSetArray(GX_VA_NRM, nrmVtxPtr, sizeof(Vec));
                GXSetVtxDesc(GX_VA_CLR0, GX_INDEX16);
                GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_CLR0, GX_CLR_RGBA, GX_RGBA8, 0);
                GXSetArray(GX_VA_CLR0, clr0VtxPtr, sizeof(GXColor));

                if (texCount == 1) { // just use texIndex[0]
                    GXSetVtxDesc(GX_VA_TEX0, GX_INDEX16);
                    GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_TEX0, GX_TEX_ST, GX_F32, 0);
                    GXSetArray(GX_VA_TEX0, texVtxIndices[texIds[0]], sizeof(f32) * 2);
                    for (j = 0; j < drawCallCount; j++, drawCall++) {
                        data = wp->mpCurPoseData; // doubly inlined function here
                        count = drawCall->count;
                        // indices are stored as u32, has to do math to load the lower 2 bytes
                        posIndexPtr = &data->posIndex[((drawCall->first + posFirstId) * 2) + 1];
                        nrmIndexPtr = &data->nrmIndex[((drawCall->first + nrmFirstId) * 2) + 1];
                        clr0IndexPtr = &data->clr0Index[((drawCall->first + clr0FirstId) * 2) + 1];
                        texIndexPtr = &data->texIndex[texIds[0]][((drawCall->first + firstId[texIds[0]]) * 2) + 1];
                        GXBegin(GX_TRIANGLEFAN, GX_VTXFMT0, (u16)drawCall->count);
                        for (k = 0; k < count; k++) { // unrolled, warning
                            GXPosition1x16(*posIndexPtr);
                            posIndexPtr += 2;
                            GXNormal1x16(*nrmIndexPtr);
                            nrmIndexPtr += 2;
                            GXColor1x16(*clr0IndexPtr);
                            clr0IndexPtr += 2;
                            GXTexCoord1x16(*texIndexPtr);
                            texIndexPtr += 2;
                        }
                    }
                } else { // enumerate texIndex
                    for (j = 0; j < texCount; j++) {
                        GXSetVtxDesc(GX_VA_TEX0 + j, GX_INDEX16);
                        GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_TEX0 + j, GX_TEX_ST, GX_F32, 0);
                        GXSetArray(GX_VA_TEX0 + j, texVtxIndices[texIds[texCount - j - 1]], sizeof(f32) * 2);
                    }
                    for (j = 0; j < drawCallCount; j++, drawCall++) {
                        firstDrawId = drawCall->first;
                        data = wp->mpCurPoseData; // doubly inlined function here
                        count = drawCall->count;
                        // indices are stored as u32, has to do math to load the lower 2 bytes
                        posIndexPtr = &data->posIndex[((drawCall->first + posFirstId) * 2) + 1];
                        nrmIndexPtr = &data->nrmIndex[((drawCall->first + nrmFirstId) * 2) + 1];
                        clr0IndexPtr = &data->clr0Index[((drawCall->first + clr0FirstId) * 2) + 1];

                        for (k = 0, texIndex = 0; k < count; k++) {
                            GXPosition1x16(*posIndexPtr);
                            GXNormal1x16(*nrmIndexPtr);
                            GXColor1x16(*clr0IndexPtr);
                            for (l = 0; l < texCount; l++) {
                                index = texIds[texCount - l - 1];
                                GXTexCoord1x16(wp->mpCurPoseData
                                                   ->texIndex[index][((firstDrawId + firstId[index]) * 2) + texIndex]);
                            }
                            texIndex += 2;
                            posIndexPtr += 2;
                            nrmIndexPtr += 2;
                            clr0IndexPtr += 2;
                        }
                    }
                }
            }
        }
    }
}

// dispProc

void animPoseDraw(s32 poseId, f32 x, f32 y, f32 z, f32 rotY, f32 scale, s32 xluMode) {
    Mtx matrix;

    MTXTrans(matrix, x, y, z);
    animPoseDrawMtx(poseId, matrix, xluMode, rotY, scale);
}

// _animPoseDrawMtx

void animPoseDrawMtx(s32 poseId, Mtx matrix, s32 xluMode, f32 rotY, f32 scale) {
}

void animSetPaperTexObj(GXTexObj* a1, GXTexObj* a2, GXTexObj* a3, void* a4, void* a5, void* a6, s32 a7, s32 a8) {
    wp->mpTexObjs[0] = a4 ? a1 : NULL;
    wp->mpTexObjs[1] = a5 ? a2 : NULL;
    wp->mpCurTexObj = a6 ? a3 : NULL;
    wp->mpTexDatas[0] = a4;
    wp->mpTexDatas[1] = a5;
    wp->mpCurTexData = a6;
    wp->field_0xD8 = a7;
    wp->field_0xDC = a7;
}

void animPaperPoseRelease(s32 poseId) {
    AnimationPose* pose = &wp->poses[poseId];
    pose->references--;
    if (pose->references < 0) {
        AnimationPoseFile* file = &wp->files[pose->fileId];
        file->references--;
        if (file->references <= 0) {
            AnimationTextureFile* tex;
            file->active = 0;
            tex = &wp->textures[file->textureId];
            tex->references--;
            if (tex->references <= 0) {
                fileFree(tex->handle);
                tex->active = 0;
            }
            fileFree(file->handle);
        }
        pose->flags = 0;
    }
}

void animPoseRelease(s32 poseId) {
    AnimationPose* pose = &wp->poses[poseId];
    if (pose->typeFlags != 0U) {
        animPaperPoseRelease(poseId);
    } else {
        AnimationPoseFile* file;
        if (pose->flags & 0x40) {
            smartFree(*(smartEntry**)&pose->field_0x15C[0]);
        }
        if (pose->flags & 0x80) {
            smartFree(*(smartEntry**)&pose->field_0x15C[8]);
        }
        file = &wp->files[pose->fileId];
        file->references--;
        if (file->references <= 0) {
            AnimationTextureFile* tex;
            file->active = 0;
            tex = &wp->textures[file->textureId];
            tex->references--;
            if (tex->references <= 0) {
                fileFree(tex->handle);
                tex->active = 0;
            }
            fileFree(file->handle);
        }
        pose->flags = 0;
    }
}

void animPoseAutoRelease(s32 type) {
    s32 i;
    for (i = 0; i < wp->totalPoses; i++) {
        AnimationPose* pose = &wp->poses[i];
        if ((pose->flags & 1) && pose->type == type) {
            animPoseRelease(i);
        }
    }
    if (type == 1) {
        wp->inBattle = FALSE;
    }
    animPoseRefresh();
}

void animPaperPoseDisp(CameraId camId, void* param) {
    AnimationPose* pose;
    int i;

    for (i = 0; i < wp->totalPoses; i++) {
        pose = &wp->poses[i];
        if (pose->flags & 1 && pose->paperId != -1) {
            animPaperPoseDispSub(0, pose);
        }
    }
}

void animPaperPoseDispSub(s32 unused, AnimationPose* pose) {
}

// animPoseDisp_MakeExtTexture

void animSetPaperTexMtx(Mtx mtx1, Mtx mtx2, Mtx mtx3) {
    if (!mtx1) {
        wp->paperMtxPtr[0] = NULL;
    } else {
        MTXCopy(mtx1, wp->paperMtx[0]);
        wp->paperMtxPtr[0] = &wp->paperMtx[0];
    }
    if (!mtx2) {
        wp->paperMtxPtr[1] = NULL;
    } else {
        MTXCopy(mtx2, wp->paperMtx[1]);
        wp->paperMtxPtr[1] = &wp->paperMtx[1];
    }
    if (!mtx3) {
        wp->paperMtxPtr[2] = NULL;
    } else {
        MTXCopy(mtx3, wp->paperMtx[2]);
        wp->paperMtxPtr[2] = &wp->paperMtx[2];
    }
}

BOOL animGroupBaseAsync(const char* name, s32 group, void* callback) {
    s32 res;
    switch (group) {
        case 0:
            fileSetCurrentArchiveType(1);
            break;
        case 1:
            fileSetCurrentArchiveType(2);
            break;
        case 2:
            fileSetCurrentArchiveType(0);
            break;
    }
    res = fileAsyncf(5, (FileCallback)callback, "%s/%s", "a", name);
    if (res == -1 || res == 1) {
        return TRUE;
    }
    return FALSE;
}

AnimationPose* animPoseGetAnimPosePtr(s32 poseId) {
    return &wp->poses[poseId];
}

AnimData* animPoseGetAnimDataPtr(s32 poseId) {
    AnimationPose* pose = animPoseGetAnimPosePtr(poseId);
    return animPoseGetAnimBaseDataPtr(poseId)->mpAnims[pose->animId].data;
}

AnimationPoseData* animPoseGetAnimBaseDataPtr(s32 poseId) {
    AnimationPose* pose = animPoseGetAnimPosePtr(poseId);
    return *wp->files[pose->fileId].handle->data;
}

AnimTableEntry* animPoseGetCurrentAnim(s32 poseId) {
    AnimationPoseData* data = animPoseGetAnimBaseDataPtr(poseId);
    return &data->mpAnims[animPoseGetAnimPosePtr(poseId)->animId];
}

// animPoseTestXLU
// evalProc

void animPoseWorldPositionEvalOn(s32 poseId) {
    AnimationPose* pose = &wp->poses[poseId];
    AnimationPoseData* data = animPoseGetBaseData(pose);
    u8 allocType;

    if (!(pose->flags & 0x40)) {
        pose->flags |= 0x40;
        switch (pose->type) {
            case 0:
                allocType = 1;
                break;
            case 1:
                allocType = 2;
                break;
            case 2:
                allocType = 0;
                break;
        }
        *(s32*)&pose->field_0x15C[4] = data->groupCount;
        *(smartEntry**)&pose->field_0x15C[0] = smartAlloc(sizeof(Vec) * data->groupCount, allocType);
    }
}

void L_animPoseWorldPositionEvalOff(s32 poseId) {
    AnimationPose* pose = &wp->poses[poseId];
    if (pose->flags & 0x40) {
        pose->flags &= ~0x40;
        smartFree(*(smartEntry**)&pose->field_0x15C[0]);
        *(smartEntry**)&pose->field_0x15C[0] = NULL;
    }
}

void animPoseWorldMatrixEvalOn(s32 poseId) {
    AnimationPose* pose = &wp->poses[poseId];
    AnimationPoseData* data = animPoseGetBaseData(pose);
    u8 allocType;

    if (!(pose->flags & 0x80)) {
        pose->flags |= 0x80;
        switch (pose->type) {
            case 0:
                allocType = 1;
                break;
            case 1:
                allocType = 2;
                break;
            case 2:
                allocType = 0;
                break;
        }
        *(s32*)&pose->field_0x15C[12] = data->groupCount;
        *(smartEntry**)&pose->field_0x15C[8] = smartAlloc(sizeof(Mtx) * data->groupCount, allocType);
    }
}

// animPoseVivianMain

s32 animPoseGetVivianType(s32 poseId) {
    return animPoseGetAnimPosePtr(poseId)->vivianType;
}

void animPoseSetDispCallBack(s32 poseId, void* callback, void* userData) {
    AnimationPose* pose = animPoseGetAnimPosePtr(poseId);
    pose->dispCallback = callback;
    pose->dispUserDataCallback = userData;
}

void animPoseDrawShape(s32 poseId, s32 shapeId) {
    AnimationPose* pose = animPoseGetAnimPosePtr(poseId);
    AnimationPose* curPose = wp->mpCurPose;
    AnimData* curAnimData = wp->mpCurAnimData;
    AnimationPoseData* curPoseData = wp->mpCurPoseData;
    GXTexObj* texObj0 = wp->mpTexObjs[0];
    GXTexObj* texObj1 = wp->mpTexObjs[1];
    GXTexObj* curTexObj = wp->mpCurTexObj;
    void* texData0 = wp->mpTexDatas[0];
    void* texData1 = wp->mpTexDatas[1];
    void* curTexData = wp->mpCurTexData;
    Mtx* paperMtx0 = wp->paperMtxPtr[0];
    Mtx* paperMtx1 = wp->paperMtxPtr[1];
    Mtx* paperMtx2 = wp->paperMtxPtr[2];
    u32 d8 = wp->field_0xD8;
    u32 dc = wp->field_0xDC;

    wp->mpCurPose = pose;
    wp->mpCurAnimData = animPoseGetBaseData(pose)->mpAnims[pose->animId].data;
    wp->mpCurPoseData = animPoseGetBaseData(pose);
    wp->mpTexObjs[0] = NULL;
    wp->mpTexObjs[1] = NULL;
    wp->mpCurTexObj = NULL;
    wp->mpTexDatas[0] = NULL;
    wp->mpTexDatas[1] = NULL;
    wp->mpCurTexData = NULL;
    wp->paperMtxPtr[0] = NULL;
    wp->paperMtxPtr[1] = NULL;
    wp->paperMtxPtr[2] = NULL;
    wp->field_0xD8 = 0;
    wp->field_0xDC = 0;

    if (pose->lastFrameZero == -1) {
        s32 idx = (s32)(pose - wp->poses) / (s32)sizeof(AnimationPose);
        if (pose->typeFlags & 2) {
            AnimationPoseData* data = animPoseGetBaseData(pose);
            s32 posNum = data->mBufferPosNum;
            s32 nrmNum = data->mBufferNrmNum;
            if (posNum + nrmNum <= (s32)wp->mFloatScratchRP) {
                if ((s32)wp->mFloatScratchWP + (posNum + nrmNum) > (s32)wp->mFloatScratchRP) {
                    wp->mFloatScratchWP = 0;
                }
                wp->mbUseFloatScratch = 1;
                wp->vtxArrayPos = &wp->mpFloatScratch[wp->mFloatScratchWP];
                wp->mFloatScratchWP += posNum;
                wp->vtxArrayNrm = &wp->mpFloatScratch[wp->mFloatScratchWP];
                wp->mFloatScratchWP += nrmNum;
                animPoseMain(idx);
                wp->mbUseFloatScratch = 0;
            } else {
                animPoseMain(idx);
            }
        } else {
            animPoseMain(idx);
        }
    }

    renderProc(shapeId);

    wp->mpCurPose = curPose;
    wp->mpCurAnimData = curAnimData;
    wp->mpCurPoseData = curPoseData;
    wp->mpTexObjs[0] = texObj0;
    wp->mpTexObjs[1] = texObj1;
    wp->mpCurTexObj = curTexObj;
    wp->mpTexDatas[0] = texData0;
    wp->mpTexDatas[1] = texData1;
    wp->mpCurTexData = curTexData;
    wp->paperMtxPtr[0] = paperMtx0;
    wp->paperMtxPtr[1] = paperMtx1;
    wp->paperMtxPtr[2] = paperMtx2;
    wp->field_0xD8 = d8;
    wp->field_0xDC = dc;
}

s32 animPoseGetShapeIdx(s32 poseId, const char* name) {
    AnimationPoseData* data;
    AnimPoseShape* shape;
    s32 i;

    data = animPoseGetAnimBaseDataPtr(poseId);
    shape = data->shapes;
    for (i = 0; i < data->shapeCount; i++, shape++) {
        if (!strcmp(shape->name, name)) {
            return i;
        }
    }
    return -1;
}

s32 animPoseGetGroupIdx(s32 poseId, const char* name) {
    AnimationPoseData* data;
    AnimPoseGroup* group;
    s32 i;

    data = animPoseGetAnimBaseDataPtr(poseId);
    group = data->groups;
    for (i = 0; i < data->groupCount; i++, group++) {
        if (!strcmp(group->name, name)) {
            return i;
        }
    }
    return -1;
}

const char* animPoseGetGroupName(s32 poseId, s32 group) {
    return animPoseGetAnimBaseDataPtr(poseId)->groups[group].name;
}
