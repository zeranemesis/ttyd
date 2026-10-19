#include "evt/evt_sub.h"
#include "evt/evt_cmd.h"
#include "manager/evtmgr.h"
#include "manager/evtmgr_cmd.h"
#include "mario/mariost.h"
#include "demo/DEMOPad.h"
#include "driver/effdrv.h"
#include "statuswindow.h"
#include "countdown.h"
#include "pmario_sound.h"
#include "driver/swdrv.h"
#include "driver/animdrv.h"
#include "system.h"
#include <dolphin/os.h>
#include <math.h>
#include <stdlib.h>
#include <stdio.h>

#ifdef TARGET_PC
extern GlobalWork* gp;
#define wp gp
#else
extern GlobalWork* wp_80422810;
#define wp wp_80422810
#endif
extern s32 lbl_8042B208;
#define stop lbl_8042B208
extern const char lbl_802CD26C[];
extern const char lbl_802CD1AC[];
extern const f32 lbl_8042C8FC;
extern const f32 lbl_8042C900;
extern const f32 lbl_8042C904;

USER_FUNC(evt_sub_get_language) {
    s32* args = event->args;
    evtSetValue(event, args[0], wp->language);
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_sub_get_system_flag) {
    s32* args = event->args;
    evtSetValue(event, args[0], wp->flags);
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_sub_system_flag_onoff) {
    s32* args = event->args;
    s32 onoff = evtGetValue(event, args[0]);
    s32 flag = args[1];

    if (onoff) {
        wp->flags |= flag;
    } else {
        wp->flags &= ~flag;
    }
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_sub_get_mapname) {
    s32* args = event->args;
    evtSetValue(event, args[0], (s32)wp->currentMapName);
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_sub_get_areaname) {
    s32* args = event->args;
    evtSetValue(event, args[0], (s32)wp->currentAreaName);
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_sub_status_gauge_force_open) {
    statusWinForceOpen();
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_sub_status_gauge_force_close) {
    statusWinForceClose();
    return EVT_RETURN_DONE;
}

USER_FUNC(fn_80054DA0) {
    statusWinForceUpdate();
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_sub_status_gauge_check_update) {
    s32* args = event->args;
    if (statusWinCheckUpdate()) {
        evtSetValue(event, args[0], 1);
    } else {
        evtSetValue(event, args[0], 0);
    }
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_sub_countdown_start) {
    s32* args = event->args;
    s32 a0 = evtGetValue(event, args[0]);
    s32 a1 = evtGetValue(event, args[1]);
    countDownStart(a0, a1);
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_sub_countdown_get_status) {
    s32* args = event->args;
    s32 status = countDownGetStatus();
    evtSetValue(event, args[0], status);
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_sub_countdown_end) {
    countDownEnd();
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_sub_countdown_flag_onoff) {
    s32* args = event->args;
    s32 onoff = evtGetValue(event, args[0]);
    s32 flag = evtGetValue(event, args[1]);

    if (onoff) {
        countDownFlagOn((u16)flag);
    } else {
        countDownFlagOff((u16)flag);
    }
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_sub_countdown_set_restart_time) {
    s32* args = event->args;
    s32 time = evtGetValue(event, args[0]);
    countDownSetRestartTime(time);
    return EVT_RETURN_DONE;
}

USER_FUNC(fn_80054ADC) {
    s32* args = event->args;
    evtSetValue(event, args[0], wp->framerate);
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_sub_animgroup_async) {
    s32* args = event->args;
    const char* name = (const char*)evtGetValue(event, args[0]);
    switch (animGroupBaseAsync(name, wp->inBattle != 0, NULL)) {
        default:
            return EVT_RETURN_DONE;
        case 0:
            break;
	}
	return EVT_RETURN_BLOCK;
}

USER_FUNC(set_stop) {
    s32* args = event->args;
    stop = evtGetValue(event, args[0]);
    return EVT_RETURN_DONE;
}

USER_FUNC(get_stop) {
    s32* args = event->args;
    evtSetValue(event, args[0], stop);
    return EVT_RETURN_DONE;
}

