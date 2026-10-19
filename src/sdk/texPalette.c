#include "sdk/texPalette.h"
#include <dolphin/gx.h>
#include <dolphin/os.h>
#include <stdlib.h>
#include <string.h>

#define PALETTE_VERSION 0x0020AF30
#define PALETTE_VERSION_BE 0x30AF2000

#ifdef TARGET_PC
static inline u16 swap16(u16 v) {
#if defined(_MSC_VER)
    return _byteswap_ushort(v);
#else
    return __builtin_bswap16(v);
#endif
}

static inline u32 swap32(u32 v) {
#if defined(_MSC_VER)
    return _byteswap_ulong(v);
#else
    return __builtin_bswap32(v);
#endif
}

#endif // TARGET_PC

void UnpackTexPalette(TEXPalette* pal) {
#ifndef TARGET_PC
    u16 i;
    TEXDescriptor* desc;
#endif
    #ifdef TARGET_PC
    if (!pal) return;

    printf("[UnpackTexPalette] pal=%p, raw versionNumber=0x%08X\n", pal, pal->versionNumber); fflush(stdout);
    // Check if already unpacked in native format
    if (pal->versionNumber == PALETTE_VERSION && pal->descriptorArray && (uintptr_t)pal->descriptorArray > 0x10000) {
        printf("[UnpackTexPalette] Already unpacked!\n"); fflush(stdout);
        return;
    }

    u32 rawVer = pal->versionNumber;
    BOOL isBigEndian = FALSE;
    if (rawVer == PALETTE_VERSION_BE || swap32(rawVer) == PALETTE_VERSION) {
        isBigEndian = TRUE;
    } else if (rawVer != PALETTE_VERSION) {
        printf("[UnpackTexPalette] INVALID VERSION NUMBER: 0x%08X (expected 0x%08X or 0x%08X)\n", rawVer, PALETTE_VERSION, PALETTE_VERSION_BE); fflush(stdout);
        OSPanic("texPalette.c", 0x24, "invalid version number for texture palette");
        return;
    }

    u8* base = (u8*)pal;
    u32 numDescriptors = isBigEndian ? swap32(((u32*)pal)[1]) : pal->numDescriptors;
    u32 descOffset = isBigEndian ? swap32(((u32*)pal)[2]) : (u32)(uintptr_t)pal->descriptorArray;
    printf("[UnpackTexPalette] isBigEndian=%d, numDescriptors=%u, descOffset=0x%X\n", isBigEndian, numDescriptors, descOffset); fflush(stdout);

    // Allocate 64-bit descriptors and headers
    TEXDescriptor* descs = (TEXDescriptor*)malloc(numDescriptors * sizeof(TEXDescriptor));
    memset(descs, 0, numDescriptors * sizeof(TEXDescriptor));

    for (u32 i = 0; i < numDescriptors; i++) {
        u8* descEntry = base + descOffset + i * 8;
        u32 thOffset = isBigEndian ? swap32(*(u32*)(descEntry + 0)) : *(u32*)(descEntry + 0);
        u32 clutOffset = isBigEndian ? swap32(*(u32*)(descEntry + 4)) : *(u32*)(descEntry + 4);

        if (thOffset != 0) {
            u8* rawTh = base + thOffset;
            TEXHeader* th = (TEXHeader*)malloc(sizeof(TEXHeader));
            memset(th, 0, sizeof(TEXHeader));

            if (isBigEndian) {
                th->height = swap16(*(u16*)(rawTh + 0));
                th->width = swap16(*(u16*)(rawTh + 2));
                th->format = swap32(*(u32*)(rawTh + 4));
                u32 dataOffset = swap32(*(u32*)(rawTh + 8));
                th->data = (char*)(base + dataOffset);
                th->wrapS = (GXTexWrapMode)swap32(*(u32*)(rawTh + 12));
                th->wrapT = (GXTexWrapMode)swap32(*(u32*)(rawTh + 16));
                th->minFilter = (GXTexFilter)swap32(*(u32*)(rawTh + 20));
                th->magFilter = (GXTexFilter)swap32(*(u32*)(rawTh + 24));
                u32 biasBits = swap32(*(u32*)(rawTh + 28));
                memcpy(&th->LODBias, &biasBits, sizeof(float));
                th->edgeLODEnable = rawTh[32];
                th->minLOD = rawTh[33];
                th->maxLOD = rawTh[34];
                th->unpacked = TRUE;
            } else {
                memcpy(th, rawTh, 36);
                th->data = (char*)(base + (uintptr_t)th->data);
                th->unpacked = TRUE;
            }
            descs[i].textureHeader = th;
        }

        if (clutOffset != 0) {
            u8* rawClut = base + clutOffset;
            CLUTHeader* clut = (CLUTHeader*)malloc(sizeof(CLUTHeader));
            memset(clut, 0, sizeof(CLUTHeader));

            if (isBigEndian) {
                clut->numEntries = swap16(*(u16*)(rawClut + 0));
                clut->unpacked = TRUE;
                clut->format = (GXTlutFmt)swap32(*(u32*)(rawClut + 4));
                u32 dataOffset = swap32(*(u32*)(rawClut + 8));
                clut->data = (char*)(base + dataOffset);
            } else {
                memcpy(clut, rawClut, 12);
                clut->data = (char*)(base + (uintptr_t)clut->data);
                clut->unpacked = TRUE;
            }
            descs[i].CLUTHeader = clut;
        }
    }

    pal->versionNumber = PALETTE_VERSION;
    pal->numDescriptors = numDescriptors;
    pal->descriptorArray = descs;
#else
    if (pal->versionNumber != PALETTE_VERSION) {
        OSPanic("texPalette.c", 0x24, "invalid version number for texture palette");
    }

    if ((u32)pal->descriptorArray <= 0x80000000) {
        pal->descriptorArray = (TEXDescriptor*)((u8*)pal + (u32)pal->descriptorArray);
        for (i = 0; i < pal->numDescriptors; i++) {
            desc = &pal->descriptorArray[i];
            if (desc->textureHeader) {
                desc->textureHeader = (TEXHeader*)((u8*)pal + (u32)desc->textureHeader);
                if (!desc->textureHeader->unpacked) {
                    desc->textureHeader->data = (char*)((u8*)pal + (u32)desc->textureHeader->data);
                    desc->textureHeader->unpacked = 1;
                }
            }
            if (desc->CLUTHeader) {
                desc->CLUTHeader = (CLUTHeader*)((u8*)pal + (u32)desc->CLUTHeader);
                if (!desc->CLUTHeader->unpacked) {
                    desc->CLUTHeader->data = (char*)((u8*)pal + (u32)desc->CLUTHeader->data);
                    desc->CLUTHeader->unpacked = 1;
                }
            }
        }
    }
#endif
}

