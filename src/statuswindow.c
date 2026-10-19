#include "statuswindow.h"
#include "battle/battle.h"
#include "driver/seqdrv.h"
#include "mario/mario.h"
#include "mario/mario_pouch.h"
#include "mario/mariost.h"
#include "memory.h"
#include <string.h>

extern GlobalWork* gp;

//.data
typedef struct alwaysDtEntry {
    s16 xpos;   // 0x0
    s16 ypos;   // 0x2
    s16 iconId; // 0x4
    f32 scale;  // 0x8
} alwaysDtEntry;

alwaysDtEntry alwaysDt[] = {
    { 108, 40, 464, 1.0f }, // red hp banner
    { 38, 40, 419, 1.0f },  // mario hp icon
    { 124, 34, 472, 1.0f }, // division sign
    { 288, 40, 465, 1.0f }, // orange fp banner
    { 220, 40, 433, 1.0f }, // fp icon
    { 305, 34, 472, 1.0f }, // division sign
    { 492, 40, 467, 1.0f }, // green item banner
    { 400, 40, 435, 1.0f }, // star points
    { 490, 40, 436, 1.0f }, // coin icon
    { 422, 34, 470, 1.0f }, // "x"
    { 512, 34, 470, 1.0f }, // "x"
    { 360, 76, 466, 1.0f }, // blue sp banner
    { 220, 80, 434, 0.9f }  // star
};

//.sbss
static StatusWindowWork* wp;

// local prototypes
void gaugeDisp(s32 a1, f32 x, f32 y);

void statusWinInit(void) {
    wp = __memAlloc(HEAP_DEFAULT, sizeof(StatusWindowWork));
    memset(wp, 0, sizeof(StatusWindowWork));
    wp->field_0x1C = 1.0f;
    wp->field_0x30 = -304.0f;
    wp->field_0x34 = 208.0f;
    wp->field_0x40 = -304.0f;
    wp->field_0x44 = 334.0f;
    wp->field_0x74 = 0;
    wp->field_0x78 = 0;
    wp->field_0x4 &= ~0xA;
    wp->field_0x4 |= 5;
}

// TODO: #defines?
void statusWinReInit(void) {
    wp->field_0x74 = 0;
    wp->field_0x78 = 0;
    if (wp->field_0x4 & 0x10) {
        wp->field_0x4 &= ~0x10;
        wp->field_0x4 &= ~0xA;
        wp->field_0x4 |= 5;
    } else if (wp->field_0x4 & 1) {
        wp->field_0x4 &= ~0xA;
        wp->field_0x4 |= 5;
    }
    if (wp->field_0x4 & 0x20) {
        wp->field_0x4 &= ~0x20;
        wp->field_0x4 |= 0xA;
        wp->field_0x4 &= ~5;
    } else if (wp->field_0x4 & 2) {
        wp->field_0x4 |= 0xA;
        wp->field_0x4 &= ~5;
    }
}

void statusWinMain(void) {
    MarioWork* mario;
    s32 frames;

    mario = marioGetPtr();
    if (mario->characterId != 0) {
        wp->field_0x4 = 2;
        wp->field_0xC = 1.0f;
        wp->field_0x8 = 1.0f;
        wp->field_0x18 = 0.0f;
        wp->field_0x10 = 0.0f;
    } else if (strcmp(gp->currentAreaName, "yuu")) {
        switch (seqGetSeq()) {
            case 2:
                if (marioChkCtrl() && marioChkKey() && mario->flags & 1) {
                    if (VECDistance(&wp->field_0x68, &mario->position) >= 1.0f) {
                        wp->field_0x74 = 0;
                        wp->field_0x7C = 0;
                        if (!wp->field_0x78) {
                            if ((wp->field_0x4 & 0x11) != 0) {
                                wp->field_0x4 |= 0xA;
                                wp->field_0x4 &= ~5;
                            }
                        }
                    } else {
                        wp->field_0x74++;
                        frames = 10 * gp->framerate;
                        if (wp->field_0x74 > frames) {
                            wp->field_0x74 = frames;
                            if (!wp->field_0x7C && !wp->field_0x78) {
                                if ((wp->field_0x4 & 0x11) != 0) {
                                    wp->field_0x4 &= !0xA;
                                    wp->field_0x4 |= 5;
                                    wp->field_0x4 |= 0x100;
                                }
                            }
                        }
                    }
                }
        }
    }
    // TODO: finish when more is implemented
}

