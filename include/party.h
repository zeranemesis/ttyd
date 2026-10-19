#pragma once

#include "mario/mario.h"
#include <dolphin/types.h>

#pragma enumsalwaysint off
typedef enum PartyMember {
    PARTY_MEMBER_NONE,      // 0
    PARTY_MEMBER_GOOMBELLA, // 1
    PARTY_MEMBER_KOOPS,     // 2
    PARTY_MEMBER_BOBBERY,   // 3
    PARTY_MEMBER_YOSHI,     // 4
    PARTY_MEMBER_FLURRIE,   // 5
    PARTY_MEMBER_VIVIAN,    // 6
    PARTY_MEMBER_MS_MOWZ,   // 7
#define MAIN_PARTY_MEMBER_MAX PARTY_MEMBER_MS_MOWZ + 1
    PARTY_MEMBER_EGG,                // 8
    PARTY_MEMBER_FLAVIO,             // 9
    PARTY_MEMBER_PUNIO,              // 10
    PARTY_MEMBER_FRANKLY,            // 11
    PARTY_MEMBER_CRAW,               // 12
    PARTY_MEMBER_GOOMBELLA_FOLLOWER, // 13
    PARTY_MEMBER_KOOPS_FOLLOWER,     // 14
    PARTY_MEMBER_BOBBERY_FOLLOWER,   // 15
    PARTY_MEMBER_YOSHI_FOLLOWER,     // 16
    PARTY_MEMBER_FLURRIE_FOLLOWER,   // 17
    PARTY_MEMBER_VIVIAN_FOLLOWER,    // 18
    PARTY_MEMBER_MS_MOWZ_FOLLOWER    // 19
} PartyMember;
#pragma enumsalwaysint on

#pragma enumsalwaysint off
typedef enum PartyFlags {
    PARTY_FLAG_IS_ACTIVE = (1 << 0),
    PARTY_FLAG_IS_GROUNDED = (1 << 1),
    PARTY_FLAG_IS_FOLLOWER = (1 << 2),
    PARTY_FLAG_IS_PARTNER = (1 << 3),
    PARTY_FLAG_IS_JUMPING = (1 << 4),
    PARTY_FLAG_IS_FALLING = (1 << 5),
    PARTY_FLAG_IS_STEPPING = (1 << 6),
    PARTY_FLAG_IN_SHALLOW_WATER = (1 << 7),
    PARTY_FLAG_IS_BEING_USED = (1 << 8),
    PARTY_FLAG_PAPER_MODE = (1 << 12),
    PARTY_FLAG_MOT_SLIT_FLOOR = (1 << 13),
    PARTY_FLAG_IS_RIDING_PLANE = (1 << 16),
    PARTY_FLAG_IS_RIDING_SHIP = (1 << 17),
    PARTY_FLAG_IS_BEHIND_MARIO = (1 << 19),
    PARTY_FLAG_SLEEP_ON = (1 << 22),
    PARTY_FLAG_BG_MODE = (1 << 23),
    PARTY_FLAG_DISABLE_FRONT_WALL_CHK = (1 << 24),
    PARTY_FLAG_FORCE_FALL = (1 << 25),
    PARTY_FLAG_DISABLE_FLOORS = (1 << 26),
    PARTY_FLAG_RUN_OFF = (1 << 28),
    PARTY_FLAG_CTRL_OFF = (1 << 29),
    PARTY_FLAG_CTRL_OFF_2 = (1 << 30),
    PARTY_FLAG_IS_EVT_USE = (1 << 31)
} PartyFlags;
#pragma enumsalwaysint on

#pragma enumsalwaysint off
typedef enum PartySlotId {
    PARTY_SLOT_PARTY = 0,    // 0x0
    PARTY_SLOT_FOLLOWER = 1, // 0x1
    PARTY_SLOT_NONE = -1
} PartySlotId;
#pragma enumsalwaysint on

typedef struct PartyKeyData {
    u16 button;       // 0x150
    u16 trigger;      // 0x152
    u16 repeat;       // 0x154
    u16 release;      // 0x156
    s8 stickX;        // 0x158
    s8 stickY;        // 0x159
    s8 subStickX;     // 0x15A
    s8 subStickY;     // 0x15B
    u8 trigL;         // 0x15C
    u8 trigR;         // 0x15D
    u8 field_15E;     // 0x15E
    u8 field_15F;     // 0x15F
} PartyKeyData;

