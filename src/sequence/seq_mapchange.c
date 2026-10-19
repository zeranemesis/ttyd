#include "seq/seq_mapchange.h"
#include "cam_road.h"
#include "data/mapdata.h"
#include "driver/animdrv.h"
#include "driver/bgdrv.h"
#include "driver/camdrv.h"
#include "driver/casedrv.h"
#include "driver/effdrv.h"
#include "driver/extdrv.h"
#include "driver/fadedrv.h"
#include "driver/hitdrv.h"
#include "driver/icondrv.h"
#include "driver/imgdrv.h"
#include "driver/itemdrv.h"
#include "driver/mapdrv.h"
#include "driver/mobjdrv.h"
#include "driver/msgdrv.h"
#include "driver/npcdrv.h"
#include "driver/offscreendrv.h"
#include "driver/swdrv.h"
#include "driver/windowdrv.h"
#include "eff/eff_nice.h"
#include "evt/evt_badgeshop.h"
#include "evt/evt_bero.h"
#include "evt/evt_cmd.h"
#include "evt/evt_fade.h"
#include "evt/evt_mario.h"
#include "evt/evt_seq.h"
#include "manager/evtmgr.h"
#include "manager/filemgr.h"
#include "manager/winmgr.h"
#include "mario/mario.h"
#include "mario/mario_cam.h"
#include "mario/mario_hit.h"
#include "mario/mario_party.h"
#include "mario/mariost.h"
#include "memory.h"
#include "party.h"
#include "pmario_sound.h"
#include "sound.h"
#include "statuswindow.h"
#include "system.h"
#include "win/win_main.h"
#include <dolphin/os.h>
#include <math.h>
#include <stdio.h>
#include <string.h>

extern BadgeShopWork* bdsw;
extern BOOL lbl_8042B6E8;
extern s32 battle_stage_object_sfx_id;
extern u32 comp;
extern u32 lbl_8042AEC4;

#ifndef TARGET_PC
extern GlobalWork* wp_80422810;
#define gp wp_80422810
#define _relUnLoad fn_80007740
#define seq_mapChangeMain fn_80007DF8
#endif

//.bss
u8 rel_bss[0xF10];

//.sdata
#undef USER_FUNC
#define USER_FUNC CALL
#undef RETURN
#define RETURN EVT_CMD(0, OPCODE_END_EVENT),

EVT_BEGIN(key_off)
USER_FUNC(evt_seq_wait, 2)
USER_FUNC(evt_fade_end_wait, -1)
USER_FUNC(evt_mario_key_onoff, 1)
RETURN
EVT_END()

static const char* dir_str[8] = {
    "e", "ne", "n", "nw", "w", "sw", "s", "se"
};

//.sbss
char _next_bero[0x20];
char _next_map[0x20];
char _next_area[0x20];

// local prototypes
void _relUnLoad(void);
void _load(const char* prevMapName, const char* newMapName, const char* beroName);
void _unload(const char* prevMapName, const char* newMapName, const char* beroName);

void seq_gameInit(SequenceWork* work) {
	OSTime now;
	GlobalWork* global;
	now = OSGetTime();
	lbl_8042AEC4 = (u32)now;
	global = gp;
	comp = (u32)(now >> 32);
	global->systemLevelFlags &= ~0x10;
}

void _relUnLoad(void) {
    if (gp->relocationBase) {
        ((void (*)(void))((OSModuleHeader*)gp->relocationBase)->epilog)();
        OSUnlink((OSModuleInfo*)gp->relocationBase);
        if (gp->relocationBase != gp->mapAlloc) {
            _mapFree(gp->relocationBase);
        }
        gp->relocationBase = NULL;
    }
}

