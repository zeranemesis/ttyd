#include "dolphin/types.h"
#include "dolphin/mtx.h"

typedef s32 M2C_UNK;
#define M2C_FIELD(expr, type_ptr, offset) (*(type_ptr)((s8 *)(expr) + (offset)))
M2C_UNK L_getTargetPos();
M2C_UNK N_partyPlaySfxPartyLanding1();
f32 PSVECDistance();
f32 angleABf();
void *anotherPartyGetPtr();
void *camGetPtr();
s32 chuchu_searchObject();
f32 distABf();
f32 evtGetFloat();
s32 evtGetValue();
M2C_UNK evtSetValue();
f32 fn_8009764C();
f32 fn_80097784();
s32 fn_800C2D44();
M2C_UNK fn_800C34F4();
u32 fn_800C3934();
M2C_UNK fn_800D826C();
M2C_UNK fn_800D84DC();
M2C_UNK fn_800DB9A8();
M2C_UNK fn_800DBD20();
M2C_UNK fn_800DCBEC();
u32 hitCheckFilter();
u32 hitCheckVecFilter();
s32 hitGetAttr();
M2C_UNK hitGetName();
s32 marioChkDeepSleep();
s32 marioChkSlitThrouh();
M2C_UNK marioClearSlitFloor();
s32 marioCtrlOffChk();
M2C_UNK marioGetExtraPartyId();
M2C_UNK marioGetPartyId();
void *marioGetPtr();
M2C_UNK marioGetScreenPos();
s32 marioGetSlitFloorPos();
s32 marioKeyOffChk();
M2C_UNK movePos();
s32 nokonokoGetStatus();
M2C_UNK partyChgMot();
M2C_UNK partyChgMoveMode();
M2C_UNK partyChgPaper();
M2C_UNK partyChgPose();
M2C_UNK partyChgPoseId();
M2C_UNK partyChgRunMode();
M2C_UNK partyChkFrontStep();
M2C_UNK partyChkGnd();
M2C_UNK partyChkWall();
M2C_UNK partyClearFootmark();
M2C_UNK partyClearFootmark2();
M2C_UNK partyGetAppearPos3();
M2C_UNK partyGetAppearPos4();
M2C_UNK partyGetAppearPos5();
s32 partyGetFootmarkId();
void *partyGetFootmarkPtr();
s32 partyGetFootmarkReadId();
s32 partyGetFootmarkWriteId();
M2C_UNK partyGetMoveDirSpd();
f32 **partyGetPtr();
M2C_UNK partyGetReadId();
M2C_UNK partyGetReadId2();
M2C_UNK partyGetReturnPos();
f32 partyGetTargetDir();
f32 partyGetTargetDist();
f32 partyGetTargetDist3D();
f32 partyGetTargetDistY();
f32 partyGetTargetMovespd2();
u32 partyHitCheck();
M2C_UNK partyMove();
M2C_UNK partyMoveBehindMario();
M2C_UNK partyMoveCheckWall();
M2C_UNK partyNextReadId();
M2C_UNK partyPaperLightOff();
M2C_UNK partyPaperOff();
M2C_UNK partyPaperOn();
u32 partySearchGround();
u32 partySearchHead();
M2C_UNK partySetDir();
M2C_UNK psndBGMOff();
M2C_UNK psndBGMOn();
M2C_UNK psndBGMScope();
M2C_UNK psndBGM_rate();
M2C_UNK psndENVOff();
M2C_UNK psndENVOn();
M2C_UNK psndENV_LPF();
s32 psndSFXChk();
M2C_UNK psndSFXOff();
s32 psndSFXOn();
s32 psndSFXOnEx_3D();
s32 psndSFXOn_();
s32 psndSFXOn_3D();
M2C_UNK psndSFX_dist();
M2C_UNK psndSFX_pit();
M2C_UNK psndSFX_pos();
M2C_UNK psndSFX_vol();
M2C_UNK psndSetReverb();
f32 revise360();
M2C_UNK sincosf();
s32 unk_800c8dd0();
M2C_UNK unk_800c8e74();
M2C_UNK unk_800c8f24();
M2C_UNK unk_800c9044();
M2C_UNK unk_800c9398();
M2C_UNK unk_800c93d0();
f32 unk_800c9f48();
M2C_UNK unk_800d7270();

