#include "party/party_kuribo.h"
#include "evt/evt_cmd.h"
#include "mario/mario_party.h"
#include "party.h"
#include "pmario_sound.h"

//.sbss
u32 msg_ep; //unknown since it only gets written to once and never used

s32 partyGoodbyeInit(PartyEntry* party);
s32 partyGoodbyeMain(PartyEntry* party);

void kuribo_bye(PartyEntry* party) {
    if (party->field_8 & 8) {
        party->field_8 &= ~8;
        partyGoodbyeInit(party);
    }
    if (partyGoodbyeMain(party) != 0) {
        partyChgRunMode(party, 14);
    }
}

f32 PSVECDistance(const Vec* a, const Vec* b);
f32 angleABf(f32 x1, f32 z1, f32 x2, f32 z2);
f32 distABf(f32 x1, f32 z1, f32 x2, f32 z2);
BOOL fn_800C3934(PartyEntry* party, Vec* pos, f32 dist, f32 angle);
s32 partyGetAppearPos3(PartyEntry* party, Vec* outPos);
void marioStSystemLevel(s32 level);
s32 fbatNpcTalkCheck(void);
void partyChgMoveMode(PartyEntry* party, s32 mode);
void partyChgMot(PartyEntry* party, s32 motId);
void partyChkGnd(PartyEntry* party);

extern const f32 lbl_8042E000;
extern const f32 lbl_8042E004;
extern const f32 lbl_8042E008;
extern const f32 lbl_8042E00C;
extern const f32 lbl_8042E010;
extern s32 kurio_msg_evt[];

void kuribo_use(PartyEntry* party) {
    MarioWork* player;
    EventEntry* event;
    u32 check;
    Vec pos;

    player = party->playerPtr;
    if (party->field_8 & 8) {
        party->field_8 &= ~8;
        if (PSVECDistance(&player->position, &party->position) > lbl_8042E000) {
            party->useMotionId = 0;
        } else {
            pos = party->position;
            check = fn_800C3934(party, &pos, lbl_8042E004, angleABf(party->position.x, party->position.z, player->position.x, player->position.z));
            if (check) {
                if (party->field_EC < distABf(party->position.x, party->position.z, player->position.x, player->position.z)) {
                    party->useMotionId = 0;
                } else {
                    check = FALSE;
                }
            }
            if (!check) {
                partyChgPoseId(party, 1);
                party->forceMoveTargetZ = lbl_8042E008;
                party->field_24 = 100;
                party->useMotionId = 10;
            }
        }
    }

    switch (party->useMotionId) {
        case 0:
            if (!partyGetAppearPos3(party, &party->field_94)) {
                party->flags |= 0x04000000;
                party->flags |= 0x02000000;
            }
            party->field_24 = 4;
            party->field_FC = angleABf(party->position.x, party->position.z, party->field_94.x, party->field_94.z);
            party->forceMoveTargetX = party->field_FC;
            party->field_10C = player->unk1AC;
            party->field_110 = player->unk1AC;
            party->forceMoveTargetZ = distABf(party->position.x, party->position.z, party->field_94.x, party->field_94.z) / (f32)party->field_24;
            party->field_114 = lbl_8042E008;
            party->useMotionId = 1;
            break;

        case 1:
            if (!partyGetAppearPos3(party, &party->field_94)) {
                party->flags |= 0x04000000;
                party->flags |= 0x02000000;
            }
            party->position.x += (party->field_94.x - party->position.x) / (f32)party->field_24;
            party->position.y += (party->field_94.y - party->position.y) / (f32)party->field_24;
            party->position.z += (party->field_94.z - party->position.z) / (f32)party->field_24;
            if (--party->field_24 > 0) {
                return;
            }
            partyChgPoseId(party, 1);
            party->forceMoveTargetZ = lbl_8042E008;
            party->field_24 = 100;
            party->useMotionId = 10;
            break;

        case 10:
            event = evtEntryType(kurio_msg_evt, 0, 0, 0x1A);
            marioStSystemLevel(1);
            event->lwData[0] = fbatNpcTalkCheck();
            party->useMotionId = 15;
            break;

        case 20:
            party->field_24 = 5;
            party->useMotionId = 21;
            // fallthrough
        case 21:
            if (--party->field_24 <= 0) {
                partyChgRunMode(party, 2);
                partyChgMoveMode(party, 0);
                partyChgMot(party, 0);
                partyChgPoseId(party, 1);
                party->flags &= ~PARTY_FLAG_IS_BEING_USED;
                party->flags &= ~PARTY_FLAG_DISABLE_FLOORS;
                party->flags &= ~PARTY_FLAG_FORCE_FALL;
            }
            break;
    }

    partyChkGnd(party);
}

void kuribo_use_post(PartyEntry* party) {
    party->flags &= ~PARTY_FLAG_DISABLE_FLOORS;
    party->flags &= ~PARTY_FLAG_FORCE_FALL;
}

extern s32 lbl_8042B400;

void kuribo_init(PartyEntry* party) {
    party->field_33 = 1;
    party->field_32 = 0;
    party->height = lbl_8042E00C;
    party->field_F4 = lbl_8042E010;
    lbl_8042B400 = 0;
}

s32 post_kurio_use(void) {
    PartyEntry* party = partyGetPtr(marioGetPartyId());

    if (party == NULL) {
        return 2;
    }

    party->useMotionId = 20;
    marioStSystemLevel(0);
    psndClearFlag(0x80);
    party->flags &= ~PARTY_FLAG_DISABLE_FLOORS;
    party->flags &= ~PARTY_FLAG_FORCE_FALL;
    return 2;
}

BOOL christineGetStatus(void) {
    PartyEntry* party = partyGetPtr(marioGetPartyId());
    if (!party || party->currentMemberId != PARTY_MEMBER_GOOMBELLA || !(party->flags & 0x100)) {
        return FALSE;
    }
    return TRUE;
}

USER_FUNC(pre_kurio_use) {
    PartyEntry* party = partyGetPtr(marioGetPartyId());

    if (!party) {
        return EVT_RETURN_DONE;
    }

    if (party->flags2 & 0x4000000) {
        return EVT_RETURN_DONE;
    }

    psndSetFlag(0x80);
    party->flags |= 0x04000000;
    party->flags |= 0x02000000;

    if (event->lwData[0]) {
        event->lwData[0] += 8;
        return EVT_RETURN_DONE;
    }


}
