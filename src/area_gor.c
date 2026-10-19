#include "evt/evt_cmd.h"
#include "manager/evtmgr.h"
#include "manager/evtmgr_cmd.h"
#include "evt/evt_bero.h"
#include "evt/evt_mario.h"
#include "evt/evt_cam.h"
#include "evt/evt_msg.h"
#include "evt/evt_sub.h"
#include "evt/evt_fade.h"
#include "data/mapdata.h"
#include <stdio.h>
#include <stdint.h>

extern MapData gor_10;
extern MapData gor_00;

static s32 gor_10_root_evt[64];
static s32 gor_10_letter_evt[64];
static s32 gor_10_voyage_evt[128];
static s32 gor_00_root_evt[64];

static void emit_cmd(s32* buf, int* idx, s32 param_count, s32 opcode) {
    buf[(*idx)++] = (param_count << 16) | (opcode & 0xFFFF);
}
static void emit_val(s32* buf, int* idx, s32 val) {
    buf[(*idx)++] = val;
}
static void emit_ptr(s32* buf, int* idx, const void* ptr) {
    buf[(*idx)++] = (s32)(uintptr_t)ptr;
}

void area_gor_init(void) {
    int idx;

    // Build gor_10_letter_evt (pro_04)
    idx = 0;
    // CALL(evt_mario_key_onoff, 0)
    emit_cmd(gor_10_letter_evt, &idx, 2, OPCODE_USER_FUNC);
    emit_ptr(gor_10_letter_evt, &idx, evt_mario_key_onoff);
    emit_val(gor_10_letter_evt, &idx, 0);

    // CALL(evt_party_stop, 0)
    emit_cmd(gor_10_letter_evt, &idx, 2, OPCODE_USER_FUNC);
    emit_ptr(gor_10_letter_evt, &idx, evt_party_stop);
    emit_val(gor_10_letter_evt, &idx, 0);

    // WAIT_FRAMES(30)
    emit_cmd(gor_10_letter_evt, &idx, 1, OPCODE_WAIT_FRAME);
    emit_val(gor_10_letter_evt, &idx, 30);

    // CALL(evt_msg_print, 0, PTR("pro_04"), 0, PTR("Princesse Peach"))
    emit_cmd(gor_10_letter_evt, &idx, 5, OPCODE_USER_FUNC);
    emit_ptr(gor_10_letter_evt, &idx, evt_msg_print);
    emit_val(gor_10_letter_evt, &idx, 0);
    emit_ptr(gor_10_letter_evt, &idx, "pro_04");
    emit_val(gor_10_letter_evt, &idx, 0);
    emit_ptr(gor_10_letter_evt, &idx, "Princesse Peach");

    // WAIT_FRAMES(30)
    emit_cmd(gor_10_letter_evt, &idx, 1, OPCODE_WAIT_FRAME);
    emit_val(gor_10_letter_evt, &idx, 30);

    // CALL(evt_fade_set_mapchange_type, 1, 17, 800, 16, 800)
    emit_cmd(gor_10_letter_evt, &idx, 6, OPCODE_USER_FUNC);
    emit_ptr(gor_10_letter_evt, &idx, evt_fade_set_mapchange_type);
    emit_val(gor_10_letter_evt, &idx, 1);
    emit_val(gor_10_letter_evt, &idx, 17);
    emit_val(gor_10_letter_evt, &idx, 800);
    emit_val(gor_10_letter_evt, &idx, 16);
    emit_val(gor_10_letter_evt, &idx, 800);

    // CALL(evt_bero_mapchange, PTR("aaa_00"), PTR("prologue2"))
    emit_cmd(gor_10_letter_evt, &idx, 3, OPCODE_USER_FUNC);
    emit_ptr(gor_10_letter_evt, &idx, evt_bero_mapchange);
    emit_ptr(gor_10_letter_evt, &idx, "aaa_00");
    emit_ptr(gor_10_letter_evt, &idx, "prologue2");

    // RETURN
    emit_cmd(gor_10_letter_evt, &idx, 0, OPCODE_END_EVENT);
    emit_cmd(gor_10_letter_evt, &idx, 0, OPCODE_END_SCRIPT);

    // Build gor_10_voyage_evt
    idx = 0;
    // CALL(evt_mario_key_onoff, 0)
    emit_cmd(gor_10_voyage_evt, &idx, 2, OPCODE_USER_FUNC);
    emit_ptr(gor_10_voyage_evt, &idx, evt_mario_key_onoff);
    emit_val(gor_10_voyage_evt, &idx, 0);

    // CALL(evt_party_stop, 0)
    emit_cmd(gor_10_voyage_evt, &idx, 2, OPCODE_USER_FUNC);
    emit_ptr(gor_10_voyage_evt, &idx, evt_party_stop);
    emit_val(gor_10_voyage_evt, &idx, 0);

    // WAIT_FRAMES(30)
    emit_cmd(gor_10_voyage_evt, &idx, 1, OPCODE_WAIT_FRAME);
    emit_val(gor_10_voyage_evt, &idx, 30);

    // CALL(evt_msg_print, 0, PTR("mac_0_012"), 0, PTR("Marin"))
    emit_cmd(gor_10_voyage_evt, &idx, 5, OPCODE_USER_FUNC);
    emit_ptr(gor_10_voyage_evt, &idx, evt_msg_print);
    emit_val(gor_10_voyage_evt, &idx, 0);
    emit_ptr(gor_10_voyage_evt, &idx, "mac_0_012");
    emit_val(gor_10_voyage_evt, &idx, 0);
    emit_ptr(gor_10_voyage_evt, &idx, "Marin");

    // WAIT_FRAMES(20)
    emit_cmd(gor_10_voyage_evt, &idx, 1, OPCODE_WAIT_FRAME);
    emit_val(gor_10_voyage_evt, &idx, 20);

    // CALL(evt_msg_print, 0, PTR("mac_0_013"), 0, PTR("Marin"))
    emit_cmd(gor_10_voyage_evt, &idx, 5, OPCODE_USER_FUNC);
    emit_ptr(gor_10_voyage_evt, &idx, evt_msg_print);
    emit_val(gor_10_voyage_evt, &idx, 0);
    emit_ptr(gor_10_voyage_evt, &idx, "mac_0_013");
    emit_val(gor_10_voyage_evt, &idx, 0);
    emit_ptr(gor_10_voyage_evt, &idx, "Marin");

    // WAIT_FRAMES(20)
    emit_cmd(gor_10_voyage_evt, &idx, 1, OPCODE_WAIT_FRAME);
    emit_val(gor_10_voyage_evt, &idx, 20);

    // CALL(evt_msg_print, 0, PTR("mac_0_013_01"), 0, PTR("Capitaine"))
    emit_cmd(gor_10_voyage_evt, &idx, 5, OPCODE_USER_FUNC);
    emit_ptr(gor_10_voyage_evt, &idx, evt_msg_print);
    emit_val(gor_10_voyage_evt, &idx, 0);
    emit_ptr(gor_10_voyage_evt, &idx, "mac_0_013_01");
    emit_val(gor_10_voyage_evt, &idx, 0);
    emit_ptr(gor_10_voyage_evt, &idx, "Capitaine");

    // WAIT_FRAMES(30)
    emit_cmd(gor_10_voyage_evt, &idx, 1, OPCODE_WAIT_FRAME);
    emit_val(gor_10_voyage_evt, &idx, 30);

    // CALL(evt_fade_set_mapchange_type, 1, 17, 800, 16, 800)
    emit_cmd(gor_10_voyage_evt, &idx, 6, OPCODE_USER_FUNC);
    emit_ptr(gor_10_voyage_evt, &idx, evt_fade_set_mapchange_type);
    emit_val(gor_10_voyage_evt, &idx, 1);
    emit_val(gor_10_voyage_evt, &idx, 17);
    emit_val(gor_10_voyage_evt, &idx, 800);
    emit_val(gor_10_voyage_evt, &idx, 16);
    emit_val(gor_10_voyage_evt, &idx, 800);

    // CALL(evt_bero_mapchange, PTR("gor_00"), PTR("marukongou"))
    emit_cmd(gor_10_voyage_evt, &idx, 3, OPCODE_USER_FUNC);
    emit_ptr(gor_10_voyage_evt, &idx, evt_bero_mapchange);
    emit_ptr(gor_10_voyage_evt, &idx, "gor_00");
    emit_ptr(gor_10_voyage_evt, &idx, "marukongou");

    // RETURN
    emit_cmd(gor_10_voyage_evt, &idx, 0, OPCODE_END_EVENT);
    emit_cmd(gor_10_voyage_evt, &idx, 0, OPCODE_END_SCRIPT);

    // Build gor_10_root_evt
    idx = 0;
    // CALL(evt_bero_get_entername, LW(0))
    emit_cmd(gor_10_root_evt, &idx, 2, OPCODE_USER_FUNC);
    emit_ptr(gor_10_root_evt, &idx, evt_bero_get_entername);
    emit_val(gor_10_root_evt, &idx, LW(0));

    // IF_STR_EQUAL(LW(0), PTR("prologue"))
    emit_cmd(gor_10_root_evt, &idx, 2, OPCODE_IF_STR_EQUAL);
    emit_val(gor_10_root_evt, &idx, LW(0));
    emit_ptr(gor_10_root_evt, &idx, "prologue");

    // CALL_CHILD(gor_10_voyage_evt)
    emit_cmd(gor_10_root_evt, &idx, 1, OPCODE_RUN_CHILD_EVENT);
    emit_ptr(gor_10_root_evt, &idx, gor_10_voyage_evt);

    // RETURN
    emit_cmd(gor_10_root_evt, &idx, 0, OPCODE_END_EVENT);

    // END_IF
    emit_cmd(gor_10_root_evt, &idx, 0, OPCODE_END_IF);

    // CALL_CHILD(gor_10_letter_evt)
    emit_cmd(gor_10_root_evt, &idx, 1, OPCODE_RUN_CHILD_EVENT);
    emit_ptr(gor_10_root_evt, &idx, gor_10_letter_evt);

    // RETURN
    emit_cmd(gor_10_root_evt, &idx, 0, OPCODE_END_EVENT);
    emit_cmd(gor_10_root_evt, &idx, 0, OPCODE_END_SCRIPT);

    gor_10.initEvtCode = gor_10_root_evt;
    printf("[Area GOR] gor_10.initEvtCode initialise avec succes (%p)!\n", (void*)gor_10_root_evt);
    fflush(stdout);

    // Build gor_00_root_evt : Active controls and spawn Mario on dock
    idx = 0;
    // CALL(evt_mario_key_onoff, 1) -> Enable controls!
    emit_cmd(gor_00_root_evt, &idx, 2, OPCODE_USER_FUNC);
    emit_ptr(gor_00_root_evt, &idx, evt_mario_key_onoff);
    emit_val(gor_00_root_evt, &idx, 1);

    // CALL(evt_mario_set_pos, FLOAT(0.0f), FLOAT(2.5f), FLOAT(-24.0f))
    emit_cmd(gor_00_root_evt, &idx, 4, OPCODE_USER_FUNC);
    emit_ptr(gor_00_root_evt, &idx, evt_mario_set_pos);
    emit_val(gor_00_root_evt, &idx, FLOAT(0.0f));
    emit_val(gor_00_root_evt, &idx, FLOAT(2.5f));
    emit_val(gor_00_root_evt, &idx, FLOAT(-24.0f));

    // CALL(evt_cam3d_evt_set, FLOAT(0.0f), FLOAT(7.0f), FLOAT(-24.0f), FLOAT(0.0f), FLOAT(19.0f), FLOAT(8.0f), FLOAT(25.0f), 0)
    emit_cmd(gor_00_root_evt, &idx, 9, OPCODE_USER_FUNC);
    emit_ptr(gor_00_root_evt, &idx, evt_cam3d_evt_set);
    emit_val(gor_00_root_evt, &idx, FLOAT(0.0f));
    emit_val(gor_00_root_evt, &idx, FLOAT(7.0f));
    emit_val(gor_00_root_evt, &idx, FLOAT(-24.0f));
    emit_val(gor_00_root_evt, &idx, FLOAT(0.0f));
    emit_val(gor_00_root_evt, &idx, FLOAT(19.0f));
    emit_val(gor_00_root_evt, &idx, FLOAT(8.0f));
    emit_val(gor_00_root_evt, &idx, FLOAT(25.0f));
    emit_val(gor_00_root_evt, &idx, 0);

    // RETURN
    emit_cmd(gor_00_root_evt, &idx, 0, OPCODE_END_EVENT);
    emit_cmd(gor_00_root_evt, &idx, 0, OPCODE_END_SCRIPT);

    gor_00.initEvtCode = gor_00_root_evt;
    printf("[Area GOR] gor_00.initEvtCode initialise avec succes (%p) - Controles Joueur prets!\n", (void*)gor_00_root_evt);
    fflush(stdout);
}