extern const f32 float_10_8041bf40;
extern f32 lbl_8042B530;


void N_partyFollowCloseOff(void *arg0) {
    M2C_FIELD(arg0, u32 *, 8) &= 0xFFFDFFFF;
}

void N_partyFollowCloseOn(void *arg0) {
    M2C_FIELD(arg0, u32 *, 8) |= 0x20000;
}

void unk_8014c47c(void) {
    lbl_8042B530 = float_10_8041bf40;
}

s32 evt_snd_sfx_all_stop(void) {
    fn_800DCBEC();
    return 2;
}


s32 evt_snd_set_rev_mode(void *arg0) {
    psndSetReverb(*M2C_FIELD(arg0, s32 **, 0x18));
    return 2;
}

s32 evt_snd_env_lpf(void *arg0) {
    s32 temp_r31;
    void *temp_r4;

    temp_r4 = M2C_FIELD(arg0, void **, 0x18);
    temp_r31 = M2C_FIELD(temp_r4, s32 *, 0);
    psndENV_LPF(temp_r31, (u16)evtGetValue(arg0, M2C_FIELD(temp_r4, void **, 4)));
    return 2;
}

s32 evt_snd_envoff_f(void *arg0) {
    s32 temp_r31;
    s32 value;
    void *temp_r4;

    temp_r4 = M2C_FIELD(arg0, void **, 0x18);
    temp_r31 = M2C_FIELD(temp_r4, s32 *, 0);
    value = evtGetValue(arg0, M2C_FIELD(temp_r4, void **, 4));
    fn_800D826C(temp_r31, value, 0);
    return 2;
}

s32 evt_snd_envoff(void *arg0) {
    psndENVOff(*M2C_FIELD(arg0, s32 **, 0x18));
    return 2;
}

s32 evt_snd_envon_f(void *arg0) {
    s32 temp_r30;
    s32 temp_r31;
    s32 value;
    void *temp_r4;

    temp_r4 = M2C_FIELD(arg0, void **, 0x18);
    temp_r30 = M2C_FIELD(temp_r4, s32 *, 0);
    temp_r31 = M2C_FIELD(temp_r4, s32 *, 4);
    value = evtGetValue(arg0, M2C_FIELD(temp_r4, void **, 8));
    fn_800D84DC(temp_r30, temp_r31, value, 0);
    return 2;
}

s32 evt_snd_envon(void *arg0) {
    void *temp_r4;

    temp_r4 = M2C_FIELD(arg0, void **, 0x18);
    psndENVOn(M2C_FIELD(temp_r4, s32 *, 0), M2C_FIELD(temp_r4, s32 *, 4));
    return 2;
}

s32 evt_snd_sfx_dist(void *arg0) {
    s32 temp_r31_2;
    void *temp_r31;

    temp_r31 = M2C_FIELD(arg0, void **, 0x18);
    temp_r31_2 = evtGetValue(arg0, M2C_FIELD(temp_r31, void **, 0));
    psndSFX_dist(temp_r31_2, (u16)evtGetValue(arg0, M2C_FIELD(temp_r31, s32 *, 4)));
    return 2;
}

s32 evt_snd_sfx_pos(void *arg0) {
    f32 sp10;
    f32 spC;
    f32 sp8;
    u16 temp_r30;
    void *temp_r31;

    temp_r31 = M2C_FIELD(arg0, void **, 0x18);
    temp_r30 = evtGetValue(M2C_FIELD(temp_r31, void **, 0));
    sp8 = evtGetFloat(arg0, M2C_FIELD(temp_r31, s32 *, 4));
    spC = evtGetFloat(arg0, M2C_FIELD(temp_r31, s32 *, 8));
    sp10 = evtGetFloat(arg0, M2C_FIELD(temp_r31, s32 *, 0xC));
    psndSFX_pos(temp_r30, &sp8);
    return 2;
}