void seq_mapChangeInit(SequenceWork* work) {
    strcpy(_next_area, "");
    strcpy(_next_map, "");
    strcpy(_next_bero, "");
    if (work->mapName) {
        strncpy(_next_area, work->mapName, 3);
        strncpy(_next_map, work->mapName, 0x20);
    }
    if (work->beroName) {
        strncpy(_next_bero, work->beroName, 0x20);
    }
    psndENVOff(0x200);
    psndENVOff(0x201);
    if (strcmp(gp->currentMapName, "") && strcmp(gp->currentMapName, "gor_01") && strcmp(_next_map, "gor_01")) {
        bdsw->field_0x114 |= 1;
    } else if (!strcmp(_next_map, "gor_01") && (bdsw->field_0x114 & 1)) {
        badgeShop_bargainGeneration();
        badgeShop_bottakuruGeneration();
        bdsw->field_0x114 &= 0xFFFE;
    }
}

void seq_mapChangeExit(SequenceWork* work) {
    GXResetOverflowCount();
    lbl_8042B6E8 = FALSE;
}

void seq_mapChangeMain(SequenceWork* work) {
    BOOL newarea;
    char* area_name;
    FileEntry* relFile;
    static const char* lang_dirs[7] = { "jp", "us", "ge", "fr", "sp", "it", "du" };

    newarea = strcmp(_next_area, gp->currentAreaName) != 0;
    switch (work->state) {
        case 0:
            if (!strcmp(_next_map, "sys_01")) {
                if (gp->areaFadeInType != FADE_OUT_BLACK) {
                    fadeEntry(gp->areaFadeInType, gp->areaFadeInDuration, (GXColor){ 0, 0, 0, 0xFF });
                }
                fadeEntry(FADE_OUT_WHITE, 2000, (GXColor){ 0, 0, 0, 0xFF });
            } else {
                if (!newarea) {
                    fadeEntry(gp->mapFadeInType, gp->mapFadeInDuration, (GXColor){ 0, 0, 0, 0xFF });
                } else {
                    fadeEntry(gp->areaFadeInType, gp->areaFadeInDuration, (GXColor){ 0, 0, 0, 0xFF });
                }
            }
            gp->mapFadeInType = FADE_OUT_BLACK;
            gp->mapFadeInDuration = 300;
            gp->areaFadeInType = FADE_OUT_BLACK;
            gp->areaFadeInDuration = 300;
            psndSFXAllOff();
            gp->didAreaChange = newarea;
            if (strcmp(_next_map, "tou_03")) {
                area_name = _next_area;
                if (!strcmp(_next_area, "tou")) {
                    if (seqGetSeq() == SEQ_TITLE) {
                        area_name = "tou2";
                    } else if (!strcmp(_next_map, "tou_03")) {
                        area_name = "tou2";
                    }
                }
                fileAsyncf(0, NULL, "%s/rel/%s.rel", getMarioStDvdRoot(), area_name);
            }
            work->state++;
            break;

        case 1:
            psndSFXAllOff();
            if (fadeIsFinish()) {
                _relUnLoad();
                _unload(gp->currentMapName, _next_map, _next_bero);
                if (strcmp(_next_map, "dmo_00") && strcmp(_next_map, "title")) {
                    if (!OSGetSoundMode()) {
                        SoundSetOutputMode(SND_OUTPUTMODE_MONO);
                    } else if (!gp->unk1274) {
                        SoundSetOutputMode(SND_OUTPUTMODE_STEREO);
                    } else {
                        SoundSetOutputMode(SND_OUTPUTMODE_SURROUND);
                    }
                }
                work->state++;
            }
            break;

        case 2:
            if (!strcmp(_next_map, "title")) {
                strcpy(gp->currentAreaName + 0x10, gp->currentMapName);
                strcpy(gp->currentAreaName, "");
                strcpy(gp->currentMapName, "");
                seqSetSeq(SEQ_TITLE, NULL, NULL);
                break;
            }
            area_name = _next_area;
            if (!strcmp(_next_area, "tou")) {
                if (seqGetSeq() == SEQ_TITLE || !strcmp(_next_map, "tou_03")) {
                    area_name = "tou2";
                }
            }
            if (!fileAsyncf(0, NULL, "%s/rel/%s.rel", getMarioStDvdRoot(), area_name)) {
                break;
            }
            if (!mapPreLoad()) {
                break;
            }
            area_name = _next_area;
            if (!strcmp(_next_area, "tou")) {
                if (seqGetSeq() == SEQ_TITLE || !strcmp(_next_map, "tou_03")) {
                    area_name = "tou2";
                }
            }
            relFile = fileAllocf(0, "%s/rel/%s.rel", getMarioStDvdRoot(), area_name);
            if (relFile) {
#ifndef TARGET_PC
                u32 relLength = DVDMgrGetLength(relFile->entry);
                if (!strncmp(area_name, "tst", 3) || !strncmp(area_name, "jon", 3)) {
                    gp->relocationBase = _mapAllocTail(OSRoundUp32B(relLength));
                } else {
                    gp->relocationBase = gp->mapAlloc;
                }
                memcpy(gp->relocationBase, *relFile->data, relLength);
#endif
                fileFree(relFile);
            }
            if (gp->relocationBase) {
                memset(rel_bss, 0, 0x3C4);
                OSLink((OSModuleInfo*)gp->relocationBase, rel_bss);
                if (((OSModuleHeader*)gp->relocationBase)->prolog) {
                    ((void (*)(void))((OSModuleHeader*)gp->relocationBase)->prolog)();
                }
            }
            _load(gp->currentMapName, _next_map, _next_bero);
            work->state++;
            break;

        case 3:
            if (!setupDataCheck()) {
                break;
            }
            if (psndGetFlag() & 0x1000) {
                if (psndBGMPlayTime(0) < 3000) {
                    break;
                }
                psndClearFlag(0x1000);
            }
            if (psndGetFlag() & 0x2000) {
                if (psndBGMPlayTime(1) < 3000) {
                    break;
                }
                psndClearFlag(0x2000);
            }
            setupDataBase(_next_area, _next_map);
            psndMapChange();
            marioEntry();
            fadeReset(0);
            fadeReset(1);
            {
                MapData* md = mapDataPtr(_next_map);
                if (md && md->initEvtCode) {
                    EventEntry* evt = evtEntry(md->initEvtCode, 0, 0);
                    gp->field_0x164 = evt->eventId;
#ifdef TARGET_PC
                    printf("[seq_mapChange] case 3: Map '%s' starting initEvtCode (evtId=%d)\n", _next_map, evt->eventId);
                    fflush(stdout);
#endif
                } else {
                    gp->field_0x164 = 0;
                }
            }
            work->state++;
            break;

        case 4:
            if (!gp->field_0x164) {
                camCtrlOn(CAMERA_3D);
                work->state++;
            } else if (!evtCheckID(gp->field_0x164)) {
#ifdef TARGET_PC
                printf("[seq_mapChange] case 4: initEvtCode finished for map '%s'!\n", _next_map);
                fflush(stdout);
#endif
                camCtrlOn(CAMERA_3D);
                gp->field_0x164 = 0;
                work->state++;
            }
            break;

        case 5:
            npcExecAllInitEvt();
            work->state++;
            break;

        case 6:
            if (!(camGetPtr(CAMERA_2D)->flags & 0x200) && !bero_get_BeroEXEC() && (!marioChkKey() || !marioChkCtrl())) {
                camGetPtr(CAMERA_2D)->flags |= 0x500;
            }
            if (npcWaitAllInitEvtEnd()) {
                npcExecAllReglEvt();
                work->state++;
            }
            break;

        case 7:
            if (strncmp(_next_map, "dmo", 3) && strncmp(_next_map, "dig", 3) &&
                strncmp(_next_map, "end", 3) && strncmp(_next_map, "sys", 3) &&
                strncmp(_next_map, "gor_10", 6))
            {
                fileAsyncf(4, NULL, "%s/w/%s/win.tpl", getMarioStDvdRoot(), lang_dirs[gp->language]);
                animGroupBaseAsync("p_roll", 0, NULL);
                animGroupBaseAsync("p_slit", 0, NULL);
                animGroupBaseAsync("p_jyabara", 0, NULL);
                animGroupBaseAsync("p_kaiten_h", 0, NULL);
                animGroupBaseAsync("p_b_st", 0, NULL);
                animGroupBaseAsync("OFF_d_maku_up_down", 0, NULL);
                effNiceAsync(0);
                fileAsyncf(4, NULL, "%s/battle/common/battle_common.tpl", getMarioStDvdRoot());
                fileAsyncf(4, NULL, "%s/battle/audience/audience_normal.tpl", getMarioStDvdRoot());
                fileAsyncf(4, NULL, "%s/battle/audience/audience_luigi.tpl", getMarioStDvdRoot());
            }
            if (gp->flags & 1) {
                MarioWork* mario = marioGetPtr();
                PartyEntry* party = partyGetPtr(marioGetPartyId());
                PartyEntry* extraParty = partyGetPtr(marioGetExtraPartyId());
                if (swByteGet(4) == 0) {
                    mario->position = gp->savePlayerPos;
                    evt_cam_road_reset(NULL, FALSE);
                    if (party) {
                        party->position = gp->savePlayerPos;
                        party->position.x += 30.0f * (f32)cos(camGetPtr(CAMERA_3D)->field_0x114);
                        party->position.z += 30.0f * (f32)sin(camGetPtr(CAMERA_3D)->field_0x114);
                    }
                    if (extraParty) {
                        extraParty->position = gp->savePlayerPos;
                        extraParty->position.x += -30.0f * (f32)cos(camGetPtr(CAMERA_3D)->field_0x114);
                        extraParty->position.z += 30.0f * (f32)sin(camGetPtr(CAMERA_3D)->field_0x114);
                    }
                    if (!strcmp(gp->currentMapName, "dou_05") || !strcmp(gp->currentMapName, "gor_01") ||
                        !strcmp(gp->currentMapName, "muj_10") || !strcmp(gp->currentMapName, "tik_07"))
                    {
                        mario->position.x -= 5.0f;
                        if (party) {
                            party->position = mario->position;
                            party->position.x -= 30.0f;
                            if (extraParty) {
                                extraParty->position = party->position;
                                extraParty->position.x -= 30.0f;
                            }
                        }
                    }
                    marioSetBottomlessResetPosition(mario->position.x, mario->position.y + 10.0f, mario->position.z);
                    marioKeyOff();
                    evtEntryType(key_off, 0, 0, 0);
                } else {
                    fadeEntry(28, 0, (GXColor){ 0, 0, 0, 0xFF });
                }
                statusWinForceUpdate();
                gp->flags &= ~1;
                marioStSystemLevel(0);
            }
            marioEntry();
            seqSetSeq(SEQ_GAME, NULL, NULL);
            break;
    }
}

