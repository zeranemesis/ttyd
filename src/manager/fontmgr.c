#include "manager/fontmgr.h"
#include "manager/dvdmgr.h"
#include "mario/mariost.h"
#include "system.h"
#include "memory.h"
#include <dolphin/os.h>
#include <dolphin/mtx.h>
#include <string.h>
#include <stdio.h>

#ifdef TARGET_PC
GXColor fontColTbl[3] = {
	{0x1E, 0x14, 0x14, 0xFF},
	{0xDC, 0xDC, 0xDC, 0xFF},
	{0xB4, 0x78, 0x14, 0xFF}
};
Vec fontScale;
BOOL fontEdge;
GXColor fontColor;
u8 fontAlpha;
GXTexObj fgTexObj;
GXTexObj bgTexObj;
#else
extern GXTexObj fgTexObj;
extern GXTexObj bgTexObj;
extern GXColor fontColTbl[2];
extern Vec fontScale;
extern BOOL fontEdge;
extern GXColor fontColor;
extern u8 fontAlpha;
#endif

extern GlobalWork* gp;

BinaryFontHeader* pfh;
BinaryFontInfo* pfi;
BinaryFontWidth* blockWidth;
BinaryFontGlyph* blockGlyph;
BinaryFontMap* blockMap;

void JUTFont_DrawStart(GXColor color);

void fontmgrInit(void) {
	; //stubbed in retail
}

#if defined(TARGET_PC)
#if defined(_MSC_VER)
#define bswap16(x) _byteswap_ushort(x)
#define bswap32(x) _byteswap_ulong(x)
#else
#define bswap16(x) __builtin_bswap16(x)
#define bswap32(x) __builtin_bswap32(x)
#endif
#else
#define bswap16(x) (x)
#define bswap32(x) (x)
#endif

void fontmgrTexSetup(void) {
	DVDEntry* dvd;
	u32 size;
	char path[128];
	int i;
	const char* chosenFont;
	u32 blockcount;
	u8* cur;
	BinaryFontBlock* blk;
	u32 magic;
	u32 blkSize;
	u16 width;
	u16 height;

	if (!pfh) {
#ifdef TARGET_PC
		printf("[fontmgrTexSetup] Starting fontmgrTexSetup, dvdroot=%s\n", getMarioStDvdRoot()); fflush(stdout);
#endif
		chosenFont = "papermarioset_EU";
		sprintf(path, "%s/f/%s.bfn", getMarioStDvdRoot(), chosenFont);
		dvd = DVDMgrOpen(path, 2, 0);
		if (!dvd) {
			chosenFont = "papermarioset_JPN";
			sprintf(path, "%s/f/%s.bfn", getMarioStDvdRoot(), chosenFont);
			dvd = DVDMgrOpen(path, 2, 0);
		}
		if (!dvd) {
#ifdef TARGET_PC
			printf("[fontmgrTexSetup] Could not open font file!\n"); fflush(stdout);
#endif
			return;
		}

		size = OSRoundUp32B(DVDMgrGetLength(dvd));
#ifdef TARGET_PC
		printf("[fontmgrTexSetup] Loading %s.bfn (size=%u)...\n", chosenFont, size); fflush(stdout);
#endif
		pfh = __memAlloc(HEAP_DEFAULT, size);
		DVDMgrRead(dvd, pfh, size, 0);
		DVDMgrClose(dvd);

		blockcount = bswap32(pfh->blockcount);
#ifdef TARGET_PC
		printf("[fontmgrTexSetup] BFN magic: %08X, blocks: %u\n", bswap32(pfh->magic), blockcount); fflush(stdout);
#endif

		cur = (u8*)(pfh + 1);
		for (i = 0; i < (int)blockcount; i++) {
			blk = (BinaryFontBlock*)cur;
			magic = bswap32(blk->magic);
			blkSize = bswap32(blk->size);
			if (magic == 0x494E4631) { // 'INF1'
				pfi = (BinaryFontInfo*)blk;
			}
			else if (magic == 0x474C5931) { // 'GLY1'
				blockGlyph = (BinaryFontGlyph*)blk;
			}
			cur += blkSize;
		}

		if (blockGlyph) {
			width = bswap16(blockGlyph->sheetwidth);
			height = bswap16(blockGlyph->sheetheight);
#ifdef TARGET_PC
			printf("[fontmgrTexSetup] GLY1 found: sheet width=%u, height=%u\n", width, height); fflush(stdout);
#endif
			GXInitTexObj(
				&fgTexObj,
				(void*)((u8*)blockGlyph + sizeof(BinaryFontGlyph)),
				width,
				height,
				GX_TF_I4,
				GX_CLAMP,
				GX_CLAMP,
				GX_FALSE);
			GXInitTexObjLOD(
				&fgTexObj,
				GX_LINEAR,
				GX_LINEAR,
				0.0f,
				0.0f,
				0.0f,
				GX_FALSE,
				GX_FALSE,
				GX_ANISO_1);
		}
#ifdef TARGET_PC
		printf("[fontmgrTexSetup] Font setup completed successfully!\n"); fflush(stdout);
#endif
	}
}

