#pragma once

#include "driver/seqdrv.h"
#include "manager/evtmgr.h"

void seq_battleInit(SequenceWork* work);
void seq_battleMain(SequenceWork* work);
void seq_battleExit(SequenceWork* work);

s32 evt_offscreen_entry(EvtEntry* evt, BOOL isFirstCall);
