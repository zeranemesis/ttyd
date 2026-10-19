#include "data/mapdata.h"

__declspec(section ".ctors") extern void (* _ctors[])();
__declspec(section ".dtors") extern void (* _dtors[])();

extern const char lbl_18_rodata_628[];
extern const char lbl_18_rodata_630[];
extern const char lbl_18_rodata_638[];
extern const char lbl_18_rodata_640[];
extern const char lbl_18_rodata_648[];
extern const char lbl_18_rodata_650[];
extern const char lbl_18_rodata_658[];
extern const char lbl_18_rodata_660[];
extern const char lbl_18_rodata_668[];
extern s32 lbl_18_data_D28[];
extern s32 lbl_18_data_10BC[];
extern s32 lbl_18_data_16DC[];
extern s32 lbl_18_data_1D3C[];
extern s32 lbl_18_data_267C[];
extern s32 lbl_18_data_5040[];
extern s32 lbl_18_data_52AC[];
extern s32 lbl_18_data_5560[];
extern s32 lbl_18_data_3DCC[];
extern s32 lbl_18_data_4250[];

void _prolog(void) {
	void (*(*constructor))();

	for (constructor = _ctors; *constructor; constructor++) {
		(*constructor)();
	}

	relSetEvtAddr(lbl_18_rodata_628, lbl_18_data_D28);
	relSetEvtAddr(lbl_18_rodata_630, lbl_18_data_10BC);
	relSetEvtAddr(lbl_18_rodata_638, lbl_18_data_16DC);
	relSetEvtAddr(lbl_18_rodata_640, lbl_18_data_1D3C);
	relSetEvtAddr(lbl_18_rodata_648, lbl_18_data_267C);
	relSetEvtAddr(lbl_18_rodata_650, lbl_18_data_5040);
	relSetEvtAddr(lbl_18_rodata_658, lbl_18_data_52AC);
	relSetEvtAddr(lbl_18_rodata_660, lbl_18_data_5560);
	relSetBtlAddr(lbl_18_rodata_668, lbl_18_data_3DCC, lbl_18_data_4250);
}

void _epilog(void) {
	void (*(*destructor))();

	for (destructor = _dtors; *destructor; destructor++) {
		(*destructor)();
	}
}

void _unresolved(void) {
}