void gaugeDisp(s32 a1, f32 x, f32 y) {
}

void statusWinDisp(CameraId camId, void* param) {
    alwaysDtEntry* dtEntry;
    s32 v6;
    int i;
    Mtx iconPos, iconScale;

    v6 = 1;
    if (wp->field_0x88 > 0) {
        wp->field_0x88--;
        v6 = (wp->field_0x88 % 16) > 0;
    }

    dtEntry = alwaysDt;
    for (i = 0; i < 13; i++, dtEntry++) {
        if (dtEntry->iconId == 466 && !pouchGetPtr()->mStarPowersObtained) {
            break; // don't draw star power bar
        }
        if (!v6) {
            if (wp->field_0x80 & 1 && i == 1) {
                continue;
            }
            if (wp->field_0x80 & 2 && i == 4 || wp->field_0x80 & 0x10 && i == 8 || wp->field_0x80 & 4 && i == 12) {
                continue;
            }
        }
        MTXTrans(iconPos, wp->field_0x20 + (f32)dtEntry->xpos, wp->field_0x24 - (f32)dtEntry->ypos, 0.0f);
        MTXScale(iconScale, dtEntry->scale, dtEntry->scale, dtEntry->scale);
        MTXConcat(iconPos, iconScale, iconPos);
    }
}

void statusWinDispOff(void) {
    wp->field_0x4 |= 0x1000;
}

void statusWinDispOn(void) {
    wp->field_0x4 &= ~0x1000;
}

void statusWinForceOff(void) {
    if (wp->field_0x4 & 0x10) {
        wp->field_0x4 &= ~0x10;
        wp->field_0x4 &= ~0xA;
        wp->field_0x4 |= 5;
    } else if (wp->field_0x4 & 1) {
        wp->field_0x4 &= ~0xA;
        wp->field_0x4 |= 5;
    }
    if (wp->field_0x4 & 0x20) {
        wp->field_0x4 &= ~0x20;
        wp->field_0x4 |= 0xA;
        wp->field_0x4 &= ~5;
    } else if (wp->field_0x4 & 2) {
        wp->field_0x4 |= 0xA;
        wp->field_0x4 &= ~5;
    }
}

s32 marioGetParty(void);

void statusGetValue(StatusValue* values) {
    s32 inBattle;
    s16 partyId;
    MarioWork* m;
    PouchData* pouch;
    void* bMario;
    void* bParty;
    s32 partyUnitId;

    if (!values) return;
    memset(values, 0, sizeof(StatusValue));

    inBattle = (wp && *(s32*)((u8*)wp + 0x14) != 0);
    if (!inBattle) {
        values->hp = (s16)pouchGetHP();
        values->maxHp = (s16)pouchGetMaxHP();
        values->fp = (s16)pouchGetFP();
        values->maxFp = (s16)pouchGetMaxFP();
        values->ap = (s16)pouchGetAP();
        values->maxAp = (s16)pouchGetMaxAP();
        values->starPoints = (s16)pouchGetStarPoint();
        values->coins = (s16)pouchGetCoin();

        partyId = 0;
        if (!marioBgmodeChk()) {
            partyId = (s16)marioGetParty();
        } else {
            m = marioGetPtr();
            if (m) partyId = (s16)m->characterId;
        }
        values->partyId = partyId;

        pouch = pouchGetPtr();
        if (pouch && partyId >= 0 && partyId < 8) {
            values->partyHp = (s16)pouch->partyData[partyId].currentHP;
            values->partyMaxHp = (s16)pouch->partyData[partyId].maximumHP;
        }
    } else {
        bMario = BattleGetMarioPtr(NULL);
        bParty = BattleGetPartyPtr(NULL);
        if (bMario) {
            values->hp = *(s16*)((u8*)bMario + 0x10C);
            values->maxHp = *(s16*)((u8*)bMario + 0x108);
            values->fp = *(s16*)((u8*)bMario + 0x112);
            values->maxFp = *(s16*)((u8*)bMario + 0x10E);
        }
        values->ap = (s16)pouchGetAP();
        values->maxAp = (s16)pouchGetMaxAP();
        values->starPoints = (s16)pouchGetStarPoint();
        values->coins = (s16)pouchGetCoin();

        if (bParty) {
            partyUnitId = *(s32*)((u8*)bParty + 0x8);
            values->partyId = (s16)BattleTransPartyId(partyUnitId);
            values->partyHp = *(s16*)((u8*)bParty + 0x10C);
            values->partyMaxHp = *(s16*)((u8*)bParty + 0x108);
        }
    }
}

