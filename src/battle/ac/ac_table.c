#include "battle/ac/ac_table.h"

#define AC_JT(off) ((void*)((u8*)BtlUnit_GetStatus + (off)))
static void* jumptable_803701A0[] = {
	AC_JT(332), AC_JT(44), AC_JT(56), AC_JT(68), AC_JT(80), AC_JT(92), AC_JT(104),
	AC_JT(116), AC_JT(128), AC_JT(140), AC_JT(152), AC_JT(176), AC_JT(200),
	AC_JT(224), AC_JT(248), AC_JT(272), AC_JT(320), AC_JT(344), AC_JT(356),
	AC_JT(296), AC_JT(308), AC_JT(368), AC_JT(380), AC_JT(392), AC_JT(404),
	AC_JT(424), AC_JT(424), AC_JT(416),
};
#undef AC_JT

extern void battleAcDelete_AirGauge(BattleWork* work);
extern void battleAcDelete_ButtonDown(BattleWork* work);
extern void battleAcDelete_CraneTiming(BattleWork* work);
extern void battleAcDelete_GaugeTiming(BattleWork* work);
extern void battleAcDelete_GaugeTiming2(BattleWork* work);
extern void battleAcDelete_LstRcKeep(BattleWork* work);
extern void battleAcDelete_MonosiriTarget(BattleWork* work);
extern void battleAcDelete_PendulumCraneTiming(BattleWork* work);
extern void battleAcDelete_PowerGauge(BattleWork* work);
extern void battleAcDelete_PowerGaugeLv(BattleWork* work);
extern void battleAcDelete_PowerGaugeLv2(BattleWork* work);
extern void battleAcDelete_Repeatedly(BattleWork* work);
extern void battleAcDelete_RepeatedlyLv(BattleWork* work);
extern void battleAcDelete_Shot(BattleWork* work);
extern void battleAcDelete_ShotTarget(BattleWork* work);
extern void battleAcDelete_SignalTiming(BattleWork* work);
extern void battleAcDelete_StickKeepLeft(BattleWork* work);
extern void battleAcDelete_StickRotate(BattleWork* work);
extern void battleAcDelete_TimingA(BattleWork* work);
extern void battleAcDelete_TimingWide(BattleWork* work);
extern void battleAcDisp_AirGauge(CameraId camId, void* param);
extern void battleAcDisp_ButtonDown(CameraId camId, void* param);
extern void battleAcDisp_CraneTiming(CameraId camId, void* param);
extern void battleAcDisp_GaugeTiming(CameraId camId, void* param);
extern void battleAcDisp_GaugeTiming2(CameraId camId, void* param);
extern void battleAcDisp_LstRcKeep(CameraId camId, void* param);
extern void battleAcDisp_MonosiriTarget(CameraId camId, void* param);
extern void battleAcDisp_PendulumCraneTiming(CameraId camId, void* param);
extern void battleAcDisp_PowerGauge(CameraId camId, void* param);
extern void battleAcDisp_PowerGaugeLv(CameraId camId, void* param);
extern void battleAcDisp_PowerGaugeLv2(CameraId camId, void* param);
extern void battleAcDisp_Repeatedly(CameraId camId, void* param);
extern void battleAcDisp_RepeatedlyLv(CameraId camId, void* param);
extern void battleAcDisp_Shot(CameraId camId, void* param);
extern void battleAcDisp_ShotTarget(CameraId camId, void* param);
extern void battleAcDisp_SignalTiming(CameraId camId, void* param);
extern void battleAcDisp_StickKeepLeft(CameraId camId, void* param);
extern void battleAcDisp_StickRotate(CameraId camId, void* param);
extern void battleAcDisp_TimingA(CameraId camId, void* param);
extern void battleAcDisp_TimingWide(CameraId camId, void* param);
extern s32 battleAcMain_AirGauge(BattleWork* work);
extern s32 battleAcMain_ButtonDown(BattleWork* work);
extern s32 battleAcMain_CraneTiming(BattleWork* work);
extern s32 battleAcMain_GaugeTiming(BattleWork* work);
extern s32 battleAcMain_GaugeTiming2(BattleWork* work);
extern s32 battleAcMain_LstRcKeep(BattleWork* work);
extern s32 battleAcMain_MonosiriTarget(BattleWork* work);
extern s32 battleAcMain_PendulumCraneTiming(BattleWork* work);
extern s32 fn_8022FB14(BattleWork* work);
extern s32 battleAcMain_PowerGaugeLv(BattleWork* work);
extern s32 battleAcMain_PowerGaugeLv2(BattleWork* work);
extern s32 battleAcMain_Repeatedly(BattleWork* work);
extern s32 battleAcMain_RepeatedlyLv(BattleWork* work);
extern s32 battleAcMain_Shot(BattleWork* work);
extern s32 battleAcMain_ShotTarget(BattleWork* work);
extern s32 battleAcMain_SignalTiming(BattleWork* work);
extern s32 battleAcMain_StickKeepLeft(BattleWork* work);
extern s32 battleAcMain_StickRotate(BattleWork* work);
extern s32 battleAcMain_TimingA(BattleWork* work);
extern s32 battleAcMain_TimingWide(BattleWork* work);
extern s32 battleAcResult_AirGauge(BattleWork* work);
extern s32 battleAcResult_ButtonDown(BattleWork* work);
extern s32 battleAcResult_CraneTiming(BattleWork* work);
extern s32 battleAcResult_GaugeTiming(BattleWork* work);
extern s32 battleAcResult_GaugeTiming2(BattleWork* work);
extern s32 battleAcResult_LstRcKeep(BattleWork* work);
extern s32 battleAcResult_MonosiriTarget(BattleWork* work);
extern s32 battleAcResult_PendulumCraneTiming(BattleWork* work);
extern s32 battleAcResult_PowerGauge(BattleWork* work);
extern s32 battleAcResult_PowerGaugeLv(BattleWork* work);
extern s32 battleAcResult_PowerGaugeLv2(BattleWork* work);
extern s32 battleAcResult_Repeatedly(BattleWork* work);
extern s32 battleAcResult_RepeatedlyLv(BattleWork* work);
extern s32 battleAcResult_Shot(BattleWork* work);
extern s32 battleAcResult_ShotTarget(BattleWork* work);
extern s32 battleAcResult_SignalTiming(BattleWork* work);
extern s32 battleAcResult_StickKeepLeft(BattleWork* work);
extern s32 battleAcResult_StickRotate(BattleWork* work);
extern s32 battleAcResult_TimingA(BattleWork* work);
extern s32 battleAcResult_TimingWide(BattleWork* work);

