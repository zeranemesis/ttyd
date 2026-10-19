#include "battle/battle_ac_help.h"
#include "battle/battle.h"

s32 cheerEnable;
s32 handBeatEnable;

s32 sac_ac_help_off(void) {
	BattleAcHelpSetDispType(0);
	return 2;
}

s32 sac_ac_help_on(void) {
	BattleAcHelpSetDispType(2);
	return 2;
}

s32 sac_cheer_end(void) {
	cheerEnable = 0;
	return 2;
}

s32 sac_handbeat_end(void) {
	handBeatEnable = 0;
	return 2;
}