TEXDescriptor* TEXGet(TEXPalette* pal, u32 id) {
#ifdef TARGET_PC
    if (!pal || id >= pal->numDescriptors) {
        return NULL;
    }
#endif
    return &pal->descriptorArray[id];
}

void TEXGetGXTexObjFromPalette(TEXPalette* pal, GXTexObj* to, u32 id) {
    TEXDescriptor* tdp = TEXGet(pal, id);
    GXBool mipMap;
#ifdef TARGET_PC
    if (!tdp || !tdp->textureHeader) return;
#endif
    mipMap = (tdp->textureHeader->minLOD == tdp->textureHeader->maxLOD) ? GX_FALSE : GX_TRUE;
    GXInitTexObj(to, tdp->textureHeader->data, tdp->textureHeader->width, tdp->textureHeader->height,
                 (GXTexFmt)tdp->textureHeader->format, tdp->textureHeader->wrapS, tdp->textureHeader->wrapT, mipMap);
    GXInitTexObjLOD(to, tdp->textureHeader->minFilter, tdp->textureHeader->magFilter, tdp->textureHeader->minLOD,
                    tdp->textureHeader->maxLOD, tdp->textureHeader->LODBias, GX_DISABLE,
                    tdp->textureHeader->edgeLODEnable, GX_ANISO_1);
}