USER_FUNC(stageclear_wait) {
	return effStageClearEndChk(effNameToPtr(lbl_802CD26C)) ? EVT_RETURN_DONE : EVT_RETURN_BLOCK;
}

USER_FUNC(stone_bg_open) {
	EffectEntry* eff = effNameToPtr(lbl_802CD1AC);
    *(u32*)((u8*)eff->userdata + 0x20) = 1;
    return EVT_RETURN_DONE;
}

USER_FUNC(bgm_start_wait) {
    return psndBGMStartCheck(1) ? EVT_RETURN_DONE : EVT_RETURN_BLOCK;
}

USER_FUNC(evt_key_get_button) {
    s32* args = event->args;
    s32 port = evtGetValue(event, *args++);
    s32 button = keyGetButton(port);
    evtSetValue(event, *args++, button);
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_key_get_buttontrg) {
    s32* args = event->args;
    s32 port = evtGetValue(event, *args++);
    s32 button = keyGetButtonTrg(port);
    evtSetValue(event, *args++, button);
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_key_get_dir) {
    s32* args = event->args;
    s32 port = evtGetValue(event, *args++);
    s32 dir = keyGetDir(port);
    evtSetValue(event, *args++, dir);
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_sub_random) {
    s32* args = event->args;
    s32 max = evtGetValue(event, args[0]);
    evtSetValue(event, args[1], rand() % (max + 1));
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_sub_rumble_onoff) {
    s32* args = event->args;
    s32 mode = evtGetValue(event, args[0]);
    s32 chan = evtGetValue(event, args[1]);

    switch (mode) {
        case 1:
            padRumbleOn(chan);
            break;
        case 0:
            padRumbleOff(chan);
            break;
        case 2:
            padRumbleHardOff(chan);
            break;
    }
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_sub_get_sincos) {
    s32* args = event->args;
    f32 deg = evtGetFloat(event, args[0]);
    f32 rad = lbl_8042C8FC * deg / lbl_8042C900;
    evtSetValue(event, args[1], (s32)(lbl_8042C904 * sin(rad)) - EVTDAT_FLOAT_BASE);
    evtSetValue(event, args[2], (s32)(lbl_8042C904 * cos(rad)) - EVTDAT_FLOAT_BASE);
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_sub_get_stopwatch) {
    s32* args = event->args;
    OSTime* timePtr = (OSTime*)evtGetPtrID(evtGetValue(event, args[0]));
    s32 ms = (s32)OSTicksToMilliseconds(*timePtr);
    if (ms > 600000) {
        ms = 600000;
    }
    evtSetValue(event, args[1], ms);
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_sub_save_playtime) {
    s32* args = event->args;
    s32 base = args[0] + EVTDAT_GSW_BASE;
    u8 time[8];
    int i;
    *(OSTime*)time = wp->retraceLocalTime;

    for (i = 0; i < 8; i++) {
        swByteSet(base + i, time[i]);
    }
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_sub_load_progresstime) {
    s32* args = event->args;
    s32 base = args[0] + EVTDAT_GSW_BASE;
    u8 time[8];
    int i;
    s32 elapsed;
    for (i = 0; i < 8; i++) {
        time[i] = (u8)swByteGet(base + i);
    }
    elapsed = (s32)OSTicksToMilliseconds(wp->retraceLocalTime - *(OSTime*)time);
    if (elapsed > 86400000) {
        elapsed = 86400000;
    }
    evtSetValue(event, args[1], elapsed);
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_sub_intpl_msec_init) {
    s32* args = event->args;
    event->lwData[11] = evtGetValue(event, args[0]);
    event->lwData[12] = evtGetValue(event, args[1]);
    event->lwData[13] = evtGetValue(event, args[2]);
    event->lwData[15] = evtGetValue(event, args[3]);
    event->savedTime = event->lifetime;
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_sub_intpl_msec_get_value) {
    s32 elapsed = (s32)OSTicksToMilliseconds(event->lifetime - event->savedTime);
    if (elapsed < event->lwData[15]) {
        event->lwData[0] = (s32)intplGetValue(event->lwData[11], elapsed, event->lwData[15], event->lwData[12], event->lwData[13]);
        event->lwData[1] = 1;
    } else {
        event->lwData[0] = event->lwData[13];
        event->lwData[1] = 0;
    }
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_sub_intpl_msec_get_value_para) {
    s32* args = event->args;
    s32 elapsed = (s32)OSTicksToMilliseconds(event->lifetime - event->savedTime);
    if (elapsed < event->lwData[15]) {
        evtSetFloat(event, args[0], intplGetValue(event->lwData[11], elapsed, event->lwData[15], event->lwData[12], event->lwData[13]));
        evtSetValue(event, args[1], 1);
    } else {
        evtSetFloat(event, args[0], (f32)event->lwData[13]);
        evtSetValue(event, args[1], 0);
    }
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_sub_get_dir) {
    s32* args = event->args;
    f32 x1 = (f32)evtGetValue(event, args[0]);
    f32 z1 = (f32)evtGetValue(event, args[1]);
    f32 x2 = (f32)evtGetValue(event, args[2]);
    f32 z2 = (f32)evtGetValue(event, args[3]);

    evtSetFloat(event, args[4], angleABf(x1, z1, x2, z2));
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_sub_get_dist) {
    s32* args = event->args;
    f32 x1 = (f32)evtGetValue(event, args[0]);
    f32 z1 = (f32)evtGetValue(event, args[1]);
    f32 x2 = (f32)evtGetValue(event, args[2]);
    f32 z2 = (f32)evtGetValue(event, args[3]);

    evtSetFloat(event, args[4], distABf(x1, z1, x2, z2));
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_sub_mail_vibration) {
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_sub_intpl_init_float) {
    s32* args = event->args;
    s32 mode = evtGetValue(event, args[0]);
    f32 start = evtGetFloat(event, args[1]);
    f32 end = evtGetFloat(event, args[2]);
    s32 duration = evtGetValue(event, args[3]);

    event->lwData[11] = mode;
    event->lwData[12] = (s32)(lbl_8042C904 * start) - EVTDAT_FLOAT_BASE;
    event->lwData[13] = (s32)(lbl_8042C904 * end) - EVTDAT_FLOAT_BASE;
    event->lwData[14] = 0;
    event->lwData[15] = duration;
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_sub_intpl_get_float) {
    s32* args = event->args;
    s32 a0 = args[0];
    s32 a1 = args[1];
    f32 start = evtGetFloat(event, LW(12));
    f32 end = evtGetFloat(event, LW(13));
    f32 val = intplGetValue(event->lwData[11], event->lwData[14], event->lwData[15], start, end);

    if (a0 != EVTDAT_POINTER_MAX) {
        evtSetFloat(event, a0, val);
    }
    if (a1 != EVTDAT_POINTER_MAX) {
        if (event->lwData[14] >= event->lwData[15]) {
            evtSetValue(event, a1, 0);
        } else {
            evtSetValue(event, a1, 1);
        }
    }
    event->lwData[14]++;
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_sub_intpl_get_value) {
    f32 val = intplGetValue(event->lwData[11], event->lwData[14], event->lwData[15], (f32)event->lwData[12], (f32)event->lwData[13]);
    event->lwData[0] = (s32)val;
    if (event->lwData[14] >= event->lwData[15]) {
        event->lwData[1] = 0;
    } else {
        event->lwData[1] = 1;
    }
    event->lwData[14]++;
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_sub_intpl_get_value_para) {
    s32* args = event->args;
    f32 val = intplGetValue(event->lwData[11], event->lwData[14], event->lwData[15], (f32)event->lwData[12], (f32)event->lwData[13]);
    evtSetFloat(event, args[0], val);
    if (event->lwData[14] >= event->lwData[15]) {
        evtSetValue(event, args[1], 0);
    } else {
        evtSetValue(event, args[1], 1);
    }
    event->lwData[14]++;
    return EVT_RETURN_DONE;
}
