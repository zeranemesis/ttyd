#include "evt/evt_bg.h"
#include "manager/evtmgr_cmd.h"

extern GXColor lbl_804360B0;

USER_FUNC(evt_bg_set_scrl_offset) {
	s32* args = event->args;
	f32 x, y;

	x = evtGetFloat(event, args[0]);
	y = evtGetFloat(event, args[1]);
	bgSetScrlOffset(x, y);
	return EVT_RETURN_DONE;
}

USER_FUNC(evt_bg_auto_scroll_onoff) {
	if (evtGetValue(event, *event->args)) {
		bgAutoScrollOff();
	} else {
		bgAutoScrollOn();
	}
	return EVT_RETURN_DONE;
}

USER_FUNC(evt_bg_set_color) {
	s32* args = event->args;
	GXColor out, color;
	s32 v0, v1, v2, v3;

	v0 = evtGetValue(event, args[0]);
	v1 = evtGetValue(event, args[1]);
	v2 = evtGetValue(event, args[2]);
	v3 = evtGetValue(event, args[3]);
	color = lbl_804360B0;
	(GXColor){ 0, 0, 0, 0 };
	color.r = (u8)v0;
	color.g = (u8)v1;
	color.b = (u8)v2;
	color.a = (u8)v3;
	out = color;
	bgSetColor(&out);
	return EVT_RETURN_DONE;
}

USER_FUNC(evt_bg_disp_onoff) {
	if (evtGetValue(event, *event->args)) {
		bgDispOn();
	} else {
		bgDispOff();
	}
	return EVT_RETURN_DONE;
}
