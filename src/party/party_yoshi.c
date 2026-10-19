#include "party.h"
#include "mario/mario_party.h"
#include "evt/evt_cmd.h"
#include "driver/effdrv.h"

#ifndef TARGET_PC
extern f32 lbl_8042DC4C;
extern f32 lbl_8042DC68;
extern f32 lbl_8042DC6C;
extern s32 lbl_8042B3E4;
extern f32 lbl_8042B3E8;
s32 fn_80005A94(void);
#else
extern s32 lbl_8042B3E4;
extern f32 lbl_8042B3E8;
s32 fn_80005A94(void);
#endif

void yoshi_init(PartyEntry* party) {
    f32 fieldF4;
    f32 height;

    party->field_33 = 4;
    height = lbl_8042DC68;
    party->field_32 = 0;
    fieldF4 = lbl_8042DC6C;
    party->height = height;
    party->field_F4 = fieldF4;
    party->yoshiPsndSFXId = -1;
}

void yoshi_jumpStand(void) {
    PartyEntry* party = partyGetPtr(marioGetPartyId());

    if (party == NULL || party->currentMemberId != PARTY_MEMBER_YOSHI ||
        (party->flags & PARTY_FLAG_IS_BEING_USED) == 0) {
        return;
    }
    if (party->useMotionId >= 0x3C) {
        if (party->useMotionId <= 0x42) {
            return;
        }
    }
    party->useMotionId = 0x3C;
}

s32 evt_get_yoshi_frm(void) {
    return lbl_8042B3E4;
}

void evt_set_yoshi_spd(f32 speed) {
    lbl_8042B3E8 = speed;
}

void evt_set_yoshi_frm(void) {
    lbl_8042B3E4 = fn_80005A94();
}

s32 yoshiGetStatus(void) {
    PartyEntry* party = partyGetPtr(marioGetPartyId());
    s32 result = 5;
    u8 status;
    if (party == NULL || party->currentMemberId != PARTY_MEMBER_YOSHI ||
        (party->flags & PARTY_FLAG_IS_BEING_USED) == 0) {
        return 0;
    }
    status = party->useMotionId;
    if (status >= 0xA && status <= 0xD) {
        result = 1;
        goto done;
    }
    if (status >= 0x14 && status <= 0x16) {
        result = 2;
        goto done;
    }
    if (status >= 0x64 && status <= 0x65) {
        result = 5;
        goto done;
    }
    if (status >= 0x3C && status <= 0x41) {
        result = 3;
        goto done;
    }
    if (status >= 0x32 && status <= 0x34) {
        result = 4;
    }
done:
    return result;
}

extern void partyGoodbyeInit(PartyEntry* party);
extern s32 partyGoodbyeMain(PartyEntry* party);
extern void fn_800B57C8(PartyEntry* party);

void yoshi_bye(PartyEntry* party) {
    if (party->field_8 & 8) {
        party->field_8 &= ~8;
        partyGoodbyeInit(party);
    }
    if (party->flags & PARTY_FLAG_IS_BEING_USED) {
        if (yoshiGetStatus() == 1) {
            party->useMotionId = 0x64;
        }
        fn_800B57C8(party);
    } else if (partyGoodbyeMain(party) != 0) {
        partyChgRunMode(party, 0xE);
    }
}

typedef struct YoshiSweatWork {
    u8 pad[0x10];
    s32 timer;
    EffectEntry* effect;
} YoshiSweatWork;

void L_sweatMain(PartyEntry* party) {
    Vec* pos;
    f32 y, z, x;
    if (party->useStruct == NULL) return;
    if (((YoshiSweatWork*)party->useStruct)->effect == NULL) return;
    y = lbl_8042DC4C + party->position.y;
    z = party->position.z;
    x = party->position.x;
    pos = (Vec*)((u8*)((YoshiSweatWork*)party->useStruct)->effect->userdata + 4);
    pos->x = x;
    pos->y = y;
    pos->z = z;
    if (--((YoshiSweatWork*)party->useStruct)->timer <= 0) {
        effDelete(((YoshiSweatWork*)party->useStruct)->effect);
        ((YoshiSweatWork*)party->useStruct)->effect = NULL;
    }
}

extern void* __memAlloc(s32 heap, u32 size);
extern s32 marioGetExtraPartyId(void);
extern void party_force_ride_yoshi(PartyEntry* party);

void marioRideYoshi(void) {
    PartyEntry* party = partyGetPtr(marioGetPartyId());
    party->field_8 &= ~8;
    party->field_8 &= ~4;
    party->field_8 &= ~2;
    party->flags |= PARTY_FLAG_IS_BEING_USED;
    party->runMode = 3;
    party->useMotionId = 0x0B;
    if (party->useStruct == NULL) {
        party->useStruct = __memAlloc(0, 0x18);
        memset(party->useStruct, 0, 0x18);
        *(s32*)((u8*)party->useStruct + 0x14) = 0;
    }
    party = partyGetPtr(marioGetExtraPartyId());
    if (party != NULL) {
        party_force_ride_yoshi(party);
    }
}
