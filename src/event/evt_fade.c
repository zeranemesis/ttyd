#include "evt/evt_fade.h"
#include "driver/fadedrv.h"
#include "driver/camdrv.h"
#include "manager/evtmgr_cmd.h"
#include "mario/mariost.h"
#include <dolphin/gx.h>

extern GlobalWork* gp;
#define wp gp

//.sdata2
extern GXColor lbl_8042F568;
extern GXColor lbl_8042F56C;
extern GXColor lbl_8042F570;
const char dat_8041c798[4] = "\x83\x68";
const char dat_8041c79c[4] = "\x83\x69";
const char dat_8041c7a0[4] = "\x83\x6a";

USER_FUNC(evt_fade_reset) {
	fadeReset(evtGetValue(event, event->args[0]));
	return EVT_RETURN_DONE;
}

USER_FUNC(evt_fade_softfocus_onoff) {
	if (evtGetValue(event, event->args[0])) {
		fadeSoftFocusOn();
	} else {
		fadeSoftFocusOff();
	}
	return EVT_RETURN_DONE;
}

USER_FUNC(evt_fade_tec_onoff) {
	s32* args = event->args;
	s32 arg1, arg2;

	arg1 = evtGetValue(event, *args++);
	arg2 = evtGetValue(event, *args++);
	if (arg1) {
		if (arg2) {
			fadeTecSoftOn();
		} else {
			fadeTecOn();
		}
	} else if (arg2) {
		fadeTecSoftOff();
	} else {
		fadeTecOff();
	}
	return EVT_RETURN_DONE;
}

USER_FUNC(evt_fade_set_mapchange_type) {
	s32* args = event->args;
	s32 arg1, arg2, arg3, arg4, arg5;

	arg1 = evtGetValue(event, *args++);
	arg2 = evtGetValue(event, *args++);
	arg3 = evtGetValue(event, *args++);
	arg4 = evtGetValue(event, *args++);
	arg5 = evtGetValue(event, *args++);
	if (!arg1) {
		if (arg2 != -1) {
			wp->mapFadeInType = arg2;
		}
		if (arg3 != -1) {
			wp->mapFadeInDuration = arg3;
		}
		if (arg4 != -1) {
			wp->mapFadeOutType = arg4;
		}
		if (arg5 != -1) {
			wp->mapFadeOutDuration = arg5;
		}
	} else {
		if (arg2 != -1) {
			wp->areaFadeInType = arg2;
		}
		if (arg3 != -1) {
			wp->areaFadeInDuration = arg3;
		}
		if (arg4 != -1) {
			wp->areaFadeOutType = arg4;
		}
		if (arg5 != -1) {
			wp->areaFadeOutDuration = arg5;
		}
	}
	return EVT_RETURN_DONE;
}

USER_FUNC(evt_fade_set_anim_virtual_pos) {
	s32* args = event->args;
	f32 x = evtGetFloat(event, args[0]);
	f32 y = evtGetFloat(event, args[1]);
	f32 z = evtGetFloat(event, args[2]);
	fadeSetAnimVirtualPos(x, y, z);
	return EVT_RETURN_DONE;
}

USER_FUNC(evt_fade_set_anim_ofs_pos) {
	s32* args = event->args;
	f32 x = evtGetFloat(event, args[0]);
	f32 y = evtGetFloat(event, args[1]);
	fadeSetAnimOfsPos(x, y);
	return EVT_RETURN_DONE;
}

USER_FUNC(evt_fade_set_spot_pos) {
	s32* args = event->args;
	CameraEntry* camera;
	f32 projection[7];
	f32 viewport[6];
	f32 saved[7];
	f32 sx, sy, sz;
	s32 arg1, arg2, arg3;

	arg1 = evtGetValue(event, *args++);
	arg2 = evtGetValue(event, *args++);
	arg3 = evtGetValue(event, *args++);
	camera = camGetPtr(4);
	GXGetProjectionv(saved);
	GXSetProjection(camera->projection, camera->projType);
	GXGetProjectionv(projection);
	GXGetViewportv(viewport);
	GXProject(arg1, arg2, arg3, camera->view, projection, viewport, &sx, &sy, &sz);
	fadeSetSpotPos(-wp->fbWidth / 2 + sx, wp->efbHeight / 2 - sy);
	GXSetProjectionv(saved);
	return EVT_RETURN_DONE;
}

USER_FUNC(evt_fade_end_wait) {
	return fadeIsFinish() != 1 ? EVT_RETURN_BLOCK : EVT_RETURN_DONE;
}

static inline GXColor fade_color(u8 r, u8 g, u8 b) {
	GXColor color = lbl_8042F570;
	color.r = r;
	color.g = g;
	color.b = b;
	return color;
}

USER_FUNC(evt_fade_entry) {
	s32* args = event->args;
	s32 type;
	s32 duration;
	u8 r, g, b;

	type = evtGetValue(event, *args++);
	duration = evtGetValue(event, *args++);
	r = evtGetValue(event, *args++);
	g = evtGetValue(event, *args++);
	b = evtGetValue(event, *args++);
	fadeEntry(type, duration, fade_color(r, g, b));
	return EVT_RETURN_DONE;
}

USER_FUNC(evt_fade_out) {
	s32 duration = evtGetValue(event, event->args[0]);
	if (isFirstCall) {
		fadeEntry(10, duration, lbl_8042F56C);
	}
	if (fadeIsFinish() != 1) {
		return EVT_RETURN_BLOCK;
	}
	return EVT_RETURN_DONE;
}

USER_FUNC(evt_fade_in) {
	s32 duration = evtGetValue(event, event->args[0]);
	if (isFirstCall) {
		fadeEntry(9, duration, lbl_8042F568);
	}
	if (fadeIsFinish() != 1) {
		return EVT_RETURN_BLOCK;
	}
	return EVT_RETURN_DONE;
}
