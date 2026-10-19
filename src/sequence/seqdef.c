#include "driver/seqdrv.h"
#include "seq/seq_logo.h"
#include "seq/seq_title.h"
#include "seq/seq_game.h"
#include "seq/seq_mapchange.h"
#include "seq/seq_battle.h"
#include "seq/seq_gameover.h"
#include "seq/seq_load.h"
#include "seq/seq_e3.h"

SequenceCallback seq_data[8][3] = {
	{seq_logoInit, seq_logoMain, seq_logoExit},                     // 0: SEQ_LOGO
	{seq_titleInit, seq_titleMain, seq_titleExit},                 // 1: SEQ_TITLE
	{seq_gameInit, seq_gameMain, seq_gameExit},                    // 2: SEQ_GAME
	{seq_mapChangeInit, seq_mapChangeMain, seq_mapChangeExit},     // 3: SEQ_MAP_CHANGE
	{seq_battleInit, seq_battleMain, seq_battleExit},              // 4: SEQ_BATTLE
	{seq_gameOverInit, seq_gameOverMain, seq_gameOverExit},        // 5: SEQ_GAME_OVER
	{seq_loadInit, seq_loadMain, seq_loadExit},                    // 6: SEQ_LOAD
	{seq_e3Init, seq_e3Main, seq_e3Exit},                          // 7: SEQ_E3
};