void _load(const char* prevMapName, const char* newMapName, const char* beroName) {
    MapData* map;
    char mapFile[64];

    map = mapDataPtr(newMapName);
    strcpy(gp->beroEnterName, beroName ? beroName : "");
    strcpy(gp->currentMapName, map->mapName);
    strncpy(gp->currentAreaName, map->mapName, 3);
    if (!strcmp(newMapName, "dmo_00")) {
        mapLoad("gor_01");
    } else {
        mapLoad(map->mapName);
    }
    camCtrlOff(CAMERA_3D);
    if (gp->field_C) {
        msgLoad("gon_11", 0);
    } else {
        memset(mapFile, 0, sizeof(mapFile));
        strncpy(mapFile, map->mapName, 6);
        msgLoad(mapFile, 0);
    }
#ifdef TARGET_PC
    printf("[_load] setupDataLoad('%s')...\n", map->mapName); fflush(stdout);
#endif
    setupDataLoad(map->mapName);
#ifdef TARGET_PC
    printf("[_load] bgEntry('%s')...\n", mapFile); fflush(stdout);
#endif
    sprintf(mapFile, "./b/%s.tpl", newMapName);
    bgEntry(mapFile);
#ifdef TARGET_PC
    printf("[_load] bgEntry done. Updating player/cam pos...\n"); fflush(stdout);
#endif
    if (gp->flags & 1) {
        MarioWork* mario = marioGetPtr();
        PartyEntry* party = partyGetPtr(marioGetPartyId());
        PartyEntry* extraParty = partyGetPtr(marioGetExtraPartyId());
        mario->field_EC = mario->position = gp->savePlayerPos;
        camRoadReset();
        camShiftReset();
        if (party) {
            party->position = gp->savePlayerPos;
        }
        if (extraParty) {
            extraParty->position = gp->savePlayerPos;
        }
    } else if (beroName && beroName[0] != '\0') {
        Vec pos;
        f32 hitX, hitY, hitZ, hitDepth, nx, ny, nz;
        int i;
        hitObjGetPos(beroName, &pos);
        for (i = 0; i < 8; i++) {
            if (!strncmp(dir_str[i], beroName, strlen(dir_str[i]))) {
                f32 angle = (f32)i * 2.0f * 3.141592f * 0.125f;
                pos.x += -30.0f * (f32)cos(angle);
                pos.z += 30.0f * (f32)sin(angle);
                break;
            }
        }
        if (i < 8) {
            hitX = hitY = hitZ = 0.0f;
            hitDepth = 100000.0f;
            if (hitCheckFilter(0, &hitX, &hitY, &hitZ, &hitDepth, &nx, &ny, &nz, pos.x, pos.y + 10.0f, pos.z, 0.0f, -1.0f, 0.0f)) {
                marioGetPtr()->position = (Vec){ hitX, hitY, hitZ };
            } else {
                marioGetPtr()->position = (Vec){ 0.0f, 0.0f, 0.0f };
            }
        } else {
            marioGetPtr()->position = (Vec){ 0.0f, 0.0f, 0.0f };
        }
    } else {
        marioGetPtr()->position = (Vec){ 0.0f, 0.0f, 0.0f };
    }

    {
        MarioWork* mario = marioGetPtr();
        CameraEntry* cam = camGetPtr(CAMERA_3D);
        cam->target.x = mario->position.x;
        cam->target.y = mario->position.y + 20.0f;
        cam->target.z = mario->position.z;
        cam->cameraAt = cam->target;
        cam->cameraPos = (Vec){ cam->target.x, cam->target.y + 100.0f, cam->target.z + 400.0f };
        cam->cameraUp = (Vec){ 0.0f, 1.0f, 0.0f };
        mario->field_EC = mario->position;
        camRoadReset();
        camShiftReset();
        if (cam->flags & 4) {
            u8 dummy[32];
            camRoadMain(CAMERA_3D, dummy, cam->target.x + 1.0f, cam->target.y + 1.0f, cam->target.z + 1.0f,
                        cam->cameraPos.x + 1.0f, cam->cameraPos.y + 1.0f, cam->cameraPos.z + 1.0f, 0.0f);
        }
    }
#ifdef TARGET_PC
    printf("[_load] _load completed successfully!\n"); fflush(stdout);
#endif
}

