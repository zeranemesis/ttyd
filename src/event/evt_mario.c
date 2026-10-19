#include "evt/evt_mario.h"
#include "evt/evt_cmd.h"
#include "mario/mario.h"
#include "mario/mario_sbr.h"
#include "mario/mariost.h"
#include "mario/mario_party.h"
#include "manager/evtmgr.h"
#include "manager/evtmgr_cmd.h"
#include "party.h"

extern void marioBalloonOff(void);
extern void marioBalloonOn(s32 type);

USER_FUNC(evt_mario_key_onoff) {
    s32* args = event->args;

    marioGetPtr();
    if (!evtGetValue(event, args[0])) {
        marioKeyOff();
        partyKeyOff();
    } else {
        marioKeyOn();
        partyKeyOn();
    }
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_mario_cont_onoff) {
    s32* args = event->args;

    if (!evtGetValue(event, args[0])) {
        marioCtrlOff();
    } else {
        marioCtrlOn();
    }
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_mario_set_pos) {
    s32* args = event->args;
    MarioWork* wp = marioGetPtr();
    wp->position.x = evtGetFloat(event, args[0]);
    wp->position.y = evtGetFloat(event, args[1]);
    wp->position.z = evtGetFloat(event, args[2]);
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_mario_get_pos) {
    s32* args = event->args;
    MarioWork* wp = marioGetPtr();
    evtSetFloat(event, args[0], wp->position.x);
    evtSetFloat(event, args[1], wp->position.y);
    evtSetFloat(event, args[2], wp->position.z);
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_mario_set_dir) {
    s32* args = event->args;
    MarioWork* wp = marioGetPtr();
    wp->targetYaw = evtGetFloat(event, args[0]);
    wp->directionView = wp->targetYaw;
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_mario_mov_pos2) {
    s32* args = event->args;
    MarioWork* wp = marioGetPtr();
    wp->position.x = evtGetFloat(event, args[0]);
    wp->position.z = evtGetFloat(event, args[1]);
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_mario_adjust_dir) {
	marioAdjustMoveDir();
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_mario_clear_party) {
	MarioWork* wp = marioGetPtr();
	s32* args = event->args;
	u8* slotBase = (u8*)wp + args[0];
	s8 slot = *(s8*)(slotBase + 0x245);
	if (slot < 0) return EVT_RETURN_DONE;
	if (args[0] == 0) {
		partyKill(slot);
	} else {
		partyKill2(1);
	}
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_mario_dispflag_onoff) {
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_mario_kill_party) {
	MarioWork* wp = marioGetPtr();
	s32* args = event->args;
	u8* slotBase = (u8*)wp + args[0];
	s8 slot = *(s8*)(slotBase + 0x245);
	if (slot < 0) return EVT_RETURN_DONE;
	if (args[0] == 0) {
		partyKill(slot);
	} else {
		partyKill(1);
	}
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_party_set_pos) {
    s32* args = event->args;
    s32 partySlot = evtGetValue(event, args[0]);
    f32 x = evtGetFloat(event, args[1]);
    f32 y = evtGetFloat(event, args[2]);
    f32 z = evtGetFloat(event, args[3]);
    // Party positions if spawned
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_party_stop) {
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_party_cont_onoff) {
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_mario_balloon_bikkuri) {
	MarioWork* wp = marioGetPtr();
	if ((wp->dispFlags & 0x04000000) == 0) {
		marioBalloonOn(0);
		wp->dispFlags |= 0x04000000;
	}
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_mario_balloon_off) {
	MarioWork* wp = marioGetPtr();
	if (wp->dispFlags & 0x04000000) {
		marioBalloonOff();
		wp->dispFlags &= ~0x04000000;
	}
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_party_dispflg_onoff) {
    return EVT_RETURN_DONE;
}
