#pragma once

#include <dolphin/types.h>

struct PartyEntry;

void kuribo_bye(struct PartyEntry* party);
void kuribo_use(struct PartyEntry* party);
void kuribo_use_post(struct PartyEntry* party);
void kuribo_init(struct PartyEntry* party);
s32 post_kurio_use(void);