void _unload(const char* prevMapName, const char* newMapName, const char* beroName) {
    BOOL areaChanged = FALSE;
    int i;

    if (prevMapName && newMapName && strncmp(prevMapName, newMapName, 3) != 0) {
        areaChanged = TRUE;
    }
    marioReset();
    marioInitCamId();
    mapUnLoad();
    npcReset(FALSE);
    extReset();
    mobjReset(FALSE);
    offscreenReset(FALSE);
    imgAutoRelease(FALSE);
    effAutoRelease(FALSE);
    animPoseAutoRelease(0);
    evtmgrReInit();
    caseReInit();
    itemReInit();
    iconReInit();
    windowReInit();
    winReInit();
    statusWinReInit();
    bgReInit();
    winMgrReInit();
    if (seqGetPrevSeq() != SEQ_LOAD && seqGetPrevSeq() != SEQ_TITLE &&
        strcmp(prevMapName, "sys_00") && strcmp(prevMapName, "sys_01") && strcmp(prevMapName, "dig_00"))
    {
        fadeReset(1);
        gp->flags &= ~0x20;
    }
    for (i = 0; i < 4; i++) {
        padRumbleHardOff(i);
    }
    if (areaChanged) {
        swReInit();
    }
    memClear(HEAP_MALLOC);
    memClear(HEAP_EXT);
    smartAutoFree(1);
    battle_stage_object_sfx_id = 0;
}
