#include "evt/evt_cmd.h"
#include "driver/mobjdrv.h"

extern void mobjDelete(const char* name);
extern MapObjectEntry* mobjNameToPtr(const char* name);
extern MapObjectEntry* mobjNameToPtrNoAssert(const char* name);

USER_FUNC(evt_mobj_delete) {
	const char* name = (const char*)evtGetValue(event, event->args[0]);
	mobjDelete(name);
	return EVT_RETURN_DONE;
}

USER_FUNC(evt_mobj_get_kindname) {
	s32* args = event->args;
	const char* name = (const char*)evtGetValue(event, args[0]);
	MapObjectEntry* mobj = mobjNameToPtr(name);
	evtSetValue(event, args[1], (s32)mobj->animName);
	return EVT_RETURN_DONE;
}

USER_FUNC(evt_mobj_check) {
	s32* args = event->args;
	const char* name = (const char*)evtGetValue(event, args[0]);
	MapObjectEntry* mobj = mobjNameToPtrNoAssert(name);
	evtSetValue(event, args[1], (s32)mobj);
	return EVT_RETURN_DONE;
}