struct ActionCommandTable { ActionCommandEntry entries[21]; u32 end; } ActionCommandList = {
{
	{1, battleAcMain_TimingA, battleAcResult_TimingA, battleAcDisp_TimingA, battleAcDelete_TimingA},
	{11, battleAcMain_TimingWide, battleAcResult_TimingWide, battleAcDisp_TimingWide, battleAcDelete_TimingWide},
	{3, battleAcMain_StickKeepLeft, battleAcResult_StickKeepLeft, battleAcDisp_StickKeepLeft, battleAcDelete_StickKeepLeft},
	{4, battleAcMain_LstRcKeep, battleAcResult_LstRcKeep, battleAcDisp_LstRcKeep, battleAcDelete_LstRcKeep},
	{5, battleAcMain_Repeatedly, battleAcResult_Repeatedly, battleAcDisp_Repeatedly, battleAcDelete_Repeatedly},
	{6, battleAcMain_RepeatedlyLv, battleAcResult_RepeatedlyLv, battleAcDisp_RepeatedlyLv, battleAcDelete_RepeatedlyLv},
	{7, battleAcMain_Shot, battleAcResult_Shot, battleAcDisp_Shot, battleAcDelete_Shot},
	{8, battleAcMain_ShotTarget, battleAcResult_ShotTarget, battleAcDisp_ShotTarget, battleAcDelete_ShotTarget},
	{9, battleAcMain_StickRotate, battleAcResult_StickRotate, battleAcDisp_StickRotate, battleAcDelete_StickRotate},
	{10, battleAcMain_ButtonDown, battleAcResult_ButtonDown, battleAcDisp_ButtonDown, battleAcDelete_ButtonDown},
	{12, battleAcMain_PendulumCraneTiming, battleAcResult_PendulumCraneTiming, battleAcDisp_PendulumCraneTiming, battleAcDelete_PendulumCraneTiming},
	{13, battleAcMain_CraneTiming, battleAcResult_CraneTiming, battleAcDisp_CraneTiming, battleAcDelete_CraneTiming},
	{14, battleAcMain_AirGauge, battleAcResult_AirGauge, battleAcDisp_AirGauge, battleAcDelete_AirGauge},
	{15, fn_8022FB14, battleAcResult_PowerGauge, battleAcDisp_PowerGauge, battleAcDelete_PowerGauge},
	{16, battleAcMain_PowerGaugeLv, battleAcResult_PowerGaugeLv, battleAcDisp_PowerGaugeLv, battleAcDelete_PowerGaugeLv},
	{17, battleAcMain_PowerGaugeLv2, battleAcResult_PowerGaugeLv2, battleAcDisp_PowerGaugeLv2, battleAcDelete_PowerGaugeLv2},
	{18, battleAcMain_SignalTiming, battleAcResult_SignalTiming, battleAcDisp_SignalTiming, battleAcDelete_SignalTiming},
	{19, battleAcMain_GaugeTiming, battleAcResult_GaugeTiming, battleAcDisp_GaugeTiming, battleAcDelete_GaugeTiming},
	{20, battleAcMain_GaugeTiming2, battleAcResult_GaugeTiming2, battleAcDisp_GaugeTiming2, battleAcDelete_GaugeTiming2},
	{21, battleAcMain_MonosiriTarget, battleAcResult_MonosiriTarget, battleAcDisp_MonosiriTarget, battleAcDelete_MonosiriTarget},
	{0, NULL, NULL, NULL, NULL}
},
0
};
