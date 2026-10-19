#include "party/party_cloud.h"
#include "dolphin/types.h"
#include "musyx/musyx.h"
#include "party.h"
#include "mario/mario.h"
#include "mario/mario_sbr.h"
#include "mario/mario_party.h"
#include "mario/mario_cam.h"
#include "pmario_sound.h"
#include "driver/effdrv.h"
#include "system.h"
#include "memory.h"
#include <math.h>

#ifndef TARGET_PC
extern const f32 lbl_80430DC8, lbl_80430DD0, lbl_80430DE0, lbl_80430DE4;
extern const f32 lbl_80430E00, lbl_80430E08, lbl_80430E0C, lbl_80430E1C;
extern const f32 lbl_80430E30, lbl_80430E34, lbl_80430E38, lbl_80430E3C;
extern const f32 lbl_80430E40, lbl_80430E44, lbl_80430E48;
#define CLOUD_ZERO lbl_80430DC8
#define CLOUD_PI lbl_80430E38
#define CLOUD_180 lbl_80430DD0
#define CLOUD_3 lbl_80430E3C
#define CLOUD_4_25 lbl_80430E34
#define CLOUD_18 lbl_80430DE4
#define CLOUD_10 lbl_80430E1C
#define CLOUD_HALF lbl_80430E40
#define CLOUD_30 lbl_80430E44
#define CLOUD_330 lbl_80430E48
#define CLOUD_100 lbl_80430E0C
#define CLOUD_NEG1 lbl_80430E00
#define CLOUD_38 lbl_80430E30
#define CLOUD_24 lbl_80430DE0
#define CLOUD_001 lbl_80430E08
#else
#define CLOUD_ZERO 0.0f
#define CLOUD_PI 3.1415927f
#define CLOUD_180 180.0f
#define CLOUD_3 3.0f
#define CLOUD_4_25 4.25f
#define CLOUD_18 18.0f
#define CLOUD_10 10.0f
#define CLOUD_HALF 0.5f
#define CLOUD_30 30.0f
#define CLOUD_330 330.0f
#define CLOUD_100 100.0f
#define CLOUD_NEG1 -1.0f
#define CLOUD_38 38.0f
#define CLOUD_24 24.0f
#define CLOUD_001 0.01f
#endif

Vec cloud_at;

void cloudGetAt(Vec* position) {
    MarioWork* mario = marioGetPtr();
    u32 partyId = marioGetPartyId();
    PartyEntry* party = partyGetPtr(partyId);

    if (party == NULL || (party->currentMemberId == PARTY_MEMBER_FLURRIE && (party->flags & PARTY_FLAG_IS_BEING_USED) == 0)) {
        return;
    }

    if ((mario->dispFlags & (1 << 20)) == 0) {
        CameraEntry* camera = camGetPtr(CAMERA_3D);

        if(camera->stateA == 0) {
            cloud_at = *position;
        }
        else {
            *position = cloud_at;
        }
    }
}

void cloud_exit(PartyEntry* party) {
    MarioWork* mario = party->playerPtr;

    if (party->useStruct != NULL && ((PartyCloud*)party->useStruct)->unk34 != -1) {
        psndSFXOff(((PartyCloud*)party->useStruct)->unk34);
        ((PartyCloud*)party->useStruct)->unk34 = -1;
    }
    if (party->field_178 != 0) {
        effSoftDelete((EffectEntry*)party->field_178);
        party->field_178 = 0;
    }
    if (party->useStruct != NULL) {
        __memFree(HEAP_DEFAULT, party->useStruct);
        party->useStruct = NULL;
    }
    mario->dispFlags &= ~(1 << 8);
    mario->dispFlags &= ~(1 << 2);
    marioAdjustMoveDir();
    marioChgMot(MARIO_MOTION_STAY);
    mario->unk150.x = CLOUD_001;
    mario->unk138 = CLOUD_ZERO;
}

void cloudResetAt() {
    MarioWork* mario = marioGetPtr();
    u32 partyId = marioGetPartyId();
    PartyEntry* party = partyGetPtr(partyId);

    if(party != NULL && party->currentMemberId == PARTY_MEMBER_FLURRIE) {
        cloud_at.x = camGetPtr(CAMERA_3D)->cameraAt.x;
        cloud_at.y = camGetPtr(CAMERA_3D)->cameraAt.y;
        cloud_at.z = camGetPtr(CAMERA_3D)->cameraAt.z;

        marioCamZoomOffReq2(600);
    }

    mario->dispFlags &= ~(1 << 8); //kDisableMakeDispDir
    mario->dispFlags &= ~(1 << 2); //kForceFrontPose
    marioAdjustMoveDir();
    mario->unk150.x = CLOUD_001;
    mario->unk138 = CLOUD_ZERO;
}

u8 cloudGetStatus() {
    u32 partyId = marioGetPartyId();
    PartyEntry* party = partyGetPtr(partyId);
    u8 status = 2;

    if (party == NULL || (party->currentMemberId == PARTY_MEMBER_FLURRIE && (party->flags & PARTY_FLAG_IS_BEING_USED) == 0)) {
        status = 0;
        return status;
    }
    else if (party->useMotionId == 10 && party->useMotionId < 20) {
        status = 1;
    }

    return status;
}

void mot_cloud() {
    MarioWork* mario = marioGetPtr();

    if((mario->trigFlags & 0x1) != 0) {
        mario->trigFlags &= ~0x1;
        mario->flags &= ~(MARIO_FLAG_HAS_INPUT_JUMP|MARIO_FLAG_IS_STEPPING|MARIO_FLAG_IS_FALLING|MARIO_FLAG_IS_JUMPING);
        mario->multiTimer = 0;
        mario->airTimer = 0;
        mario->currSubMotionId = 0;
    }
}

