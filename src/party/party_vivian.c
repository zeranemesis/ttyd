#include "party/party_vivian.h"
#include "party.h"
#include "mario/mario_party.h"
#include "mario/mariost.h"
#include "driver/animdrv.h"
#include <string.h>

#ifdef TARGET_PC
extern GlobalWork* gp;
#define wp gp
#else
extern GlobalWork* wp_80422810;
#define wp wp_80422810
#endif

f32 distABf(f32 x1, f32 z1, f32 x2, f32 z2);
f32 angleABf(f32 x1, f32 z1, f32 x2, f32 z2);
void movePos(f32 dist, f32 angle, f32* x, f32* z);
void marioPaperOff(void);
void marioChgPaper(const char* paper);
BOOL marioPaperOn(const char* paper);
s32 marioAnimeId(void);

extern s32 lbl_8042B570;
#ifndef TARGET_PC
extern const char lbl_803045FC[7];
extern const char lbl_80304604[0x1C];

extern const f32 lbl_80430D58;
extern const f32 lbl_80430D5C;
extern const f32 lbl_80430DA4;
extern const f32 lbl_80430DBC;
extern const f64 lbl_80304628;
extern const f32 lbl_80430DC0;
extern const char lbl_80430D8C[6];
#define VIVIAN_TAIL_MAP "bom_01"
#define VIVIAN_PAPER_BIBI lbl_803045FC
#define VIVIAN_PAPER_PM lbl_80304604
#define VIVIAN_POSE_NAME lbl_80430D8C
#define VIVIAN_INIT_35 lbl_80430DC0
#define VIVIAN_INIT_24 lbl_80430DA4
#define VIVIAN_INIT_10 lbl_80430D58
#define VIVIAN_MOTION_ZERO lbl_80430D5C
#define VIVIAN_TAIL_Y -1000.0f
#else
#define VIVIAN_TAIL_MAP "bom_01"
#define VIVIAN_PAPER_BIBI "p_bibi"
#define VIVIAN_PAPER_PM "PM_B_1"
#define VIVIAN_POSE_NAME "M_B_3"
#define VIVIAN_INIT_35 35.0f
#define VIVIAN_INIT_24 24.0f
#define VIVIAN_INIT_10 10.0f
#define VIVIAN_MOTION_ZERO 0.0f
#define VIVIAN_TAIL_Y -1000.0f
#endif'
void vivianShadow(PartyEntry* party);

void vivian_move(PartyEntry* party) {
    if (party->field_8 & 0x8) {
        party->field_8 &= ~0x8;
        partyMoveFlyInit(party, 1);
    }
    partyFlyMain(party);
    vivianShadow(party);
}

void vivianShadow(PartyEntry* party) {
    if (party != NULL && party->currentMemberId == PARTY_MEMBER_VIVIAN) {
        f32 dx, dy, dz;
        dx = party->position.x - party->field_B8.x;
        dy = party->position.y - party->field_B8.y;
        dz = party->position.z - party->field_B8.z;
        party->field_B8.x = party->field_B8.x + 0.1f * dx;
        party->field_B8.y = party->field_B8.y + 0.1f * dy;
        party->field_B8.z = party->field_B8.z + 0.1f * dz;
        if (distABf(party->position.x, party->position.z, party->field_B8.x, party->field_B8.z) >= 30.0f) {
            party->field_B8 = party->position;
            movePos(30.0f, angleABf(party->position.x, party->position.z, party->field_B8.x, party->field_B8.z), &party->field_B8.x, &party->field_B8.z);
        }
    }
}

BOOL N_vivianDisplayTailCheck(PartyEntry* party) {
    if (party->currentMemberId != PARTY_MEMBER_VIVIAN) {
        return FALSE;
    }
    if ((party->field_4F & 1) == 0) {
        return FALSE;
    }
    if ((party->flags2 & 0x800000) == 0) {
        return FALSE;
    }
    if (strcmp(wp->currentMapName, VIVIAN_TAIL_MAP) == 0 && party->position.y == VIVIAN_TAIL_Y) {
        return FALSE;
    }
    return TRUE;
}

