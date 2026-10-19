#include "battle/battle_seq_end.h"
#include "battle/battle.h"
#include "driver/camdrv.h"
#include "manager/evtmgr.h"
#include "manager/evtmgr_cmd.h"

#ifndef TARGET_PC
#define _rank_up_data lbl_803BBBF0
#endif

//.data
RankUpData _rank_up_data[] = {
    {1, 0, "msg_menu_mario_name_0", "butai_01"},
    {10, 1, "msg_menu_mario_name_1", "butai_01"},
    {20, 2, "msg_menu_mario_name_2", "butai_02"},
    {30, 3, "msg_menu_mario_name_3", "butai_03"},
    {0xFFFF, 0, NULL, NULL},
};

#pragma dont_inline on
RankUpData* _get_rank_data(s32 level) {
    RankUpData* rank;

    for (rank = _rank_up_data; level != rank->startLevel; rank++) {
        if (!rank->rankUpMessage) {
            return NULL;
        }
    }
    return rank;
}
#pragma dont_inline reset

s32 _get_rank_up_msg(EventEntry* evt) {
    s32 outIndex = evt->args[0];
    RankUpData* rank = _get_rank_data(pouchGetPtr()->level);
    evtSetValue(evt, outIndex, (s32)rank->rankUpMessage);
    return 2;
}

const char* BattleGetRankNameLabel(s32 level) {
    RankUpData* rank = _rank_up_data;
    RankUpData* previous = NULL;

    while (level >= rank->startLevel) {
        if (rank->startLevel < 0) {
            break;
        }
        previous = rank;
        rank++;
    }
    if (previous == NULL) {
        return NULL;
    }
    return previous->rankUpName;
}

void _ac_help_disp(CameraId camId, void* param) {

}

void _lvup_select_object_disp(CameraId camId, void* param) {

}

s32 _lvup_select_obj_disp_on(void) {
    BattleSeqEndWork* work = _battleWorkPointer->seqEndWork;
    *(s32*)((u8*)work + 0x30) |= 8;
    return 2;
}

s32 _lvup_select_obj_lvup_param_disp(void) {
    BattleSeqEndWork* work = _battleWorkPointer->seqEndWork;
    s32 i;

    for (i = 0; i < 3; i++) {
        *((u8*)work + 0x228 + i * 0x34) = 1;
    }
    return 2;
}

s32 fn_80218118(EventEntry* evt) {
    s32 value = evtGetValue(evt, evt->args[0]);
    *(s32*)((u8*)_battleWorkPointer->seqEndWork + 0x10) = value;
    return 2;
}

s32 _set_confetti_control_evtid(EventEntry* evt) {
    s32* args = evt->args;
    BattleSeqEndWork* work = _battleWorkPointer->seqEndWork;
    s32 value = evtGetValue(evt, args[0]);
    *(s32*)((u8*)work + 0x2A4) = value;
    return 2;
}

void btlseqEnd_DispInit(BattleWork* work) {
    *(s32*)((u8*)work->seqEndWork + 0x30) = 0;
}
