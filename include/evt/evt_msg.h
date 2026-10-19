#pragma once

#include "evt/evt_cmd.h"
#include <dolphin/types.h>

void evt_msg_init(void);

USER_FUNC(evt_msg_print);
USER_FUNC(evt_msg_print_add);
USER_FUNC(evt_msg_print_insert);
USER_FUNC(evt_msg_close);
USER_FUNC(evt_msg_continue);