void L_partyVivianTailStart(PartyEntry* party) {
    if (party->currentMemberId == PARTY_MEMBER_VIVIAN) {
        party->field_4F &= 0xFE;
    }
}

void L_partyVivianTailStop(PartyEntry* party) {
    if (party->currentMemberId == PARTY_MEMBER_VIVIAN) {
        party->field_4F |= 1;
    }
}

void vivianUnhold(PartyEntry* party) {
    if (party == NULL) {
        return;
    }
    if (party->currentMemberId != PARTY_MEMBER_VIVIAN) {
        return;
    }
    if (party->flags & PARTY_FLAG_IS_BEING_USED) {
        if (party->useMotionId == 10) {
            party->useMotionId = 20;
        }
    }
}

s32 vivianGetStatus(void) {
    PartyEntry* party = partyGetPtr(marioGetPartyId());

    if (party == NULL) {
        return 0;
    }
    if (party->currentMemberId != PARTY_MEMBER_VIVIAN || !(party->flags & PARTY_FLAG_IS_BEING_USED)) {
        return 0;
    }
    if (party->useMotionId < 10) {
        return 1;
    }
    if (party->useMotionId == 10) {
        return 2;
    }
    if (party->useMotionId >= 20 && party->useMotionId <= 27) {
        return 3;
    }
    return 4;
}

s32 lbl_8042B570;

void vivian_init(PartyEntry* party) {
    party->field_33 = 6;
    party->field_32 = 1;
    party->height = VIVIAN_INIT_35;
    party->field_F4 = VIVIAN_INIT_24;
    party->field_174 = VIVIAN_INIT_10;
    party->field_4F = 0;
    party->field_B8 = party->position;
    party->field_184 = 0;
    lbl_8042B570 = 0;
}

void mot_vivian_post(void) {
    PartyEntry* party = partyGetPtr(marioGetPartyId());
    MarioWork* mario = marioGetPtr();

    if (mario->currMotionId == 32 || mario->currMotionId == 31) {
        mario->dispFlags &= ~0x1000;
        marioPaperOff();
        mario->field_A4 = (Vec){0.0f, 0.0f, 0.0f};
        mario->flags &= ~0x400;
        if (party != NULL) {
            partyPaperOff(party);
            party->flags &= ~0x100;
            party->flags &= 0x7FFFFFFF;
            party->flags2 &= ~0x200000;
            party->field_174 = VIVIAN_INIT_10;
            party->field_7C = (Vec){0.0f, 0.0f, 0.0f};
            partyChgRunMode(party, 2);
            party->field_3B = 0;
            partyChgPoseId(party, 1);
            if (party->currentMemberId == PARTY_MEMBER_VIVIAN) {
                party->field_4F = (party->field_4F >> 1) << 1;
            }
        }
    } else {
        if (party != NULL && party->currentMemberId == PARTY_MEMBER_VIVIAN) {
            if (!(party->flags & 0x100)) {
                return;
            }
            lbl_8042B570 = 1;
        }
    }
}

void mot_vivian(void) {
    MarioWork* mario = marioGetPtr();

    if (mario->trigFlags & 1) {
        mario->trigFlags &= ~1;
        mario->flags &= ~(MARIO_FLAG_HAS_INPUT_JUMP | MARIO_FLAG_IS_STEPPING | MARIO_FLAG_IS_FALLING | MARIO_FLAG_IS_JUMPING);
        mario->multiTimer = 0;
        mario->airTimer = 0;
        mario->currSubMotionId = 0;
        mario->baseSpeed = VIVIAN_MOTION_ZERO;
        mario->field_0x1E4 = NULL;
        mario->field_0x1E0 = NULL;
        mario->hitobjPush = NULL;
        mario->field_0x1F4 = NULL;
    }
}

void N_marioForceVivianAnime(void) {
    MarioWork* mario = marioGetPtr();

    marioPaperOff();
    marioChgPaper(NULL);
    marioPaperOn(VIVIAN_PAPER_BIBI);
    marioChgPose(VIVIAN_POSE_NAME);
    marioChgPaper(VIVIAN_PAPER_PM);
    mario->poseTime = 24;
    animPoseSetLocalTime(mario->paperAnimGroupId[marioAnimeId()], (f32)mario->poseTime);
}
