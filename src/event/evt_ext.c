#include "evt/evt_ext.h"

USER_FUNC(evt_ext_reset) {
	extReset();
	return EVT_RETURN_DONE;
}

USER_FUNC(evt_ext_entry) {
	s32* args = event->args;
	s32 v1, v2, v3, v4, v5;

	v1 = evtGetValue(event, args[0]);
	v2 = evtGetValue(event, args[1]);
	v3 = evtGetValue(event, args[2]);
	v4 = evtGetValue(event, args[3]);
	v5 = evtGetValue(event, args[4]);
	extEntry(v1, v2, v3, v4, v5);
	return EVT_RETURN_DONE;
}