//does some fucky stuff on the alpha channel that doesn't change anything
void FontDrawStart(void) {
	fontScale.x = 1.0f;
	fontScale.y = 1.0f;
	fontScale.z = 1.0f;
	fontEdge = FALSE;
	fontColor = fontColTbl[0];
	fontAlpha = 0xFF;
	JUTFont_DrawStart(fontColTbl[0]);
}

void FontDrawStart_alpha(u8 alpha) {
	GXColor start;

	start = fontColTbl[0];
	fontAlpha = alpha;
	fontScale.x = 1.0f;
	fontScale.y = 1.0f;
	fontScale.z = 1.0f;
	fontEdge = FALSE;
	fontColor = fontColTbl[0];
	start.a = (start.a * alpha) / 255;
	JUTFont_DrawStart(start);
}

void JUTFont_DrawStart(GXColor color) {

}

u16 JUTFont_CodeToGlyph(u32 code);

u16 kanjiSearch(u32 code) {
	return JUTFont_CodeToGlyph(code);
}

u16 JUTFont_CodeToGlyph(u32 code) {
	BinaryFontMap* map;
	u32 blockcount;
	u16 glyph, basecode;
	u8 sjis[2];
	u32 lead, sjisIndex;
	s32 max, min;
	u32 i;

	map = (BinaryFontMap*)&pfh[1];
	glyph = pfi->replace;
	blockcount = pfh->blockcount;
	for (i = 0; i < blockcount; i++) {
		if (map->magic == 'MAP1') { //search for a correct mapping
			basecode = map->firstchar;
			if (((u16)code) >= basecode && ((u16)code) <= map->lastchar) {
				switch (map->mapping) {
					case LINEAR_CHARACTER_MAP:
						glyph = (u16)(code - basecode);
						break;

					case SJIS_CHARACTER_MAP:
						*(u16*)sjis = (u16)code;
						lead = sjis[0];
						sjisIndex = sjis[1];
						if (sjis[1] >= 0x80) {
							sjisIndex -= 65;
						}
						else {
							sjisIndex -= 64;
						}

						if (!map->count) {
							glyph = (u16)((((lead - 136) * 188) + sjisIndex) + 702);
						}
						else {
							glyph = (u16)(((lead - 136) * 188) + sjisIndex - 94 + map->table[0]);
						}
					break;

					case TABLE_CHARACTER_MAP:
						glyph = map->table[(u16)code - basecode];
						break;

					case MAP_CHARACTER_MAP:
						max = map->count;
						min = 0;

						while (max >= min) {
							s32 index = (min + max) / 2;
							if ((u16)code == map->table[index * 2]) {
								glyph = map->table[2 * index + 1];
								break;
							}
							if ((u16)code < map->table[2 * index]) {
								max = index - 1;
							}
							else {
								min = index + 1;
							}
						}
					break;
				}
				break;
			}
		}
		map = (BinaryFontMap*)((u8*)map + map->size);
	}
	return glyph;
}

u16 kanjiGetWidth(u16 code) {
	BinaryFontBlock* block = (BinaryFontBlock*)(pfh + 1);
	u8* width = NULL;
	u32 i;

	for (i = 0; i < pfh->blockcount; i++) {
		if (block->magic == 'WID1') {
			BinaryFontWidth* widthBlock = (BinaryFontWidth*)block;
			if (code >= widthBlock->firstcode && code <= widthBlock->lastcode) {
				width = (u8*)block + 0xC + (code - widthBlock->firstcode) * 2;
				break;
			}
		}
		block = (BinaryFontBlock*)((u8*)block + block->size);
	}
	return width[1];
}
