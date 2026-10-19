#include "data/mapdata.h"
#include "manager/evtmgr_cmd.h"

extern void fn_8002FEAC(s32, void*);
extern void* fn_80030040(s32, s32);
extern s32 DemoNumValidPads;
extern s32 partyPoseTbl[];

extern const char lbl_11_rodata_A28[];
extern const char lbl_11_rodata_A30[];
extern const char lbl_11_rodata_A38[];
extern const char lbl_11_rodata_A40[];
extern const char lbl_11_rodata_A48[];
extern const char lbl_11_rodata_A50[];
extern const char lbl_11_rodata_A58[];
extern s32 lbl_11_data_1A38[];
extern s32 lbl_11_data_1E3C[];
extern s32 lbl_11_data_23C0[];
extern s32 lbl_11_data_261C[];
extern s32 lbl_11_data_2CFC[];
extern s32 lbl_11_data_3CF4[];
extern s32 lbl_11_data_4D48[];
extern s32 lbl_11_data_6D48[];
extern s32 lbl_11_data_7100[];

static void* lbl_11_bss_0[2]; // 0x0
const char lbl_11_rodata_A60[4] = "gra";
const s32 lbl_11_rodata_A64 = 0;

__declspec(section ".ctors") extern void (* _ctors[])();
__declspec(section ".dtors") extern void (* _dtors[])();

s32 evt_gra_vivian_pose_set(EventEntry* event) {
	s32* args = event->args;
	s32 index;

	index = evtGetValue(event, args[0]);
	partyPoseTbl[90 + index] = evtGetValue(event, args[1]);
	return 2;
}

void _prolog(void) {
	void (*(*constructor))();

	for (constructor = _ctors; *constructor; constructor++) {
		(*constructor)();
	}

	relSetEvtAddr(lbl_11_rodata_A28, lbl_11_data_1A38);
	relSetEvtAddr(lbl_11_rodata_A30, lbl_11_data_1E3C);
	relSetEvtAddr(lbl_11_rodata_A38, lbl_11_data_23C0);
	relSetEvtAddr(lbl_11_rodata_A40, lbl_11_data_261C);
	relSetEvtAddr(lbl_11_rodata_A48, lbl_11_data_2CFC);
	relSetEvtAddr(lbl_11_rodata_A50, lbl_11_data_3CF4);
	relSetEvtAddr(lbl_11_rodata_A58, lbl_11_data_4D48);
	relSetBtlAddr(lbl_11_rodata_A60, lbl_11_data_6D48, lbl_11_data_7100);
	memcpy(lbl_11_bss_0[0] = fn_80030040(DemoNumValidPads, 60), &partyPoseTbl[90], 0x3c);
}

void _epilog(void) {
	void (*(*destructor))();

	for (destructor = _dtors; *destructor; destructor++) {
		(*destructor)();
	}

	memcpy(&partyPoseTbl[90], lbl_11_bss_0[0], 0x3c);
	fn_8002FEAC(DemoNumValidPads, lbl_11_bss_0[0]);
}

void _unresolved(void) {
}