s32 evt_snd_sfx_vol(void *arg0) {
    s32 temp_r31_2;
    void *temp_r31;
    s32 volume;

    temp_r31 = M2C_FIELD(arg0, void **, 0x18);
    temp_r31_2 = evtGetValue(arg0, M2C_FIELD(temp_r31, void **, 0));
    volume = evtGetValue(arg0, M2C_FIELD(temp_r31, s32 *, 4));
    psndSFX_vol(temp_r31_2, (u8)volume);
    return 2;
}

s32 evt_snd_sfx_pit(void *arg0) {
    s32 temp_r31_2;
    void *temp_r31;
    s32 pitch;

    temp_r31 = M2C_FIELD(arg0, void **, 0x18);
    temp_r31_2 = evtGetValue(arg0, M2C_FIELD(temp_r31, void **, 0));
    pitch = evtGetValue(arg0, M2C_FIELD(temp_r31, s32 *, 4));
    psndSFX_pit(temp_r31_2, (s16)pitch);
    return 2;
}

s32 evt_snd_sfxchk(void *arg0) {
    void *temp_r31;

    temp_r31 = M2C_FIELD(arg0, void **, 0x18);
    evtGetValue(arg0, M2C_FIELD(temp_r31, void **, 0));
    evtSetValue(arg0, M2C_FIELD(temp_r31, s32 *, 4), psndSFXChk());
    return 2;
}

s32 evt_snd_sfxoff(void *arg0) {
    evtGetValue(arg0, *M2C_FIELD(arg0, void ***, 0x18));
    psndSFXOff();
    return 2;
}

s32 evt_snd_sfxon_3d_ex(void *arg0) {
    f32 sp10;
    f32 spC;
    f32 sp8;
    s32 temp_r30;
    s32 temp_r31_2;
    u16 temp_r27;
    u16 temp_r28;
    u16 temp_r29;
    void *temp_r31;

    temp_r31 = M2C_FIELD(arg0, void **, 0x18);
    temp_r30 = M2C_FIELD(temp_r31, s32 *, 0);
    sp8 = evtGetFloat(arg0, M2C_FIELD(temp_r31, s32 *, 4));
    spC = evtGetFloat(arg0, M2C_FIELD(temp_r31, s32 *, 8));
    sp10 = evtGetFloat(arg0, M2C_FIELD(temp_r31, s32 *, 0xC));
    temp_r29 = evtGetValue(arg0, M2C_FIELD(temp_r31, s32 *, 0x10));
    temp_r28 = evtGetValue(arg0, M2C_FIELD(temp_r31, s32 *, 0x14));
    temp_r27 = evtGetValue(arg0, M2C_FIELD(temp_r31, s32 *, 0x18));
    evtGetValue(arg0, M2C_FIELD(temp_r31, s32 *, 0x1C));
    temp_r31_2 = M2C_FIELD(temp_r31, s32 *, 0x20);
    if (temp_r31_2 == 0) {
        psndSFXOnEx_3D(temp_r30, &sp8, (u8) temp_r29, (u8) temp_r28, temp_r27);
    } else {
        evtSetValue(arg0, temp_r31_2, psndSFXOnEx_3D(temp_r30, &sp8, (u8) temp_r29, (u8) temp_r28, temp_r27));
    }
    return 2;
}

s32 evt_snd_sfxon_3d(void *arg0) {
    Vec position;
    void *temp_r31;
    s32 temp_r30;
    s32 temp_r31_2;

    temp_r31 = M2C_FIELD(arg0, void **, 0x18);
    temp_r30 = M2C_FIELD(temp_r31, s32 *, 0);
    position.x = evtGetFloat(arg0, M2C_FIELD(temp_r31, s32 *, 4));
    position.y = evtGetFloat(arg0, M2C_FIELD(temp_r31, s32 *, 8));
    position.z = evtGetFloat(arg0, M2C_FIELD(temp_r31, s32 *, 0xC));
    temp_r31_2 = M2C_FIELD(temp_r31, s32 *, 0x10);
    if ((temp_r31_2 == 0) || ((u32) (temp_r31_2 + 0x0EE70000) == 0x4D80U)) {
        psndSFXOn_3D(temp_r30, &position);
    } else {
        evtSetValue(arg0, temp_r31_2, psndSFXOn_3D(temp_r30, &position));
    }
    return 2;
}

