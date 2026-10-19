#include "evt/evt_door.h"
#include "driver/mapdrv.h"
#include "driver/npcdrv.h"
#include "manager/evtmgr.h"
#include "pmario_sound.h"

extern void fn_800DCF0C(s32 flag);
extern s32 lbl_8042B468;

extern s32 lbl_8042B46C;

USER_FUNC(evt_door_load_mapflag) {
	s32* args = event->args;
	const char* mapName = (const char*)evtGetValue(event, args[0]);
	MapObject* obj = mapGetMapObj(mapName);
	if (obj == NULL) {
		mapErrorEntry(0, mapName);
		return EVT_RETURN_DONE;
	}
	obj->flags = lbl_8042B468;
	return EVT_RETURN_DONE;
}

USER_FUNC(evt_door_save_mapflag) {
	s32* args = event->args;
	const char* mapName = (const char*)evtGetValue(event, args[0]);
	MapObject* obj = mapGetMapObj(mapName);
	if (obj == NULL) {
		mapErrorEntry(0, mapName);
		return EVT_RETURN_DONE;
	}
	lbl_8042B468 = obj->flags;
	return EVT_RETURN_DONE;
}

USER_FUNC(npc_hide_onoff) {
	s32* args = event->args;
	NpcEntry* work = (NpcEntry*)evtGetValue(event, args[0]);
	s32 onoff = evtGetValue(event, args[1]);
	const char** names = *(const char***)((u8*)work + 0x34);
	while (*names != NULL) {
		NpcEntry* npc = npcNameToPtr_NoAssert(*names);
		if (npc != NULL) {
			if (onoff) {
				npc->flags |= 0x800;
			} else {
				npc->flags &= ~0x800;
			}
		}
		names++;
	}
	return EVT_RETURN_DONE;
}

USER_FUNC(evt_door_end_wait) {
	return lbl_8042B46C ? 0 : EVT_RETURN_DONE;
}

USER_FUNC(evt_door_set_param) {
	s32* args = event->args;
	s32* param = (s32*)evtGetValue(event, args[0]);
	s32 enabled = evtGetValue(event, args[1]);
	s32 value = evtGetValue(event, args[2]);
	if (!enabled) {
		param[13] = value;
	}
	return EVT_RETURN_DONE;
}

USER_FUNC(door_group) {
	s32* args = event->args;
	s32* param = (s32*)evtGetValue(event, args[0]);
	evtSetValue(event, args[1], (*(s32**)param)[10]);
	return EVT_RETURN_DONE;
}

USER_FUNC(outside_group) {
	s32* args = event->args;
	s32* param = (s32*)evtGetValue(event, args[0]);
	evtSetValue(event, args[1], (*(s32**)param)[9]);
	return EVT_RETURN_DONE;
}

USER_FUNC(inside_group) {
	s32* args = event->args;
	s32* param = (s32*)evtGetValue(event, args[0]);
	evtSetValue(event, args[1], (*(s32**)param)[11]);
	evtSetValue(event, args[2], (*(s32**)param)[12]);
	return EVT_RETURN_DONE;
}

USER_FUNC(init_inside_group) {
	s32* args = event->args;
	s32* param = (s32*)evtGetValue(event, args[0]);
	evtSetValue(event, args[1], (*(s32**)param)[13]);
	evtSetValue(event, args[2], (*(s32**)param)[14]);
	return EVT_RETURN_DONE;
}

USER_FUNC(inout_hit) {
	s32* args = event->args;
	s32* param = (s32*)evtGetValue(event, args[0]);
	evtSetValue(event, args[1], (*(s32**)param)[1]);
	evtSetValue(event, args[2], (*(s32**)param)[2]);
	evtSetValue(event, args[3], (*(s32**)param)[3]);
	evtSetValue(event, args[4], (*(s32**)param)[4]);
	return EVT_RETURN_DONE;
}

USER_FUNC(animation) {
	s32* args = event->args;
	s32* param = (s32*)evtGetValue(event, args[0]);
	evtSetValue(event, args[1], (*(s32**)param)[5]);
	evtSetValue(event, args[2], (*(s32**)param)[6]);
	evtSetValue(event, args[3], (*(s32**)param)[7]);
	evtSetValue(event, args[4], (*(s32**)param)[8]);
	return EVT_RETURN_DONE;
}

USER_FUNC(in_pos) {
	s32* args = event->args;
	f32* pos = (f32*)evtGetValue(event, args[0]);
	evtSetValue(event, args[1], (s32)pos[1]);
	evtSetValue(event, args[2], (s32)pos[3]);
	return EVT_RETURN_DONE;
}

USER_FUNC(out_pos) {
	s32* args = event->args;
	f32* pos = (f32*)evtGetValue(event, args[0]);
	evtSetValue(event, args[1], (s32)pos[4]);
	evtSetValue(event, args[2], (s32)pos[6]);
	return EVT_RETURN_DONE;
}

USER_FUNC(snd_door_in) {
	fn_800DCF0C(0x100);
	return EVT_RETURN_DONE;
}

USER_FUNC(snd_door_out) {
	psndClearFlag(0x100);
	return EVT_RETURN_DONE;
}

USER_FUNC(door_entry) {
	return EVT_RETURN_DONE;
}

EVT_BEGIN(evt_door_entry)
	RETURN
	END
EVT_END();

EVT_BEGIN(evt_door_open_quick)
	RETURN
	END
EVT_END();