void statusWinForceUpdate(void) {
    StatusValue val;
    statusGetValue(&val);

    *(s16*)((u8*)wp + 0x50) = val.hp;
    *(s16*)((u8*)wp + 0x52) = val.maxHp;
    *(s16*)((u8*)wp + 0x54) = val.fp;
    *(s16*)((u8*)wp + 0x56) = val.maxFp;
    *(s16*)((u8*)wp + 0x62) = val.ap;
    *(s16*)((u8*)wp + 0x64) = val.maxAp;
    *(s16*)((u8*)wp + 0x5E) = val.starPoints;
    *(s16*)((u8*)wp + 0x60) = val.coins;
    *(s16*)((u8*)wp + 0x58) = val.partyId;
    if (*(s16*)((u8*)wp + 0x58) != 0) {
        *(s16*)((u8*)wp + 0x5A) = val.partyHp;
        *(s16*)((u8*)wp + 0x5C) = val.partyMaxHp;
    }
}

void statusWinForceUpdateCoin(void) {
    StatusValue val;
    statusGetValue(&val);
    *(s16*)((u8*)wp + 0x60) = val.coins;
}

s32 valueCheck(void);

BOOL statusWinCheckUpdate(void) {
    return valueCheck() != 0;
}

BOOL statusWinCheck(void) {
    return (wp->field_0x4 & 0x11) != 0;
}

void statusWinForceCloseClear(void) {
    wp->field_0x4 &= ~0xA0;
}

void statusWinForceClose(void) {
    wp->field_0x4 &= ~5;
    wp->field_0x4 &= ~0x50;
    wp->field_0x4 |= 0xA0;
}

void statusWinForceOpen(void) {
    wp->field_0x4 &= ~0xA;
    wp->field_0x4 &= ~0xA0;
    wp->field_0x4 |= 0x50;
}

void statusWinClose(void) {
    wp->field_0x4 |= 0xA;
    wp->field_0x4 &= ~5;
}

void statusWinOpen(void) {
    wp->field_0x4 &= ~0xA;
    wp->field_0x4 |= 5;
}

void statusPartyHPBlink(s32 duration) {
    wp->field_0x80 |= 8;
    wp->field_0x88 = duration;
}

void statusMarioHPBlink(s32 duration) {
    wp->field_0x80 |= 1;
    wp->field_0x88 = duration;
}

void statusFPBlink(s32 duration) {
    wp->field_0x80 |= 2;
    wp->field_0x88 = duration;
}

void N_statusClearBlink(void) {
    wp->field_0x80 = 0;
}

void statusAPBlink(s32 duration) {
    wp->field_0x80 = 0;
    wp->field_0x80 |= 4;
    wp->field_0x88 = duration;
}

void statusGetApPos(Vec* outPos) {
    Vec pos;
    pos.x = wp->field_0x20 + (f32)alwaysDt[12].xpos;
    pos.y = wp->field_0x24 - (f32)alwaysDt[12].ypos;
    pos.z = 0.0f;
    *outPos = pos;
}