s32 evt_snd_sfxon_(void *arg0) {
    void *temp_r31;
    s32 output;
    s32 soundId;

    temp_r31 = M2C_FIELD(arg0, void **, 0x18);
    output = M2C_FIELD(temp_r31, s32 *, 4);
    soundId = M2C_FIELD(temp_r31, s32 *, 0);
    if (output == 0) {
        psndSFXOn_(soundId);
    } else {
        evtSetValue(arg0, output, psndSFXOn_(soundId));
    }
    return 2;
}

s32 evt_snd_sfxon__(void *arg0) {
    s32 temp_r31_2;
    void *temp_r31;

    temp_r31 = M2C_FIELD(arg0, void **, 0x18);
    evtGetValue(arg0, M2C_FIELD(temp_r31, void **, 0));
    temp_r31_2 = M2C_FIELD(temp_r31, s32 *, 4);
    if (temp_r31_2 == 0) {
        psndSFXOn();
    } else {
        evtSetValue(arg0, temp_r31_2, psndSFXOn());
    }
    return 2;
}

s32 evt_snd_sfxon(void *arg0) {
    void *temp_r31;
    s32 output;
    s32 soundId;

    temp_r31 = M2C_FIELD(arg0, void **, 0x18);
    output = M2C_FIELD(temp_r31, s32 *, 4);
    soundId = M2C_FIELD(temp_r31, s32 *, 0);
    if (output == 0) {
        psndSFXOn(soundId);
    } else {
        evtSetValue(arg0, output, psndSFXOn(soundId));
    }
    return 2;
}

s32 evt_snd_bgm_scope(void *arg0) {
    s32 temp_r31;
    s32 value;
    void *temp_r4;

    temp_r4 = M2C_FIELD(arg0, void **, 0x18);
    temp_r31 = M2C_FIELD(temp_r4, s32 *, 0);
    value = evtGetValue(arg0, M2C_FIELD(temp_r4, void **, 4));
    psndBGMScope(temp_r31, value);
    return 2;
}

s32 evt_snd_bgm_freq(void *arg0) {
    s32 temp_r31;
    s32 value;
    void *temp_r4;

    temp_r4 = M2C_FIELD(arg0, void **, 0x18);
    temp_r31 = M2C_FIELD(temp_r4, s32 *, 0);
    value = evtGetValue(arg0, M2C_FIELD(temp_r4, void **, 4));
    psndBGM_rate(temp_r31, (u16)value);
    return 2;
}

s32 evt_snd_bgmoff_f(void *arg0) {
    s32 temp_r31;
    s32 value;
    void *temp_r4;

    temp_r4 = M2C_FIELD(arg0, void **, 0x18);
    temp_r31 = M2C_FIELD(temp_r4, s32 *, 0);
    value = evtGetValue(arg0, M2C_FIELD(temp_r4, void **, 4));
    fn_800DB9A8(temp_r31, value, 0);
    return 2;
}

s32 evt_snd_bgmoff(void *arg0) {
    psndBGMOff(*M2C_FIELD(arg0, s32 **, 0x18));
    return 2;
}

s32 evt_snd_bgmon_f(void *arg0) {
    s32 temp_r30;
    s32 temp_r31;
    s32 value;
    void *temp_r4;

    temp_r4 = M2C_FIELD(arg0, void **, 0x18);
    temp_r30 = M2C_FIELD(temp_r4, s32 *, 0);
    temp_r31 = M2C_FIELD(temp_r4, s32 *, 4);
    value = evtGetValue(arg0, M2C_FIELD(temp_r4, void **, 8));
    unk_800d7270(temp_r30, temp_r31, value, 0);
    return 2;
}

s32 evt_snd_bgmon(void *arg0) {
    void *temp_r4;

    temp_r4 = M2C_FIELD(arg0, void **, 0x18);
    psndBGMOn(M2C_FIELD(temp_r4, s32 *, 0), M2C_FIELD(temp_r4, s32 *, 4));
    return 2;
}