f32 cloudGetBreathDir() {
    PartyEntry* party = partyGetPtr(PARTY_SLOT_PARTY);
    f32 breathDir;

    if (party == NULL) {
        return CLOUD_ZERO;
    }

    if ((party->flags & PARTY_FLAG_IS_BEING_USED) != 0 && (party->useMotionId < MARIO_MOTION_FALL)){
        breathDir = CLOUD_NEG1;
    }
    else if (party->useStruct == NULL) {
        breathDir = CLOUD_ZERO;
    }
    else {
        PartyCloud* cloud = (PartyCloud*)party->useStruct;
        breathDir = toMovedir(cloud->unk4);
    }

    return breathDir;
}

f32 cloudGetBreathDist() {
    u32 partyId = marioGetPartyId();
    PartyEntry* party = partyGetPtr(partyId);
    f32 breathDist;

    if (party == NULL) {
        breathDist = CLOUD_ZERO;
        return breathDist;
    }

    if ((party->currentMemberId != PARTY_MEMBER_FLURRIE || (party->flags & PARTY_FLAG_IS_BEING_USED) == 0) || party->useMotionId < MARIO_MOTION_FALL) {
        breathDist = CLOUD_ZERO;
    }
    else {
        PartyCloud* cloud = (PartyCloud*)party->useStruct;
        breathDist = cloud->unk8;
    }

    return breathDist;
}

f32 cloudGetBreathPower(f32 param_1, Vec* param_2) {
    PartyEntry* party = partyGetPtr(PARTY_SLOT_PARTY);
    f32 breathPower;
    f32 dist;
    f32 angle;
    f32 unk8;
    f32 val;

    breathPower = 0.0f;
    if (party == NULL)  {
        return breathPower;
    }

    val = 10.0f;
    if(
        party->currentMemberId == PARTY_MEMBER_FLURRIE
        && (party->flags & PARTY_FLAG_IS_BEING_USED) != 0
        && party->useMotionId >= MARIO_MOTION_FALL
        && ((fabs(party->position.y - param_2->y - 18.0f)) <= val)
    ) {
        PartyCloud* cloud = (PartyCloud*)party->useStruct;
        unk8 = cloud->unk8;
        if (-((0.5f * param_1) - distABf(party->position.x, party->position.z, param_2->x, param_2->z)) <= unk8) {
            PartyCloud* cloud = (PartyCloud*)party->useStruct;
            dist = toMovedir(cloud->unk4);
            angle = angleABf(party->position.x, party->position.z, param_2->x, param_2->z);
            dist = revise360(angle - dist);

            if (dist <= 30.0f || dist >= 330.0f) {
                breathPower = cos((3.1415927f * dist) / 180.0f);
                val = 100.0f;
                breathPower = val * fabsf(breathPower);
            }
        }
    }

    return breathPower;
}


s32 cloudGetHitBreathout(s32 param_1) {
    u32 partyId = marioGetPartyId();
    PartyEntry* party = partyGetPtr(partyId);
    s32 hitBreathout;

    if (party == NULL) {
        return 0;
    }

    if (
        party->currentMemberId != PARTY_MEMBER_FLURRIE
        || (party->flags & PARTY_FLAG_IS_BEING_USED) == 0
        || party->useMotionId < MARIO_MOTION_FALL
    ){
            return 0;
    }

    hitBreathout = 0;
    if (param_1 == 0) {
        hitBreathout = ((PartyCloud*)party->useStruct)->unk20;
    }
    else if (param_1 == 1) {
        hitBreathout = ((PartyCloud*)party->useStruct)->unk24;
    }
    else if (param_1 == 2) {
        hitBreathout = ((PartyCloud*)party->useStruct)->unk28;
    }
    else if (param_1 == 3) {
        hitBreathout = ((PartyCloud*)party->useStruct)->unk2C;
    }

    return hitBreathout;
}

void fn_801898E0(PartyEntry* party) {
    party->field_33 = 5;
    party->field_32 = 1;
    party->height = CLOUD_38;
    party->field_F4 = CLOUD_24;
    *(u32*)&party->field_174 = 0;
    *(u32*)&party->field_178 = 0;
    *(u32*)&party->field_17C = 0;
    *(u32*)&party->field_180 = 0;
    *(u32*)&party->field_184 = 0;
    party->useStruct = NULL;
    party->field_178 = 0;
}

void fn_80189924(PartyEntry* party) {
    if (party->field_8 & 8) {
        party->field_8 &= ~8;
        partyMoveFlyInit(party, 1);
        unk_801820d0(party);
    }
    if (party->field_184 == 0) {
        partyFlyMain(party);
        unk_8018206c(party);
        if (party->field_3B != 2) {
            party->field_8 &= ~0x40000;
        }
    }
}


void unk_8018206c(PartyEntry* party) {
	f32 temp;
    party->field_17C = revise360(party->field_17C - party->field_180);
	temp = sin((CLOUD_PI * party->field_17C) / CLOUD_180);
    party->field_7C.y = temp * CLOUD_3 + CLOUD_3;
}

void unk_801820d0(PartyEntry* party) {
    party->field_17C = CLOUD_ZERO;
    party->field_180 = CLOUD_4_25;
}

extern const char lbl_80304668[];
extern const char double_to_int_802f8010[];

void fn_80189A58(PartyEntry* party) {
    party->field_184 = 1;
    partyChgPose(party, lbl_80304668);
}

void fn_80189A88(PartyEntry* party) {
    party->field_184 = 0;
    partyChgPose(party, double_to_int_802f8010);
}
