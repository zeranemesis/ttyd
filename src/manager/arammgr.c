#include "manager/arammgr.h"
#include "memory.h"
#include <string.h>

//.bss
ARAMWork work_803F1F30;

//.sdata
extern ARAMWork* lbl_80424720;
#define wp lbl_80424720

void aramMgrInit(void) {
	memset(wp, 0, sizeof(ARAMWork));
	if (!ARCheckInit()) {
		ARInit((u32*)wp, 8);
	}
	ARQInit();
	*((u32*)((u8*)wp + 0x28)) = 0x3FC000;
	*((u32*)((u8*)wp + 0x30)) = 0x40;
	*((u32*)((u8*)wp + 0x24)) = ARAlloc(*((u32*)((u8*)wp + 0x28)));
	*((u32*)((u8*)wp + 0x20)) = *((u32*)((u8*)wp + 0x24));
	*((u32*)((u8*)wp + 0x2C)) = (u32)__memAlloc(HEAP_DEFAULT, *((u32*)((u8*)wp + 0x30)) * 0x38);
	memset((void*)*((u32*)((u8*)wp + 0x2C)), 0, *((u32*)((u8*)wp + 0x30)) * 0x38);
	*((u32*)((u8*)wp + 0x34)) = 0;
	*((u32*)((u8*)wp + 0x38)) = 0;
}
