#include "evt/evt_cmd.h"
#include "manager/evtmgr.h"
#include "manager/evtmgr_cmd.h"
#include "evt/evt_bero.h"
#include "evt/evt_mario.h"
#include "evt/evt_cam.h"
#include "evt/evt_msg.h"
#include "evt/evt_sub.h"
#include "evt/evt_door.h"
#include "evt/evt_fade.h"
#include "data/mapdata.h"
#include <stdio.h>
#include <stdint.h>

extern MapData aaa_00;

static s32 aaa_00_root_evt[64];
static s32 aaa_00_prologue1_evt[128];
static s32 aaa_00_prologue2_evt[128];

static void emit_cmd(s32* buf, int* idx, s32 param_count, s32 opcode) {
    buf[(*idx)++] = (param_count << 16) | (opcode & 0xFFFF);
}
static void emit_val(s32* buf, int* idx, s32 val) {
    buf[(*idx)++] = val;
}
static void emit_ptr(s32* buf, int* idx, const void* ptr) {
    buf[(*idx)++] = (s32)(uintptr_t)ptr;
}

void area_aaa_init(void) {
    int idx;

    // Build aaa_00_prologue1_evt
    idx = 0;
    // CALL(evt_mario_key_onoff, 0)
    emit_cmd(aaa_00_prologue1_evt, &idx, 2, OPCODE_USER_FUNC);
    emit_ptr(aaa_00_prologue1_evt, &idx, evt_mario_key_onoff);
    emit_val(aaa_00_prologue1_evt, &idx, 0);

    // CALL(evt_party_stop, 0)
    emit_cmd(aaa_00_prologue1_evt, &idx, 2, OPCODE_USER_FUNC);
    emit_ptr(aaa_00_prologue1_evt, &idx, evt_party_stop);
    emit_val(aaa_00_prologue1_evt, &idx, 0);

    // CALL(evt_mario_set_pos, FLOAT(-100.0f), FLOAT(0.0f), FLOAT(0.0f))
    emit_cmd(aaa_00_prologue1_evt, &idx, 4, OPCODE_USER_FUNC);
    emit_ptr(aaa_00_prologue1_evt, &idx, evt_mario_set_pos);
    emit_val(aaa_00_prologue1_evt, &idx, FLOAT(-100.0f));
    emit_val(aaa_00_prologue1_evt, &idx, FLOAT(0.0f));
    emit_val(aaa_00_prologue1_evt, &idx, FLOAT(0.0f));

    // CALL(evt_cam3d_evt_set, FLOAT(-100.0f), FLOAT(20.0f), FLOAT(0.0f), FLOAT(-100.0f), FLOAT(50.0f), FLOAT(320.0f), FLOAT(25.0f), 0)
    emit_cmd(aaa_00_prologue1_evt, &idx, 9, OPCODE_USER_FUNC);
    emit_ptr(aaa_00_prologue1_evt, &idx, evt_cam3d_evt_set);
    emit_val(aaa_00_prologue1_evt, &idx, FLOAT(-100.0f));
    emit_val(aaa_00_prologue1_evt, &idx, FLOAT(20.0f));
    emit_val(aaa_00_prologue1_evt, &idx, FLOAT(0.0f));
    emit_val(aaa_00_prologue1_evt, &idx, FLOAT(-100.0f));
    emit_val(aaa_00_prologue1_evt, &idx, FLOAT(50.0f));
    emit_val(aaa_00_prologue1_evt, &idx, FLOAT(320.0f));
    emit_val(aaa_00_prologue1_evt, &idx, FLOAT(25.0f));
    emit_val(aaa_00_prologue1_evt, &idx, 0);

    // WAIT_FRAMES(30)
    emit_cmd(aaa_00_prologue1_evt, &idx, 1, OPCODE_WAIT_FRAME);
    emit_val(aaa_00_prologue1_evt, &idx, 30);

    // CALL(evt_msg_print, 0, PTR("pro_00"), 0, PTR("Parakarry"))
    emit_cmd(aaa_00_prologue1_evt, &idx, 5, OPCODE_USER_FUNC);
    emit_ptr(aaa_00_prologue1_evt, &idx, evt_msg_print);
    emit_val(aaa_00_prologue1_evt, &idx, 0);
    emit_ptr(aaa_00_prologue1_evt, &idx, "pro_00");
    emit_val(aaa_00_prologue1_evt, &idx, 0);
    emit_ptr(aaa_00_prologue1_evt, &idx, "Parakarry");

    // WAIT_FRAMES(20)
    emit_cmd(aaa_00_prologue1_evt, &idx, 1, OPCODE_WAIT_FRAME);
    emit_val(aaa_00_prologue1_evt, &idx, 20);

    // CALL(evt_msg_print, 0, PTR("pro_01"), 0, PTR("Luigi"))
    emit_cmd(aaa_00_prologue1_evt, &idx, 5, OPCODE_USER_FUNC);
    emit_ptr(aaa_00_prologue1_evt, &idx, evt_msg_print);
    emit_val(aaa_00_prologue1_evt, &idx, 0);
    emit_ptr(aaa_00_prologue1_evt, &idx, "pro_01");
    emit_val(aaa_00_prologue1_evt, &idx, 0);
    emit_ptr(aaa_00_prologue1_evt, &idx, "Luigi");

    // WAIT_FRAMES(15)
    emit_cmd(aaa_00_prologue1_evt, &idx, 1, OPCODE_WAIT_FRAME);
    emit_val(aaa_00_prologue1_evt, &idx, 15);

    // CALL(evt_msg_print, 0, PTR("pro_02"), 0, PTR("Luigi"))
    emit_cmd(aaa_00_prologue1_evt, &idx, 5, OPCODE_USER_FUNC);
    emit_ptr(aaa_00_prologue1_evt, &idx, evt_msg_print);
    emit_val(aaa_00_prologue1_evt, &idx, 0);
    emit_ptr(aaa_00_prologue1_evt, &idx, "pro_02");
    emit_val(aaa_00_prologue1_evt, &idx, 0);
    emit_ptr(aaa_00_prologue1_evt, &idx, "Luigi");

    // WAIT_FRAMES(15)
    emit_cmd(aaa_00_prologue1_evt, &idx, 1, OPCODE_WAIT_FRAME);
    emit_val(aaa_00_prologue1_evt, &idx, 15);

    // CALL(evt_msg_print, 0, PTR("pro_03"), 0, PTR("Luigi"))
    emit_cmd(aaa_00_prologue1_evt, &idx, 5, OPCODE_USER_FUNC);
    emit_ptr(aaa_00_prologue1_evt, &idx, evt_msg_print);
    emit_val(aaa_00_prologue1_evt, &idx, 0);
    emit_ptr(aaa_00_prologue1_evt, &idx, "pro_03");
    emit_val(aaa_00_prologue1_evt, &idx, 0);
    emit_ptr(aaa_00_prologue1_evt, &idx, "Luigi");

    // WAIT_FRAMES(25)
    emit_cmd(aaa_00_prologue1_evt, &idx, 1, OPCODE_WAIT_FRAME);
    emit_val(aaa_00_prologue1_evt, &idx, 25);

    // CALL(evt_fade_set_mapchange_type, 1, 17, 800, 16, 800)
    emit_cmd(aaa_00_prologue1_evt, &idx, 6, OPCODE_USER_FUNC);
    emit_ptr(aaa_00_prologue1_evt, &idx, evt_fade_set_mapchange_type);
    emit_val(aaa_00_prologue1_evt, &idx, 1);
    emit_val(aaa_00_prologue1_evt, &idx, 17);
    emit_val(aaa_00_prologue1_evt, &idx, 800);
    emit_val(aaa_00_prologue1_evt, &idx, 16);
    emit_val(aaa_00_prologue1_evt, &idx, 800);

    // CALL(evt_bero_mapchange, PTR("gor_10"), 0)
    emit_cmd(aaa_00_prologue1_evt, &idx, 3, OPCODE_USER_FUNC);
    emit_ptr(aaa_00_prologue1_evt, &idx, evt_bero_mapchange);
    emit_ptr(aaa_00_prologue1_evt, &idx, "gor_10");
    emit_val(aaa_00_prologue1_evt, &idx, 0);

    // RETURN
    emit_cmd(aaa_00_prologue1_evt, &idx, 0, OPCODE_END_EVENT);
    emit_cmd(aaa_00_prologue1_evt, &idx, 0, OPCODE_END_SCRIPT);

    // Build aaa_00_prologue2_evt
    idx = 0;
    // CALL(evt_mario_key_onoff, 0)
    emit_cmd(aaa_00_prologue2_evt, &idx, 2, OPCODE_USER_FUNC);
    emit_ptr(aaa_00_prologue2_evt, &idx, evt_mario_key_onoff);
    emit_val(aaa_00_prologue2_evt, &idx, 0);

    // CALL(evt_party_stop, 0)
    emit_cmd(aaa_00_prologue2_evt, &idx, 2, OPCODE_USER_FUNC);
    emit_ptr(aaa_00_prologue2_evt, &idx, evt_party_stop);
    emit_val(aaa_00_prologue2_evt, &idx, 0);

    // CALL(evt_mario_set_pos, FLOAT(-85.0f), FLOAT(30.0f), FLOAT(-54.0f))
    emit_cmd(aaa_00_prologue2_evt, &idx, 4, OPCODE_USER_FUNC);
    emit_ptr(aaa_00_prologue2_evt, &idx, evt_mario_set_pos);
    emit_val(aaa_00_prologue2_evt, &idx, FLOAT(-85.0f));
    emit_val(aaa_00_prologue2_evt, &idx, FLOAT(30.0f));
    emit_val(aaa_00_prologue2_evt, &idx, FLOAT(-54.0f));

    // CALL(evt_mario_set_dir, FLOAT(90.0f))
    emit_cmd(aaa_00_prologue2_evt, &idx, 2, OPCODE_USER_FUNC);
    emit_ptr(aaa_00_prologue2_evt, &idx, evt_mario_set_dir);
    emit_val(aaa_00_prologue2_evt, &idx, FLOAT(90.0f));

    // CALL(evt_cam3d_evt_set, ...)
    emit_cmd(aaa_00_prologue2_evt, &idx, 9, OPCODE_USER_FUNC);
    emit_ptr(aaa_00_prologue2_evt, &idx, evt_cam3d_evt_set);
    emit_val(aaa_00_prologue2_evt, &idx, FLOAT(-85.0f));
    emit_val(aaa_00_prologue2_evt, &idx, FLOAT(35.0f));
    emit_val(aaa_00_prologue2_evt, &idx, FLOAT(-54.0f));
    emit_val(aaa_00_prologue2_evt, &idx, FLOAT(-85.0f));
    emit_val(aaa_00_prologue2_evt, &idx, FLOAT(60.0f));
    emit_val(aaa_00_prologue2_evt, &idx, FLOAT(250.0f));
    emit_val(aaa_00_prologue2_evt, &idx, FLOAT(25.0f));
    emit_val(aaa_00_prologue2_evt, &idx, 0);

    // WAIT_FRAMES(30)
    emit_cmd(aaa_00_prologue2_evt, &idx, 1, OPCODE_WAIT_FRAME);
    emit_val(aaa_00_prologue2_evt, &idx, 30);

    // CALL(evt_msg_print, 0, PTR("pro_05"), 0, PTR("Luigi"))
    emit_cmd(aaa_00_prologue2_evt, &idx, 5, OPCODE_USER_FUNC);
    emit_ptr(aaa_00_prologue2_evt, &idx, evt_msg_print);
    emit_val(aaa_00_prologue2_evt, &idx, 0);
    emit_ptr(aaa_00_prologue2_evt, &idx, "pro_05");
    emit_val(aaa_00_prologue2_evt, &idx, 0);
    emit_ptr(aaa_00_prologue2_evt, &idx, "Luigi");

    // WAIT_FRAMES(20)
    emit_cmd(aaa_00_prologue2_evt, &idx, 1, OPCODE_WAIT_FRAME);
    emit_val(aaa_00_prologue2_evt, &idx, 20);

    // CALL(evt_mario_balloon_bikkuri)
    emit_cmd(aaa_00_prologue2_evt, &idx, 1, OPCODE_USER_FUNC);
    emit_ptr(aaa_00_prologue2_evt, &idx, evt_mario_balloon_bikkuri);

    // CALL(evt_key_get_buttontrg, 0, LW(0))
    emit_cmd(aaa_00_prologue2_evt, &idx, 3, OPCODE_USER_FUNC);
    emit_ptr(aaa_00_prologue2_evt, &idx, evt_key_get_buttontrg);
    emit_val(aaa_00_prologue2_evt, &idx, 0);
    emit_val(aaa_00_prologue2_evt, &idx, LW(0));

    // CALL(evt_mario_balloon_off)
    emit_cmd(aaa_00_prologue2_evt, &idx, 1, OPCODE_USER_FUNC);
    emit_ptr(aaa_00_prologue2_evt, &idx, evt_mario_balloon_off);

    // WAIT_FRAMES(30)
    emit_cmd(aaa_00_prologue2_evt, &idx, 1, OPCODE_WAIT_FRAME);
    emit_val(aaa_00_prologue2_evt, &idx, 30);

    // CALL(evt_fade_set_mapchange_type, 1, 17, 800, 16, 800)
    emit_cmd(aaa_00_prologue2_evt, &idx, 6, OPCODE_USER_FUNC);
    emit_ptr(aaa_00_prologue2_evt, &idx, evt_fade_set_mapchange_type);
    emit_val(aaa_00_prologue2_evt, &idx, 1);
    emit_val(aaa_00_prologue2_evt, &idx, 17);
    emit_val(aaa_00_prologue2_evt, &idx, 800);
    emit_val(aaa_00_prologue2_evt, &idx, 16);
    emit_val(aaa_00_prologue2_evt, &idx, 800);

    // CALL(evt_bero_mapchange, PTR("gor_10"), PTR("prologue"))
    emit_cmd(aaa_00_prologue2_evt, &idx, 3, OPCODE_USER_FUNC);
    emit_ptr(aaa_00_prologue2_evt, &idx, evt_bero_mapchange);
    emit_ptr(aaa_00_prologue2_evt, &idx, "gor_10");
    emit_ptr(aaa_00_prologue2_evt, &idx, "prologue");

    // RETURN
    emit_cmd(aaa_00_prologue2_evt, &idx, 0, OPCODE_END_EVENT);
    emit_cmd(aaa_00_prologue2_evt, &idx, 0, OPCODE_END_SCRIPT);

    // Build aaa_00_root_evt
    idx = 0;
    // CALL(evt_bero_get_entername, LW(0))
    emit_cmd(aaa_00_root_evt, &idx, 2, OPCODE_USER_FUNC);
    emit_ptr(aaa_00_root_evt, &idx, evt_bero_get_entername);
    emit_val(aaa_00_root_evt, &idx, LW(0));

    // IF_STR_EQUAL(LW(0), PTR("prologue"))
    emit_cmd(aaa_00_root_evt, &idx, 2, OPCODE_IF_STR_EQUAL);
    emit_val(aaa_00_root_evt, &idx, LW(0));
    emit_ptr(aaa_00_root_evt, &idx, "prologue");

    // CALL_CHILD(aaa_00_prologue1_evt)
    emit_cmd(aaa_00_root_evt, &idx, 1, OPCODE_RUN_CHILD_EVENT);
    emit_ptr(aaa_00_root_evt, &idx, aaa_00_prologue1_evt);

    // RETURN
    emit_cmd(aaa_00_root_evt, &idx, 0, OPCODE_END_EVENT);

    // END_IF
    emit_cmd(aaa_00_root_evt, &idx, 0, OPCODE_END_IF);

    // IF_STR_EQUAL(LW(0), PTR("prologue2"))
    emit_cmd(aaa_00_root_evt, &idx, 2, OPCODE_IF_STR_EQUAL);
    emit_val(aaa_00_root_evt, &idx, LW(0));
    emit_ptr(aaa_00_root_evt, &idx, "prologue2");

    // CALL_CHILD(aaa_00_prologue2_evt)
    emit_cmd(aaa_00_root_evt, &idx, 1, OPCODE_RUN_CHILD_EVENT);
    emit_ptr(aaa_00_root_evt, &idx, aaa_00_prologue2_evt);

    // RETURN
    emit_cmd(aaa_00_root_evt, &idx, 0, OPCODE_END_EVENT);

    // END_IF
    emit_cmd(aaa_00_root_evt, &idx, 0, OPCODE_END_IF);

    // RETURN
    emit_cmd(aaa_00_root_evt, &idx, 0, OPCODE_END_EVENT);
    emit_cmd(aaa_00_root_evt, &idx, 0, OPCODE_END_SCRIPT);

    aaa_00.initEvtCode = aaa_00_root_evt;
    printf("[Area AAA] aaa_00.initEvtCode initialise avec succes (%p)!\n", (void*)aaa_00_root_evt);
    fflush(stdout);
}