typedef struct PartyEntry {
    u32 flags;                   // 0x0
    u32 flags2;                  // 0x4
    u32 field_8;                 // 0x8
    s32 paperAnimGroup;          // 0xC
    s32 field_10;                // 0x10
    s32 paperPoseId;             // 0x14
    char* field_18;              // 0x18
    u32 field_1C;                // 0x1C
    u16 field_20;                // 0x20
    u8 field_22[2];              // 0x22
    s32 field_24;                // 0x24
    s16 field_28;                // 0x28
    u8 field_2A[4];              // 0x2A
    u8 field_2E;                 // 0x2E
    s8 currentSlotId;            // 0x2F
    s8 padNo;                    // 0x30
    s8 currentMemberId;          // 0x31
    u8 field_32;                 // 0x32
    u8 field_33;                 // 0x33
    u8 runMode;                  // 0x34
    u8 prevRunMode;              // 0x35
    u8 field_36[2];              // 0x36
    u8 field_38;                 // 0x38
    u8 useMotionId;              // 0x39
    u8 field_3A;                 // 0x3A
    u8 field_3B;                 // 0x3B
    u8 field_3C[0x4F - 0x3C];    // 0x3C
    u8 field_4F;                 // 0x4F
    u8 field_50[0x58 - 0x50];    // 0x50
    Vec position;                // 0x58
    u8 field_64[0x7C - 0x64];    // 0x64
    Vec field_7C;                // 0x7C
    u8 field_88[0x94 - 0x88];    // 0x88
    Vec field_94;                // 0x94
    u8 field_A0[0xB8 - 0xA0];    // 0xA0
    Vec field_B8;                // 0xB8
    u8 field_C4[0xEC - 0xC4];    // 0xC4
    f32 field_EC;                // 0xEC
    f32 height;                  // 0xF0
    f32 field_F4;                // 0xF4
    u8 field_F8[0xFC - 0xF8];    // 0xF8
    f32 field_FC;                // 0xFC
    f32 forceMoveTargetX;        // 0x100
    f32 forceMoveTargetZ;        // 0x104
    u8 field_108[0x10C - 0x108]; // 0x108
    f32 field_10C;               // 0x10C
    f32 field_110;               // 0x110
    f32 field_114;               // 0x114
    u8 field_118[0x150 - 0x118]; // 0x118
    PartyKeyData key;            // 0x150
    MarioWork* playerPtr;        // 0x160
    s32 camId;                   // 0x164
    s32 yoshiPsndSFXId;          // 0x168
    s32 unk16C;                  // 0x16C
    void* useStruct;             // 0x170
    f32 field_174;               // 0x174
    u32 field_178;               // 0x178
    f32 field_17C;               // 0x17C
    f32 field_180;               // 0x180
    s32 field_184;               // 0x184
} PartyEntry;

PartyEntry* partyGetPtr(s32 index);
PartyEntry* anotherPartyGetPtr(s32 index);
BOOL partyPaperOn(PartyEntry* party, const char* animName);
void partyPaperOff(PartyEntry* party);
void partyPaperLightOff(PartyEntry* party);
char* partyChgPoseId(PartyEntry* party, s32 index);
void partyChgPose(PartyEntry* party, const char* pose);
void partyChgPaper(PartyEntry* party, const char* paper);
void partyCtrlOff(void);
void partyCtrlOn(void);
void partyRun(PartyEntry* party);
void partyStop(PartyEntry* party);
void partySetForceMove(PartyEntry* party, s16 duration, f32 targetX, f32 targetZ);
s32 partyEntryHello(s32 memberId);
s32 partyGoodbye(s32 index);
f32 partyGetHeight(PartyEntry* party);
void partyInit(void);
void partyUsePost(PartyEntry* party);
void partyChgRunMode(PartyEntry* party, u8 mode);
void N_partyForceChgRunMode(PartyEntry* party, u8 mode, u8 val);
void partySetCamId(PartyEntry* party, s32 cameraId);
void partyKeyOn(void);
void partyKeyOff(void);
void partyShadowOff(PartyEntry* party);
void partyInitCamId(PartyEntry* party);
s32 partyRideChk(PartyEntry* party);
s32 partyKill(s32 slotId);
s32 partyKill2(s32 slotId);
void party_ride_yoshi_force_move(void);
s32 yoshiSetColor(void);
BOOL partyDoWork(void);
void allPartyRidePlane(void);
void allPartyRideShip(void);
void L_allPartyRideOn(void);
void partyUpdateKeyData(PartyEntry* party);
void L_partyClearCont(PartyEntry* party);
void partyMoveFlyInit(PartyEntry* party, u32 param2);
void partyFlyMain(PartyEntry* party);
