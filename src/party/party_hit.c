#include "dolphin/types.h"
#define M2C_UNK s32
#define M2C_FIELD(expr, type_ptr, offset) (*(type_ptr)((u8 *)(expr) + (offset)))
#define M2C_BITWISE(type, value) ((type)(value))
#define M2C_ERROR(x) ((void)0)

f32 angleABf();              /* extern */
f64 cos();                             /* extern */
f32 fn_8009764C(void *, f32);                       /* extern */
u32 hitCheckFilter(M2C_UNK, M2C_UNK *, M2C_UNK *, M2C_UNK *, f32 *, M2C_UNK *, M2C_UNK *, M2C_UNK *, f32, f32, f32, f32, f32, f32); /* extern */
u32 hitCheckVecFilter(M2C_UNK *, s32 (*)(void *, void *));  /* extern */
s32 hitGetAttr();                                   /* extern */
void *marioGetPtr();                        /* extern */
u32 marioHitCheckVec(); /* extern */
s32 mobjHitObjPtrToPtr(void *);                     /* extern */
M2C_UNK partyChgMot(void *, M2C_UNK);               /* extern */
f32 partyGetHeight(void *);                         /* extern */
f32 revise360(f32);                                 /* extern */
f64 sin();                             /* extern */
void sincosf(f32 *, f32 *, f32);       /* extern */
                            /* extern */
f32 toMovedir(f32);                                 /* extern */
s32 yoshiGetStatus(); /* extern */
s32 chkfilter(void *, void *);                          /* static */
s32 chkfilterNokotaro(void *, void *);                  /* static */
s32 chkfilterYoshi(void *, void *);                     /* static */
extern f32 __float_nan;
extern char lbl_802D73A8;
extern f32 lbl_8042DF8C;
extern f32 lbl_8042DF90;
extern f32 lbl_8042DF94;
extern f32 lbl_8042DF98;
extern f32 lbl_8042DFA0;
extern f32 lbl_8042DFA4;
extern f32 lbl_8042DFA8;
extern f32 lbl_8042DFAC;
extern f32 lbl_8042DFB0;
extern f32 lbl_8042DFB4;
extern f32 lbl_8042DFB8;
extern f32 lbl_8042DFBC;
extern f32 lbl_8042DFC0;
extern f32 lbl_8042DFC4;
extern f32 lbl_8042DFC8;
extern f32 lbl_8042DFCC;
extern f32 lbl_8042DFD0;
extern f32 lbl_8042DFD4;
extern f32 lbl_8042DFD8;
extern f32 lbl_8042DFDC;
extern f32 lbl_8042DFE0;
extern f32 lbl_8042DFE4;
extern f32 lbl_8042DFE8;
extern f32 lbl_8042DFEC;
extern f32 lbl_8042DFF0;
extern f32 lbl_8042DFF4;
extern char *wp_80422810;

void partySearchHead(void *arg0, void *arg1, f32 *arg2) {
    f32 spC4;
    f32 spC0;
    f32 spBC;
    f32 spB8;
    f32 spB4;
    f32 spB0;
    f32 spAC;
    f32 spA8;
    f32 spA4;
    f32 spA0;
    f32 sp9C;
    f32 sp98;
    f32 sp94;
    f32 sp90;
    f32 sp8C;
    f32 sp88;
    f32 sp84;
    f32 sp80;
    f32 sp7C;
    f32 sp78;
    f32 sp74;
    M2C_UNK sp70;
    M2C_UNK sp6C;
    M2C_UNK sp68;
    M2C_UNK sp64;
    M2C_UNK sp60;
    M2C_UNK sp5C;
    f32 sp58;
    M2C_UNK sp54;
    M2C_UNK sp50;
    M2C_UNK sp4C;
    M2C_UNK sp48;
    M2C_UNK sp44;
    M2C_UNK sp40;
    f32 sp3C;
    M2C_UNK sp38;
    M2C_UNK sp34;
    M2C_UNK sp30;
    M2C_UNK sp2C;
    M2C_UNK sp28;
    M2C_UNK sp24;
    f32 sp20;
    M2C_UNK sp1C;
    M2C_UNK sp18;
    M2C_UNK sp14;
    M2C_UNK sp10;
    M2C_UNK spC;
    M2C_UNK sp8;
    f32 temp_f0;
    f32 temp_f26;
    f32 temp_f27;
    f32 temp_f27_2;
    f32 var_f28;
    f32 var_f29;
    f32 var_f31;
    u32 var_r3;

    temp_f27 = lbl_8042DFAC * M2C_FIELD(arg0, f32 *, 0xF4);
    sincosf(&sp88, &sp84, lbl_8042DFAC);
    sp80 = M2C_FIELD(arg1, f32 *, 0);
    temp_f26 = -temp_f27 * sp84;
    temp_f27_2 = temp_f27 * sp88;
    temp_f0 = M2C_FIELD(arg1, f32 *, 8);
    sp7C = M2C_FIELD(arg1, f32 *, 4);
    sp78 = temp_f0;
    var_f31 = sp80 + temp_f27_2;
    var_f29 = temp_f0 + temp_f26;
    spB8 = M2C_FIELD(&lbl_802D73A8, f32 *, 0x98);
    spB4 = M2C_FIELD(&lbl_802D73A8, f32 *, 0x94);
    spB4 = sp7C;
    var_f28 = *arg2;
    spB0 = M2C_FIELD(&lbl_802D73A8, f32 *, 0x90);
    spB8 = var_f29;
    spC0 = spB4;
    spB0 = var_f31;
    spC4 = spB8;
    spBC = spB0;
    sp74 = var_f28;
    var_r3 = hitCheckFilter(0, &sp5C, &sp60, &sp64, &sp74, &sp68, &sp6C, &sp70, spBC, lbl_8042DF98 + spC0, spC4, lbl_8042DF90, lbl_8042DFA4, lbl_8042DF90);
    if (var_r3 == 0U) {
        goto block_3;
    }
    M2C_ERROR(/* unknown instruction: cror eq, gt, eq */);
    if (var_f28 != sp74) {
        goto block_3;
    }
    var_f28 = sp74;
block_3:
    if (var_r3 != 0U) {
        goto block_15;
    }
    spA8 = M2C_FIELD(&lbl_802D73A8, f32 *, 0xA0);
    var_f31 = sp80 - temp_f27_2;
    var_f29 = sp78 - temp_f26;
    spA8 = sp7C;
    spAC = M2C_FIELD(&lbl_802D73A8, f32 *, 0xA4);
    spA4 = M2C_FIELD(&lbl_802D73A8, f32 *, 0x9C);
    spAC = var_f29;
    spC0 = spA8;
    spA4 = var_f31;
    spC4 = spAC;
    spBC = spA4;
    sp58 = var_f28;
    var_r3 = hitCheckFilter(0, &sp40, &sp44, &sp48, &sp58, &sp4C, &sp50, &sp54, spBC, lbl_8042DF98 + spC0, spC4, lbl_8042DF90, lbl_8042DFA4, lbl_8042DF90);
    if (var_r3 == 0U) {
        goto block_7;
    }
    M2C_ERROR(/* unknown instruction: cror eq, gt, eq */);
    if (var_f28 != sp58) {
        goto block_7;
    }
    var_f28 = sp58;
block_7:
    if (var_r3 != 0U) {
        goto block_15;
    }
    sp9C = M2C_FIELD(&lbl_802D73A8, f32 *, 0xAC);
    var_f31 = sp80 + temp_f26;
    var_f29 = sp78 + temp_f27_2;
    sp9C = sp7C;
    spA0 = M2C_FIELD(&lbl_802D73A8, f32 *, 0xB0);
    sp98 = M2C_FIELD(&lbl_802D73A8, f32 *, 0xA8);
    spA0 = var_f29;
    spC0 = sp9C;
    sp98 = var_f31;
    spC4 = spA0;
    spBC = sp98;
    sp3C = var_f28;
    var_r3 = hitCheckFilter(0, &sp24, &sp28, &sp2C, &sp3C, &sp30, &sp34, &sp38, spBC, lbl_8042DF98 + spC0, spC4, lbl_8042DF90, lbl_8042DFA4, lbl_8042DF90);
    if (var_r3 == 0U) {
        goto block_11;
    }
    M2C_ERROR(/* unknown instruction: cror eq, gt, eq */);
    if (var_f28 != sp3C) {
        goto block_11;
    }
    var_f28 = sp3C;
block_11:
    if (var_r3 != 0U) {
        goto block_15;
    }
    sp90 = M2C_FIELD(&lbl_802D73A8, f32 *, 0xB8);
    var_f31 = sp80 - temp_f26;
    var_f29 = sp78 - temp_f27_2;
    sp90 = sp7C;
    sp94 = M2C_FIELD(&lbl_802D73A8, f32 *, 0xBC);
    sp8C = M2C_FIELD(&lbl_802D73A8, f32 *, 0xB4);
    sp94 = var_f29;
    spC0 = sp90;
    sp8C = var_f31;
    spC4 = sp94;
    spBC = sp8C;
    sp20 = var_f28;
    var_r3 = hitCheckFilter(0, &sp8, &spC, &sp10, &sp20, &sp14, &sp18, &sp1C, spBC, lbl_8042DF98 + spC0, spC4, lbl_8042DF90, lbl_8042DFA4, lbl_8042DF90);
    if (var_r3 == 0U) {
        goto block_15;
    }
    M2C_ERROR(/* unknown instruction: cror eq, gt, eq */);
    if (var_f28 != sp20) {
        goto block_15;
    }
    var_f28 = sp20;
block_15:
    if (var_r3 == 0U) {
        goto block_17;
    }
    M2C_FIELD(arg1, f32 *, 0) = var_f31;
    M2C_FIELD(arg1, f32 *, 4) = sp7C;
    M2C_FIELD(arg1, f32 *, 8) = var_f29;
    *arg2 = var_f28;
block_17:
    if (var_r3 != 0U) {
        goto block_19;
    }
    M2C_FIELD(arg1, f32 *, 0) = var_f31;
    M2C_FIELD(arg1, f32 *, 4) = sp7C;
    M2C_FIELD(arg1, f32 *, 8) = var_f29;
    *arg2 = lbl_8042DF90;
block_19:
    return;
}

u32 fn_800C3934(void *arg0, void *arg1, f32 farg0, f32 farg1) {
    f32 spC0;
    f32 sp80;
    f32 sp84;
    f32 sp88;
    f32 spC4;
    f32 spC8;

    f32 spCC;
    f32 spB0;
    f32 spAC;
    f32 spA8;
    f32 spA4;
    f32 spA0;
    f32 sp9C;
    M2C_UNK sp90;
    f32 sp8C;
    f32 sp70;
    f32 sp6C;
    f32 sp68;
    f32 sp64;
    f32 sp60;
    f32 sp5C;
    M2C_UNK sp50;
    f32 sp4C;
    f32 sp48;
    f32 sp44;
    f32 sp40;
    f32 sp3C;
    f32 sp38;
    f32 sp34;
    f32 sp30;
    f32 sp2C;
    f32 sp28;
    f32 sp24;
    f32 sp20;
    f32 sp1C;
    f32 sp18;
    f32 sp14;
    f32 sp10;
    f32 spC;
    f32 sp8;
    f32 temp_f1;
    f32 temp_f2;
    f32 temp_f2_2;
    f32 temp_f4;
    f32 temp_f4_2;
    f32 temp_f5;
    f32 temp_f5_2;
    f32 temp_r6;
    f32 temp_r6_2;
    f32 temp_r7;
    f32 temp_r7_2;
    f32 var_f0;
    f32 var_f0_2;
    f32 var_f1;
    f32 var_f1_2;
    f32 var_f29;
    f32 var_f31;
    f32 var_f3;
    f64 temp_f1_2;
    f64 temp_f1_3;
    f64 temp_f1_4;
    s32 temp_r3;
    s32 var_r0;
    u32 var_r3;
    u32 var_r3_2;

    sincosf(&sp10, &spC, farg1);
    var_f1 = M2C_FIELD(arg0, f32 *, 0xF4);
    if (!(M2C_FIELD(arg0, s32 *, 0) & 0x1000)) {
        goto block_2;
    }
    var_f1 = lbl_8042DFA0;
block_2:
    var_f31 = (lbl_8042DF8C * var_f1) + farg0;
    var_f29 = var_f31;
    temp_f1 = partyGetHeight(arg0);
    sp48 = M2C_FIELD(arg0, f32 *, 0x5C);
    temp_r7 = M2C_FIELD(arg0, f32 *, 0x58);
    sp2C = M2C_FIELD(&lbl_802D73A8, f32 *, 0x78);
    sp48 += lbl_8042DFB0 * temp_f1;
    temp_r6 = M2C_FIELD(arg0, f32 *, 0x60);
    sp34 = M2C_FIELD(&lbl_802D73A8, f32 *, 0x80);
    sp2C = sp10;
    sp34 = spC;
    sp44 = temp_r7;
    sp4C = temp_r6;
    sp30 = M2C_FIELD(&lbl_802D73A8, f32 *, 0x7C);
    sp9C = temp_r7;
    spA0 = sp48;
    spA4 = temp_r6;
    spA8 = sp2C;
    spAC = M2C_FIELD(&lbl_802D73A8, f32 *, 0x7C);
    spB0 = sp34;
    spCC = var_f29;
    if (yoshiGetStatus(sp2C, sp48, M2C_FIELD(&lbl_802D73A8, f32 *, 0x7C), temp_r6, temp_r7, sp10) == 0) {
        goto block_4;
    }
    var_r3 = hitCheckVecFilter(&sp90, chkfilterYoshi);
    goto block_8;
block_4:
    if ((s32) M2C_FIELD(arg0, u8 *, 0x31) != 2) {
        goto block_7;
    }
    if (!(M2C_FIELD(arg0, s32 *, 0) & 0x100)) {
        goto block_7;
    }
    var_r3 = hitCheckVecFilter(&sp90, chkfilterNokotaro);
    goto block_8;
block_7:
    var_r3 = hitCheckVecFilter(&sp90, chkfilter);
block_8:
    if (var_r3 != 0U) {
        goto block_10;
    }
    goto block_14;
block_10:
    M2C_ERROR(/* unknown instruction: cror eq, gt, eq */);
    if (spCC != lbl_8042DF90) {
        goto block_12;
    }
    var_f0 = lbl_8042DF8C;
    goto block_13;
block_12:
    var_f0 = lbl_8042DFB4;
block_13:
    sp3C = spC4;
    sp40 = spC8;
    sp38 = spC0;
    var_f29 = (f32) (s32) ((lbl_8042DFB8 * spCC) + var_f0) / lbl_8042DFB8;
block_14:
    if (var_r3 == 0U) {
        goto block_16;
    }
    M2C_FIELD(arg0, f32 *, 0xEC) = var_f29;
    sp28 = sp40;
    temp_f5 = spC * farg0;
    sp20 = sp38;
    temp_f2 = sp10 * farg0;
    sp24 = sp3C;
    temp_f4 = (temp_f2 * sp20) + (temp_f5 * sp28);
    M2C_FIELD(arg1, f32 *, 0) = (f32) ((lbl_8042DF8C * -((temp_f4 * sp20) - temp_f2)) + M2C_FIELD(arg1, f32 *, 0));
    M2C_FIELD(arg1, f32 *, 8) = (f32) ((lbl_8042DF8C * -((temp_f4 * sp28) - temp_f5)) + M2C_FIELD(arg1, f32 *, 8));
    return var_r3;
block_16:
    var_f1_2 = lbl_8042DFBC;
    if ((s32) M2C_FIELD(arg0, u8 *, 0x31) != 3) {
        goto block_19;
    }
    if (!(M2C_FIELD(arg0, s32 *, 0) & 0x100)) {
        goto block_19;
    }
    var_f1_2 = lbl_8042DFA4;
block_19:
    temp_r7_2 = M2C_FIELD(arg0, f32 *, 0x58);
    sp48 = M2C_FIELD(arg0, f32 *, 0x5C);
    temp_r6_2 = M2C_FIELD(arg0, f32 *, 0x60);
    sp44 = temp_r7_2;
    sp48 += var_f1_2;
    sp4C = temp_r6_2;
    sp5C = temp_r7_2;
    sp60 = sp48;
    sp64 = temp_r6_2;
    sp68 = sp2C;
    sp6C = sp30;
    sp70 = sp34;
    sp8C = var_f31;
    if (yoshiGetStatus(sp30, sp2C, sp48, temp_r6_2, temp_r7_2) == 0) {
        goto block_21;
    }
    var_r3_2 = hitCheckVecFilter(&sp50, chkfilterYoshi);
    goto block_25;
block_21:
    if ((s32) M2C_FIELD(arg0, u8 *, 0x31) != 2) {
        goto block_24;
    }
    if (!(M2C_FIELD(arg0, s32 *, 0) & 0x100)) {
        goto block_24;
    }
    var_r3_2 = hitCheckVecFilter(&sp50, chkfilterNokotaro);
    goto block_25;
block_24:
    var_r3_2 = hitCheckVecFilter(&sp50, chkfilter);
block_25:
    if (var_r3_2 != 0U) {
        goto block_27;
    }
    goto block_31;
block_27:
    M2C_ERROR(/* unknown instruction: cror eq, gt, eq */);
    if (sp8C != lbl_8042DF90) {
        goto block_29;
    }
    var_f0_2 = lbl_8042DF8C;
    goto block_30;
block_29:
    var_f0_2 = lbl_8042DFB4;
block_30:
    sp38 = sp80;
    sp3C = sp84;
    sp40 = sp88;
    var_f31 = (f32) (s32) ((lbl_8042DFB8 * sp8C) + var_f0_2) / lbl_8042DFB8;
block_31:
    if (var_r3_2 == 0U) {
        goto block_51;
    }
    var_f3 = (sp38 * sp38) + (sp40 * sp40);
    if (!(var_f3 > lbl_8042DF90)) {
        goto block_34;
    }
    temp_f1_2 = __frsqrte(var_f3);
    temp_f1_3 = M2C_FIELD(&lbl_802D73A8, f64 *, 0x110) * temp_f1_2 * -(((f64) var_f3 * (temp_f1_2 * temp_f1_2)) - M2C_FIELD(&lbl_802D73A8, f64 *, 0x118));
    temp_f1_4 = M2C_FIELD(&lbl_802D73A8, f64 *, 0x110) * temp_f1_3 * -(((f64) var_f3 * (temp_f1_3 * temp_f1_3)) - M2C_FIELD(&lbl_802D73A8, f64 *, 0x118));
    var_f3 = (f32) ((f64) var_f3 * (M2C_FIELD(&lbl_802D73A8, f64 *, 0x110) * temp_f1_4 * -(((f64) var_f3 * (temp_f1_4 * temp_f1_4)) - M2C_FIELD(&lbl_802D73A8, f64 *, 0x118))));
    goto block_49;
block_34:
    if (!(var_f3 < (f32) M2C_FIELD(&lbl_802D73A8, f64 *, 0x120))) {
        goto block_36;
    }
    var_f3 = __float_nan;
    goto block_49;
block_36:
    sp8 = var_f3;
    temp_r3 = M2C_BITWISE(s32, sp8) & 0x7F800000;
    if (temp_r3 == 0x7F800000) {
        goto block_40;
    }
    if (temp_r3 >= 0x7F800000) {
        goto block_46;
    }
    if (temp_r3 == 0) {
        goto block_43;
    }
    goto block_46;
block_40:
    if (!(M2C_BITWISE(s32, sp8) & 0x7FFFFF)) {
        goto block_42;
    }
    var_r0 = 1;
    goto block_47;
block_42:
    var_r0 = 2;
    goto block_47;
block_43:
    if (!(M2C_BITWISE(s32, sp8) & 0x7FFFFF)) {
        goto block_45;
    }
    var_r0 = 5;
    goto block_47;
block_45:
    var_r0 = 3;
    goto block_47;
block_46:
    var_r0 = 4;
block_47:
    if (var_r0 != 1) {
        goto block_49;
    }
    var_f3 = __float_nan;
block_49:
    M2C_ERROR(/* unknown instruction: cror eq, gt, eq */);
    if (angleABf(lbl_8042DF90, lbl_8042DF90, var_f3, -sp3C) != lbl_8042DFC0) {
        goto block_51;
    }
    M2C_FIELD(arg0, f32 *, 0xEC) = var_f31;
    sp1C = sp40;
    temp_f5_2 = spC * farg0;
    sp14 = sp38;
    temp_f2_2 = sp10 * farg0;
    temp_f4_2 = (temp_f2_2 * sp14) + (temp_f5_2 * sp1C);
    sp18 = sp3C;
    M2C_FIELD(arg1, f32 *, 0) = (f32) ((lbl_8042DF8C * -((temp_f4_2 * sp14) - temp_f2_2)) + M2C_FIELD(arg1, f32 *, 0));
    M2C_FIELD(arg1, f32 *, 8) = (f32) ((lbl_8042DF8C * -((temp_f4_2 * sp1C) - temp_f5_2)) + M2C_FIELD(arg1, f32 *, 8));
    return var_r3_2;
block_51:
    M2C_FIELD(arg0, f32 *, 0xEC) = (f32) lbl_8042DFC4;
    M2C_FIELD(arg1, f32 *, 0) = (f32) ((farg0 * sp10) + M2C_FIELD(arg1, f32 *, 0));
    M2C_FIELD(arg1, f32 *, 8) = (f32) ((farg0 * spC) + M2C_FIELD(arg1, f32 *, 8));
    return 0U;
}

u32 fn_800C3E9C(void *arg0, void *arg1, f32 farg0, f32 farg1) {
    f32 sp70;
    f32 sp74;

    f32 spA4;
    f32 spA0;
    f32 sp9C;
    f32 sp98;
    s32 sp94;
    f32 sp90;
    M2C_UNK sp84;
    f32 sp80;
    s32 sp7C;
    f32 sp78;
    f32 sp6C;
    f32 sp68;
    f32 sp64;
    f32 sp60;
    f32 sp5C;
    f32 sp58;
    f32 sp54;
    f32 sp50;
    f32 sp4C;
    s32 sp48;
    f32 sp44;
    f32 sp40;
    s32 sp3C;
    f32 sp38;
    f32 sp34;
    s32 sp30;
    f32 sp2C;
    f32 sp28;
    s32 sp24;
    f32 sp20;
    f32 sp1C;
    f32 sp18;
    f32 sp14;
    f32 sp10;
    f32 spC;
    f32 sp8;
    f32 *var_r26_2;
    f32 *var_r29;
    f32 *var_r30;
    f32 temp_f1;
    f32 temp_f1_2;
    f32 temp_f1_3;
    f32 temp_f28;
    f32 temp_f2;
    f32 temp_f31;
    f32 temp_f3;
    f32 temp_f3_2;
    f32 temp_f5;
    f32 temp_f6;
    s32 var_r24;
    s32 var_r26;
    s32 var_r26_3;
    s32 var_r27;
    u32 temp_r3;
    u32 var_r28;
    u32 var_r30_2;

    sp60 = M2C_FIELD(&lbl_802D73A8, f32 *, 0x48);
    sp64 = M2C_FIELD(&lbl_802D73A8, f32 *, 0x4C);
    sp68 = M2C_FIELD(&lbl_802D73A8, f32 *, 0x50);
    sincosf(&sp1C, &sp18, farg1);
    var_r27 = 3;
    sp50 = lbl_8042DFB0 * partyGetHeight(arg0);
    sp54 = lbl_8042DF8C * partyGetHeight(arg0);
    if (M2C_FIELD(arg0, s32 *, 0) & 0x30) {
        goto block_2;
    }
    if (yoshiGetStatus() != 2) {
        goto block_3;
    }
block_2:
    sp58 = lbl_8042DFA4;
    goto block_7;
block_3:
    sp58 = lbl_8042DFBC;
    if (strcmp(wp_80422810 + 0x12C, &lbl_802D73A8 + 0x128) != 0) {
        goto block_7;
    }
    M2C_ERROR(/* unknown instruction: cror eq, gt, eq */);
    if (M2C_FIELD(arg0, f32 *, 0x5C) != lbl_8042DFC8) {
        goto block_7;
    }
    M2C_ERROR(/* unknown instruction: cror eq, lt, eq */);
    if (M2C_FIELD(arg0, f32 *, 0x58) != lbl_8042DFCC) {
        goto block_7;
    }
    var_r27 = 4;
    sp5C = lbl_8042DFA4;
block_7:
    var_r29 = &sp50;
    var_r30 = var_r29;
    var_r28 = 0U;
    temp_f31 = lbl_8042DF8C * M2C_FIELD(arg0, f32 *, 0xF4);
    var_r26 = 0;
    sp44 = M2C_FIELD(&lbl_802D73A8, f32 *, 0x54);
    temp_f28 = temp_f31 + farg0;
    sp4C = M2C_FIELD(&lbl_802D73A8, f32 *, 0x5C);
    spC = temp_f28;
    sp44 = sp1C;
    sp4C = sp18;
    sp9C = M2C_FIELD(arg1, f32 *, 0);
    spA0 = M2C_FIELD(arg1, f32 *, 4);
    sp48 = M2C_FIELD(&lbl_802D73A8, s32 *, 0x58);
    spA4 = M2C_FIELD(arg1, f32 *, 8);
    sp90 = sp44;
    sp94 = M2C_FIELD(&lbl_802D73A8, s32 *, 0x58);
    sp98 = sp4C;
    goto loop_14;
block_8:
    sp8 = temp_f28;
    temp_f1 = M2C_FIELD(arg1, f32 *, 4);
    spA0 = temp_f1 + *var_r30;
    temp_r3 = marioHitCheckVec(&sp9C, &sp90, &sp84, &sp6C, &sp8, temp_f1);
    if (temp_r3 == 0U) {
        goto block_13;
    }
    if (var_r28 == 0U) {
        goto block_12;
    }
    if (!(sp8 < spC)) {
        goto block_13;
    }
    var_r28 = temp_r3;
    spC = sp8;
    sp78 = sp6C;
    sp7C = sp70;
    sp80 = sp74;
    goto block_13;
block_12:
    var_r28 = temp_r3;
    spC = sp8;
    sp78 = sp6C;
    sp7C = sp70;
    sp80 = sp74;
block_13:
    var_r30 += 4;
    var_r26 += 1;
loop_14:
    if (var_r26 < var_r27) {
        goto block_8;
    }
    if (var_r28 == 0U) {
        goto block_17;
    }
    sp40 = sp80;
    temp_f6 = sp18 * farg0;
    sp38 = sp78;
    temp_f3 = sp1C * farg0;
    temp_f5 = (temp_f3 * sp38) + (temp_f6 * sp40);
    sp3C = sp7C;
    sp60 += lbl_8042DF8C * -((temp_f5 * sp38) - temp_f3);
    sp68 += lbl_8042DF8C * -((temp_f5 * sp40) - temp_f6);
block_17:
    var_r30_2 = 0U;
    sincosf(&sp14, &sp10, lbl_8042DFC0 + farg1);
    spC = temp_f31;
    var_r26_2 = var_r29;
    var_r24 = 0;
    sp2C = M2C_FIELD(&lbl_802D73A8, f32 *, 0x60);
    sp34 = M2C_FIELD(&lbl_802D73A8, f32 *, 0x68);
    sp2C = sp14;
    sp34 = sp10;
    sp9C = M2C_FIELD(arg1, f32 *, 0);
    spA0 = M2C_FIELD(arg1, f32 *, 4);
    sp30 = M2C_FIELD(&lbl_802D73A8, s32 *, 0x64);
    spA4 = M2C_FIELD(arg1, f32 *, 8);
    sp90 = sp2C;
    sp94 = M2C_FIELD(&lbl_802D73A8, s32 *, 0x64);
    sp98 = sp34;
    goto loop_20;
block_18:
    spC = temp_f31;
    temp_f1_2 = M2C_FIELD(arg1, f32 *, 4);
    spA0 = temp_f1_2 + *var_r26_2;
    var_r30_2 = marioHitCheckVec(&sp9C, &sp90, NULL, NULL, &spC, temp_f1_2);
    if (var_r30_2 != 0U) {
        goto block_21;
    }
    var_r26_2 += 4;
    var_r24 += 1;
loop_20:
    if (var_r24 < var_r27) {
        goto block_18;
    }
block_21:
    if (var_r30_2 == 0U) {
        goto block_25;
    }
    var_r30_2 = 0U;
    sincosf(&sp14, &sp10, farg1 - lbl_8042DFC0);
    var_r26_3 = 0;
    goto loop_24;
block_23:
    spC = temp_f31;
    sp20 = M2C_FIELD(&lbl_802D73A8, f32 *, 0x6C);
    sp28 = M2C_FIELD(&lbl_802D73A8, f32 *, 0x74);
    temp_f3_2 = M2C_FIELD(arg1, f32 *, 4);
    temp_f2 = *var_r29;
    sp20 = sp14;
    temp_f1_3 = temp_f3_2 + temp_f2;
    sp28 = sp10;
    spA0 = temp_f1_3;
    sp24 = M2C_FIELD(&lbl_802D73A8, s32 *, 0x70);
    sp90 = sp20;
    sp94 = M2C_FIELD(&lbl_802D73A8, s32 *, 0x70);
    sp98 = sp28;
    var_r30_2 = marioHitCheckVec(&sp9C, &sp90, NULL, NULL, &spC, sp20, M2C_FIELD(&lbl_802D73A8, s32 *, 0x70), temp_f1_3, temp_f2, temp_f3_2);
    var_r26_3 += 1;
    var_r29 += 4;
loop_24:
    if (var_r26_3 < var_r27) {
        goto block_23;
    }
block_25:
    if (var_r28 == 0U) {
        goto block_29;
    }
    if (var_r30_2 != 0U) {
        goto block_28;
    }
    M2C_FIELD(arg1, f32 *, 0) = (f32) (M2C_FIELD(arg1, f32 *, 0) + sp60);
    M2C_FIELD(arg1, f32 *, 8) = (f32) (M2C_FIELD(arg1, f32 *, 8) + sp68);
block_28:
    return var_r28;
block_29:
    if (var_r30_2 != 0U) {
        goto block_31;
    }
    M2C_FIELD(arg1, f32 *, 0) = (f32) ((farg0 * sp1C) + M2C_FIELD(arg1, f32 *, 0));
    M2C_FIELD(arg1, f32 *, 8) = (f32) ((farg0 * sp18) + M2C_FIELD(arg1, f32 *, 8));
block_31:
    return 0U;
}

u32 partyChkFrontStep(void *arg0, f32 *arg1, f32 *arg2, f32 *arg3, f32 farg0) {
    f32 saved_reg_f28;
    f32 saved_reg_f29;
    f32 saved_reg_f30;

    f32 sp54;
    f32 sp58;
    f32 sp5C;
    f32 sp60;
    f32 sp64;
    f32 sp68;

    s32 sp9C;
    s32 sp98;
    f32 sp6C;
    s32 sp50;
    f32 sp4C;
    f32 sp48;
    f32 sp44;
    f32 sp40;
    f32 sp3C;
    M2C_UNK sp30;
    f32 sp2C;
    f32 sp28;
    f32 sp24;
    f32 sp20;
    f32 sp1C;
    f32 sp18;
    f32 sp14;
    f32 sp10;
    f32 spC;
    f32 sp8;
    f32 temp_f1;
    f32 temp_f1_2;
    f32 temp_f23;
    f32 temp_f23_2;
    f32 temp_f24;
    f32 temp_f24_2;
    f32 temp_f25;
    f32 temp_f25_2;
    f32 temp_f26;
    f32 temp_f26_2;
    f32 temp_f2;
    f32 temp_f2_2;
    f32 temp_f31;
    f32 temp_f3;
    f32 var_f0;
    f32 var_f0_2;
    f32 var_f0_3;
    f32 var_f1;
    f32 var_f28;
    f32 var_f29;
    f32 var_f30;
    f32 var_f3;
    f64 temp_f1_3;
    f64 temp_f1_4;
    f64 temp_f1_5;
    s32 temp_r0;
    s32 temp_r3;
    s32 var_r0;
    u32 var_r3;

    var_f28 = saved_reg_f28;
    var_f29 = saved_reg_f29;
    var_f30 = saved_reg_f30;
    temp_f24 = farg0 + lbl_8042DFD0;
    M2C_FIELD(arg0, void **, 0xC4) = (void *) M2C_FIELD(&lbl_802D73A8, void **, 0x18);
    M2C_FIELD(arg0, f32 *, 0xC8) = (f32) M2C_FIELD(&lbl_802D73A8, f32 *, 0x1C);
    M2C_FIELD(arg0, f32 *, 0xCC) = (f32) M2C_FIELD(&lbl_802D73A8, f32 *, 0x20);
    M2C_FIELD(arg0, f32 *, 0xE0) = (f32) lbl_8042DF90;
    temp_f31 = partyGetHeight(M2C_FIELD(&lbl_802D73A8, void **, 0x18));
    *arg1 = M2C_FIELD(arg0, f32 *, 0x5C);
    *arg2 = lbl_8042DFC4;
    if (lbl_8042DF90 == M2C_FIELD(arg0, f32 *, 0x104)) {
        goto block_2;
    }
    var_f1 = M2C_FIELD(arg0, f32 *, 0x100);
    goto block_3;
block_2:
    var_f1 = toMovedir(M2C_FIELD(arg0, f32 *, 0x10C));
block_3:
    temp_f2 = M2C_FIELD(arg0, f32 *, 0xF4);
    temp_f25 = (lbl_8042DFD8 * revise360((f32) (s32) (lbl_8042DF8C + var_f1))) / lbl_8042DF94;
    temp_f23 = lbl_8042DFD4 * temp_f2;
    temp_f26 = (f32) sin(temp_f25, temp_f2, lbl_8042DFD4);
    temp_f25_2 = M2C_FIELD(arg0, f32 *, 0x5C);
    temp_f26_2 = M2C_FIELD(arg0, f32 *, 0x58);
    temp_f2_2 = temp_f25_2 + temp_f31;
    temp_f1 = -(f32) cos(temp_f25);
    temp_f3 = temp_f26_2 + (temp_f23 * temp_f26);
    sp18 = M2C_FIELD(&lbl_802D73A8, f32 *, 0x24);
    temp_f24_2 = M2C_FIELD(arg0, f32 *, 0x60);
    sp1C = M2C_FIELD(&lbl_802D73A8, f32 *, 0x28);
    temp_f1_2 = temp_f24_2 + (temp_f23 * temp_f1);
    sp20 = M2C_FIELD(&lbl_802D73A8, f32 *, 0x2C);
    sp18 = temp_f3;
    sp1C = temp_f2_2;
    sp20 = temp_f1_2;
    sp3C = sp18;
    sp40 = sp1C;
    sp44 = sp20;
    sp48 = M2C_FIELD(&lbl_802D73A8, f32 *, 0x30);
    sp4C = M2C_FIELD(&lbl_802D73A8, f32 *, 0x34);
    sp50 = M2C_FIELD(&lbl_802D73A8, s32 *, 0x38);
    sp6C = temp_f31 + temp_f24;
    if (yoshiGetStatus(M2C_FIELD(&lbl_802D73A8, f32 *, 0x34), M2C_FIELD(&lbl_802D73A8, f32 *, 0x30), sp20, sp1C, sp18, temp_f1_2, temp_f2_2, temp_f3) == 0) {
        goto block_5;
    }
    var_r3 = hitCheckVecFilter(&sp30, chkfilterYoshi);
    goto block_9;
block_5:
    if ((s32) M2C_FIELD(arg0, u8 *, 0x31) != 2) {
        goto block_8;
    }
    if (!(M2C_FIELD(arg0, s32 *, 0) & 0x100)) {
        goto block_8;
    }
    var_r3 = hitCheckVecFilter(&sp30, chkfilterNokotaro);
    goto block_9;
block_8:
    var_r3 = hitCheckVecFilter(&sp30, chkfilter);
block_9:
    if (var_r3 != 0U) {
        goto block_11;
    }
    goto block_21;
block_11:
    M2C_ERROR(/* unknown instruction: cror eq, gt, eq */);
    if (sp54 != lbl_8042DF90) {
        goto block_13;
    }
    var_f0 = lbl_8042DF8C;
    goto block_14;
block_13:
    var_f0 = lbl_8042DFB4;
block_14:
    var_f30 = (f32) (s32) ((lbl_8042DFB8 * sp54) + var_f0) / lbl_8042DFB8;
    M2C_ERROR(/* unknown instruction: cror eq, gt, eq */);
    if (sp58 != lbl_8042DF90) {
        goto block_16;
    }
    var_f0_2 = lbl_8042DF8C;
    goto block_17;
block_16:
    var_f0_2 = lbl_8042DFB4;
block_17:
    var_f29 = (f32) (s32) ((lbl_8042DFB8 * sp58) + var_f0_2) / lbl_8042DFB8;
    M2C_ERROR(/* unknown instruction: cror eq, gt, eq */);
    if (sp5C != lbl_8042DF90) {
        goto block_19;
    }
    var_f0_3 = lbl_8042DF8C;
    goto block_20;
block_19:
    var_f0_3 = lbl_8042DFB4;
block_20:
    sp98 = 0x43300000;
    sp24 = sp60;
    temp_r0 = (s32) ((lbl_8042DFB8 * sp5C) + var_f0_3);
    sp28 = sp64;
    sp9C = temp_r0 ^ 0x80000000;
    sp2C = sp68;
    var_f28 = (f32) temp_r0 / lbl_8042DFB8;
block_21:
    if (var_r3 == 0U) {
        goto block_43;
    }
    sp10 = sp28;
    sp14 = sp2C;
    temp_f23_2 = lbl_8042DFA8 * sp10;
    spC = sp24;
    M2C_FIELD(arg0, f32 *, 0xC4) = (f32) (s32) (angleABf(sp28, lbl_8042DF90, lbl_8042DF90, lbl_8042DFA8 * sp14, temp_f23_2) - lbl_8042DF94);
    M2C_FIELD(arg0, f32 *, 0xC8) = (f32) lbl_8042DF90;
    var_f3 = (spC * spC) + (sp14 * sp14);
    M2C_FIELD(arg0, f32 *, 0xCC) = (f32) (s32) (angleABf(lbl_8042DF90, lbl_8042DF90, lbl_8042DFA8 * spC, temp_f23_2) - lbl_8042DF94);
    if (!(var_f3 > lbl_8042DF90)) {
        goto block_24;
    }
    temp_f1_3 = __frsqrte(var_f3);
    temp_f1_4 = M2C_FIELD(&lbl_802D73A8, f64 *, 0x110) * temp_f1_3 * -(((f64) var_f3 * (temp_f1_3 * temp_f1_3)) - M2C_FIELD(&lbl_802D73A8, f64 *, 0x118));
    temp_f1_5 = M2C_FIELD(&lbl_802D73A8, f64 *, 0x110) * temp_f1_4 * -(((f64) var_f3 * (temp_f1_4 * temp_f1_4)) - M2C_FIELD(&lbl_802D73A8, f64 *, 0x118));
    var_f3 = (f32) ((f64) var_f3 * (M2C_FIELD(&lbl_802D73A8, f64 *, 0x110) * temp_f1_5 * -(((f64) var_f3 * (temp_f1_5 * temp_f1_5)) - M2C_FIELD(&lbl_802D73A8, f64 *, 0x118))));
    goto block_39;
block_24:
    if (!(var_f3 < (f32) M2C_FIELD(&lbl_802D73A8, f64 *, 0x120))) {
        goto block_26;
    }
    var_f3 = __float_nan;
    goto block_39;
block_26:
    sp8 = var_f3;
    temp_r3 = M2C_BITWISE(s32, sp8) & 0x7F800000;
    if (temp_r3 == 0x7F800000) {
        goto block_30;
    }
    if (temp_r3 >= 0x7F800000) {
        goto block_36;
    }
    if (temp_r3 == 0) {
        goto block_33;
    }
    goto block_36;
block_30:
    if (!(M2C_BITWISE(s32, sp8) & 0x7FFFFF)) {
        goto block_32;
    }
    var_r0 = 1;
    goto block_37;
block_32:
    var_r0 = 2;
    goto block_37;
block_33:
    if (!(M2C_BITWISE(s32, sp8) & 0x7FFFFF)) {
        goto block_35;
    }
    var_r0 = 5;
    goto block_37;
block_35:
    var_r0 = 3;
    goto block_37;
block_36:
    var_r0 = 4;
block_37:
    if (var_r0 != 1) {
        goto block_39;
    }
    var_f3 = __float_nan;
block_39:
    M2C_FIELD(arg0, f32 *, 0xE0) = angleABf(lbl_8042DF90, lbl_8042DF90, var_f3, -sp10);
    if (!(var_f29 < temp_f25_2)) {
        goto block_42;
    }
    M2C_ERROR(/* unknown instruction: cror eq, gt, eq */);
    if (fabs(var_f29 - temp_f25_2) != lbl_8042DFDC) {
        goto block_42;
    }
    *arg2 = temp_f25_2 - var_f29;
    *arg3 = angleABf(temp_f26_2, temp_f24_2, var_f30, var_f28);
    goto block_43;
block_42:
    *arg1 = var_f29;
    *arg2 = lbl_8042DF90;
    *arg3 = angleABf(temp_f26_2, temp_f24_2, sp24, sp2C);
    return var_r3;
block_43:
    return 0U;
}

u32 partySearchGround(void *arg0, f32 farg0, f32 farg1) {
    f32 saved_reg_f29;
    f32 saved_reg_f30;
    f32 saved_reg_f31;

    f32 sp74;
    f32 sp7C;
    f32 sp80;
    f32 sp84;

    s32 sp9C;
    s32 sp98;
    f32 sp88;
    s32 sp6C;
    s32 sp68;
    s32 sp64;
    f32 sp60;
    f32 sp5C;
    f32 sp58;
    M2C_UNK sp4C;
    f32 sp48;
    f32 sp44;
    f32 sp40;
    s32 sp3C;
    s32 sp38;
    s32 sp34;
    f32 sp30;
    f32 sp2C;
    f32 sp28;
    f32 sp24;
    f32 sp20;
    f32 sp1C;
    f32 sp18;
    f32 sp14;
    f32 sp10;
    f32 spC;
    f32 sp8;
    f32 temp_f0;
    f32 temp_f19;
    f32 temp_f19_2;
    f32 temp_f1;
    f32 temp_f1_8;
    f32 temp_f20;
    f32 temp_f21;
    f32 temp_f22;
    f32 temp_f22_2;
    f32 temp_f22_3;
    f32 temp_f23;
    f32 temp_f24;
    f32 temp_f27;
    f32 temp_f28;
    f32 temp_f3;
    f32 var_f0;
    f32 var_f1;
    f32 var_f20;
    f32 var_f25;
    f32 var_f29;
    f32 var_f30;
    f32 var_f31;
    f32 var_f3;
    f32 var_f3_2;
    f64 temp_f1_2;
    f64 temp_f1_3;
    f64 temp_f1_4;
    f64 temp_f1_5;
    f64 temp_f1_6;
    f64 temp_f1_7;
    s32 temp_r0;
    s32 temp_r29;
    s32 temp_r30;
    s32 temp_r31;
    s32 temp_r3;
    s32 temp_r3_2;
    s32 var_r0;
    s32 var_r0_2;
    s32 var_r25;
    u32 var_r26;
    u32 var_r27;
    u32 var_r3;

    var_f29 = saved_reg_f29;
    var_f30 = saved_reg_f30;
    var_f31 = saved_reg_f31;
    if (!(M2C_FIELD(arg0, s32 *, 0) & 0x02000000)) {
        goto block_2;
    }
    return 0U;
block_2:
    temp_f22 = fabs(farg0);
    M2C_FIELD(arg0, f32 *, 0xC4) = (f32) M2C_FIELD(&lbl_802D73A8, f32 *, 0);
    temp_f22_2 = temp_f22 + lbl_8042DFE0;
    M2C_FIELD(arg0, f32 *, 0xC8) = (f32) M2C_FIELD(&lbl_802D73A8, f32 *, 4);
    M2C_FIELD(arg0, f32 *, 0xCC) = (f32) M2C_FIELD(&lbl_802D73A8, f32 *, 8);
    M2C_FIELD(arg0, f32 *, 0xE0) = (f32) lbl_8042DF90;
    if ((u16) M2C_FIELD(marioGetPtr(M2C_FIELD(&lbl_802D73A8, f32 *, 0), lbl_8042DF90), u16 *, 0x2E) != 0x1A) {
        goto block_4;
    }
    var_f20 = lbl_8042DFE4;
    goto block_5;
block_4:
    var_f20 = lbl_8042DFB0 * partyGetHeight(arg0);
block_5:
    M2C_FIELD(arg0, f32 *, 0xE4) = (f32) M2C_FIELD(arg0, f32 *, 0x5C);
    M2C_FIELD(arg0, f32 *, 0xE8) = (f32) lbl_8042DFC4;
    if (lbl_8042DF90 == M2C_FIELD(arg0, f32 *, 0x104)) {
        goto block_7;
    }
    var_f1 = M2C_FIELD(arg0, f32 *, 0x100);
    goto block_8;
block_7:
    var_f1 = fn_8009764C(arg0, M2C_FIELD(arg0, f32 *, 0x10C));
block_8:
    temp_f19 = (lbl_8042DFD8 * var_f1) / lbl_8042DF94;
    temp_f3 = (f32) sin(temp_f19, lbl_8042DFD8);
    temp_f24 = lbl_8042DFE8 * (M2C_FIELD(arg0, f32 *, 0xF4) * temp_f3);
    temp_f21 = M2C_FIELD(arg0, f32 *, 0x5C);
    temp_r29 = M2C_FIELD(&lbl_802D73A8, s32 *, 0xC);
    temp_f27 = -temp_f24;
    temp_r30 = M2C_FIELD(&lbl_802D73A8, s32 *, 0x10);
    temp_f19_2 = temp_f21 + var_f20;
    temp_r31 = M2C_FIELD(&lbl_802D73A8, s32 *, 0x14);
    var_r26 = 0U;
    temp_f0 = M2C_FIELD(arg0, f32 *, 0xF4) * -(f32) cos(temp_f19, lbl_8042DFE8, temp_f3);
    sp34 = temp_r29;
    temp_f22_3 = M2C_FIELD(arg0, f32 *, 0x58);
    var_r25 = 0;
    sp38 = temp_r30;
    temp_f23 = lbl_8042DFE8 * temp_f0;
    sp3C = temp_r31;
    temp_f20 = M2C_FIELD(arg0, f32 *, 0x60);
    temp_f28 = -temp_f23;
    var_f25 = lbl_8042DFEC;
loop_9:
    if (var_r25 != 0) {
        goto block_11;
    }
    var_f31 = lbl_8042DF90;
    var_f30 = var_f31;
    goto block_19;
block_11:
    if (var_r25 != 1) {
        goto block_13;
    }
    var_f31 = temp_f24;
    var_f30 = temp_f23;
    goto block_19;
block_13:
    if (var_r25 != 2) {
        goto block_15;
    }
    var_f31 = temp_f28;
    var_f30 = temp_f24;
    goto block_19;
block_15:
    if (var_r25 != 3) {
        goto block_17;
    }
    var_f31 = temp_f23;
    var_f30 = temp_f27;
    goto block_19;
block_17:
    if (var_r25 != 4) {
        goto block_19;
    }
    var_f31 = temp_f27;
    var_f30 = temp_f28;
block_19:
    temp_f1 = temp_f22_3 + var_f31;
    sp44 = temp_f19_2;
    sp40 = temp_f1;
    sp48 = temp_f20 + var_f30;
    sp58 = sp40;
    sp5C = sp44;
    sp60 = sp48;
    sp64 = temp_r29;
    sp68 = temp_r30;
    sp6C = temp_r31;
    sp88 = var_f20 + temp_f22_2;
    if (yoshiGetStatus(sp44, sp40, temp_f1) == 0) {
        goto block_21;
    }
    var_r3 = hitCheckVecFilter(&sp4C, chkfilterYoshi);
    goto block_25;
block_21:
    if ((s32) M2C_FIELD(arg0, u8 *, 0x31) != 2) {
        goto block_24;
    }
    if (!(M2C_FIELD(arg0, s32 *, 0) & 0x100)) {
        goto block_24;
    }
    var_r3 = hitCheckVecFilter(&sp4C, chkfilterNokotaro);
    goto block_25;
block_24:
    var_r3 = hitCheckVecFilter(&sp4C, chkfilter);
block_25:
    if (var_r3 != 0U) {
        goto block_27;
    }
    goto block_31;
block_27:
    M2C_ERROR(/* unknown instruction: cror eq, gt, eq */);
    if (sp74 != lbl_8042DF90) {
        goto block_29;
    }
    var_f0 = lbl_8042DF8C;
    goto block_30;
block_29:
    var_f0 = lbl_8042DFB4;
block_30:
    sp98 = 0x43300000;
    sp2C = sp80;
    temp_r0 = (s32) ((lbl_8042DFB8 * sp74) + var_f0);
    sp30 = sp84;
    sp9C = temp_r0 ^ 0x80000000;
    sp28 = sp7C;
    var_f29 = (f32) temp_r0 / lbl_8042DFB8;
block_31:
    var_r27 = var_r3;
    if (var_r3 == 0U) {
        goto block_51;
    }
    sp20 = sp2C;
    sp24 = sp30;
    sp1C = sp28;
    M2C_FIELD(arg0, f32 *, 0xC4) = (f32) (s32) (angleABf(sp2C, lbl_8042DF90, lbl_8042DF90, lbl_8042DFA8 * sp24, lbl_8042DFA8 * sp20) - lbl_8042DF94);
    M2C_FIELD(arg0, f32 *, 0xC8) = (f32) lbl_8042DF90;
    var_f3 = (sp1C * sp1C) + (sp24 * sp24);
    M2C_FIELD(arg0, f32 *, 0xCC) = (f32) (s32) (angleABf(lbl_8042DF90, lbl_8042DF90, lbl_8042DFA8 * sp1C, lbl_8042DFA8 * sp20) - lbl_8042DF94);
    if (!(var_f3 > lbl_8042DF90)) {
        goto block_34;
    }
    temp_f1_2 = __frsqrte(var_f3);
    temp_f1_3 = M2C_FIELD(&lbl_802D73A8, f64 *, 0x110) * temp_f1_2 * -(((f64) var_f3 * (temp_f1_2 * temp_f1_2)) - M2C_FIELD(&lbl_802D73A8, f64 *, 0x118));
    temp_f1_4 = M2C_FIELD(&lbl_802D73A8, f64 *, 0x110) * temp_f1_3 * -(((f64) var_f3 * (temp_f1_3 * temp_f1_3)) - M2C_FIELD(&lbl_802D73A8, f64 *, 0x118));
    var_f3 = (f32) ((f64) var_f3 * (M2C_FIELD(&lbl_802D73A8, f64 *, 0x110) * temp_f1_4 * -(((f64) var_f3 * (temp_f1_4 * temp_f1_4)) - M2C_FIELD(&lbl_802D73A8, f64 *, 0x118))));
    goto block_49;
block_34:
    if (!(var_f3 < (f32) M2C_FIELD(&lbl_802D73A8, f64 *, 0x120))) {
        goto block_36;
    }
    var_f3 = __float_nan;
    goto block_49;
block_36:
    spC = var_f3;
    temp_r3 = M2C_BITWISE(s32, spC) & 0x7F800000;
    if (temp_r3 == 0x7F800000) {
        goto block_40;
    }
    if (temp_r3 >= 0x7F800000) {
        goto block_46;
    }
    if (temp_r3 == 0) {
        goto block_43;
    }
    goto block_46;
block_40:
    if (!(M2C_BITWISE(s32, spC) & 0x7FFFFF)) {
        goto block_42;
    }
    var_r0 = 1;
    goto block_47;
block_42:
    var_r0 = 2;
    goto block_47;
block_43:
    if (!(M2C_BITWISE(s32, spC) & 0x7FFFFF)) {
        goto block_45;
    }
    var_r0 = 5;
    goto block_47;
block_45:
    var_r0 = 3;
    goto block_47;
block_46:
    var_r0 = 4;
block_47:
    if (var_r0 != 1) {
        goto block_49;
    }
    var_f3 = __float_nan;
block_49:
    M2C_ERROR(/* unknown instruction: cror eq, gt, eq */);
    if (angleABf(lbl_8042DF90, lbl_8042DF90, var_f3, -sp20) != lbl_8042DFF0) {
        goto block_51;
    }
    var_r27 = 0U;
block_51:
    if (var_r27 == 0U) {
        goto block_78;
    }
    sp14 = sp2C;
    sp18 = sp30;
    sp10 = sp28;
    M2C_FIELD(arg0, f32 *, 0xC4) = (f32) (s32) (angleABf(sp2C, lbl_8042DF90, lbl_8042DF90, lbl_8042DFA8 * sp18, lbl_8042DFA8 * sp14) - lbl_8042DF94);
    M2C_FIELD(arg0, f32 *, 0xC8) = (f32) lbl_8042DF90;
    var_f3_2 = (sp10 * sp10) + (sp18 * sp18);
    M2C_FIELD(arg0, f32 *, 0xCC) = (f32) (s32) (angleABf(lbl_8042DF90, lbl_8042DF90, lbl_8042DFA8 * sp10, lbl_8042DFA8 * sp14) - lbl_8042DF94);
    if (!(var_f3_2 > lbl_8042DF90)) {
        goto block_54;
    }
    temp_f1_5 = __frsqrte(var_f3_2);
    temp_f1_6 = M2C_FIELD(&lbl_802D73A8, f64 *, 0x110) * temp_f1_5 * -(((f64) var_f3_2 * (temp_f1_5 * temp_f1_5)) - M2C_FIELD(&lbl_802D73A8, f64 *, 0x118));
    temp_f1_7 = M2C_FIELD(&lbl_802D73A8, f64 *, 0x110) * temp_f1_6 * -(((f64) var_f3_2 * (temp_f1_6 * temp_f1_6)) - M2C_FIELD(&lbl_802D73A8, f64 *, 0x118));
    var_f3_2 = (f32) ((f64) var_f3_2 * (M2C_FIELD(&lbl_802D73A8, f64 *, 0x110) * temp_f1_7 * -(((f64) var_f3_2 * (temp_f1_7 * temp_f1_7)) - M2C_FIELD(&lbl_802D73A8, f64 *, 0x118))));
    goto block_69;
block_54:
    if (!(var_f3_2 < (f32) M2C_FIELD(&lbl_802D73A8, f64 *, 0x120))) {
        goto block_56;
    }
    var_f3_2 = __float_nan;
    goto block_69;
block_56:
    sp8 = var_f3_2;
    temp_r3_2 = M2C_BITWISE(s32, sp8) & 0x7F800000;
    if (temp_r3_2 == 0x7F800000) {
        goto block_60;
    }
    if (temp_r3_2 >= 0x7F800000) {
        goto block_66;
    }
    if (temp_r3_2 == 0) {
        goto block_63;
    }
    goto block_66;
block_60:
    if (!(M2C_BITWISE(s32, sp8) & 0x7FFFFF)) {
        goto block_62;
    }
    var_r0_2 = 1;
    goto block_67;
block_62:
    var_r0_2 = 2;
    goto block_67;
block_63:
    if (!(M2C_BITWISE(s32, sp8) & 0x7FFFFF)) {
        goto block_65;
    }
    var_r0_2 = 5;
    goto block_67;
block_65:
    var_r0_2 = 3;
    goto block_67;
block_66:
    var_r0_2 = 4;
block_67:
    if (var_r0_2 != 1) {
        goto block_69;
    }
    var_f3_2 = __float_nan;
block_69:
    M2C_FIELD(arg0, f32 *, 0xE0) = angleABf(lbl_8042DF90, lbl_8042DF90, var_f3_2, -sp14);
    if (!(var_f29 < temp_f21)) {
        goto block_75;
    }
    temp_f1_8 = fabs(var_f29 - temp_f21);
    M2C_ERROR(/* unknown instruction: cror eq, gt, eq */);
    if (temp_f1_8 != lbl_8042DFDC) {
        goto block_75;
    }
    if (!(temp_f1_8 > fabs(farg1))) {
        goto block_73;
    }
    M2C_FIELD(arg0, f32 *, 0xE8) = fabs(temp_f21 - var_f29);
    goto block_78;
block_73:
    if (!(var_f29 > var_f25)) {
        goto block_78;
    }
    var_f25 = var_f29;
    var_r26 = var_r27;
    goto block_78;
block_75:
    M2C_ERROR(/* unknown instruction: cror eq, lt, eq */);
    if (fabs(var_f29 - temp_f21) != lbl_8042DFBC) {
        goto block_78;
    }
    if (!(var_f29 > var_f25)) {
        goto block_78;
    }
    var_f25 = var_f29;
    var_r26 = var_r27;
block_78:
    var_r25 += 1;
    if (var_r25 < 5) {
        goto loop_9;
    }
    if (var_r26 == 0U) {
        goto block_81;
    }
    M2C_FIELD(arg0, f32 *, 0xE4) = var_f25;
    M2C_FIELD(arg0, f32 *, 0xE8) = (f32) lbl_8042DF90;
    return var_r26;
block_81:
    return 0U;
}

void partyChkGnd(void *arg0) {
    f32 sp10;
    f32 spC;
    f32 sp8;
    s32 temp_r3;
    s32 var_r30;
    u32 var_r29;

    if (M2C_FIELD(arg0, s32 *, 0) & 0x02000000) {
        goto block_14;
    }
    var_r30 = 0;
    var_r29 = partySearchGround(arg0, lbl_8042DFBC, M2C_FIELD(arg0, f32 *, 0x114));
    if (var_r29 == 0U) {
        goto block_4;
    }
    temp_r3 = hitGetAttr();
    var_r30 = temp_r3;
    if (!(temp_r3 & 0x200)) {
        goto block_4;
    }
    var_r29 = 0U;
block_4:
    M2C_FIELD(arg0, u32 *, 0x138) = var_r29;
    M2C_FIELD(arg0, s32 *, 0) = (s32) (M2C_FIELD(arg0, s32 *, 0) & 0xFFFFFF7F);
    if (var_r29 != 0U) {
        goto block_6;
    }
    M2C_FIELD(arg0, s32 *, 0) = (s32) (M2C_FIELD(arg0, s32 *, 0) & 0xFFFFFFFD);
    partyChgMot(arg0, 3);
    M2C_FIELD(arg0, f32 *, 0x11C) = (f32) lbl_8042DFC4;
    goto block_9;
block_6:
    M2C_FIELD(arg0, s32 *, 0) = (s32) (M2C_FIELD(arg0, s32 *, 0) | 2);
    if (!(var_r30 & 0x100)) {
        goto block_8;
    }
    M2C_FIELD(arg0, s32 *, 0) = (s32) (M2C_FIELD(arg0, s32 *, 0) | 0x80);
    goto block_9;
block_8:
    M2C_FIELD(arg0, s32 *, 0) = (s32) (M2C_FIELD(arg0, s32 *, 0) & 0xFFFFFF7F);
block_9:
    M2C_FIELD(arg0, f32 *, 0x5C) = (f32) M2C_FIELD(arg0, f32 *, 0xE4);
    if (M2C_FIELD(arg0, s32 *, 0) & 0x30) {
        goto block_14;
    }
    partyChkFrontStep(arg0, &sp10, &spC, &sp8, lbl_8042DFBC);
    if (!((sp10 - M2C_FIELD(arg0, f32 *, 0x5C)) >= lbl_8042DFE4)) {
        goto block_14;
    }
    if (!(M2C_FIELD(arg0, f32 *, 0xE0) <= lbl_8042DFF4)) {
        goto block_14;
    }
    M2C_FIELD(arg0, f32 *, 0x120) = sp10;
    if (lbl_8042DF90 == M2C_FIELD(arg0, f32 *, 0x104)) {
        goto block_14;
    }
    partyChgMot(arg0, 6);
block_14:
    return;
}

void partyNrmToAngle(void *arg0) {
    f32 sp8;
    f32 temp_f0;
    f32 temp_f1;
    f32 var_f3;
    f64 temp_f1_2;
    f64 temp_f1_3;
    f64 temp_f1_4;
    s32 temp_r4;
    s32 var_r0;

    temp_f0 = M2C_FIELD(arg0, f32 *, 0);
    temp_f1 = M2C_FIELD(arg0, f32 *, 8);
    var_f3 = (temp_f0 * temp_f0) + (temp_f1 * temp_f1);
    if (!(var_f3 > lbl_8042DF90)) {
        goto block_2;
    }
    temp_f1_2 = __frsqrte(var_f3);
    temp_f1_3 = M2C_FIELD(&lbl_802D73A8, f64 *, 0x110) * temp_f1_2 * -(((f64) var_f3 * (temp_f1_2 * temp_f1_2)) - M2C_FIELD(&lbl_802D73A8, f64 *, 0x118));
    temp_f1_4 = M2C_FIELD(&lbl_802D73A8, f64 *, 0x110) * temp_f1_3 * -(((f64) var_f3 * (temp_f1_3 * temp_f1_3)) - M2C_FIELD(&lbl_802D73A8, f64 *, 0x118));
    var_f3 = (f32) ((f64) var_f3 * (M2C_FIELD(&lbl_802D73A8, f64 *, 0x110) * temp_f1_4 * -(((f64) var_f3 * (temp_f1_4 * temp_f1_4)) - M2C_FIELD(&lbl_802D73A8, f64 *, 0x118))));
    goto block_17;
block_2:
    if (!(var_f3 < (f32) M2C_FIELD(&lbl_802D73A8, f64 *, 0x120))) {
        goto block_4;
    }
    var_f3 = __float_nan;
    goto block_17;
block_4:
    sp8 = var_f3;
    temp_r4 = M2C_BITWISE(s32, sp8) & 0x7F800000;
    if (temp_r4 == 0x7F800000) {
        goto block_8;
    }
    if (temp_r4 >= 0x7F800000) {
        goto block_14;
    }
    if (temp_r4 == 0) {
        goto block_11;
    }
    goto block_14;
block_8:
    if (!(M2C_BITWISE(s32, sp8) & 0x7FFFFF)) {
        goto block_10;
    }
    var_r0 = 1;
    goto block_15;
block_10:
    var_r0 = 2;
    goto block_15;
block_11:
    if (!(M2C_BITWISE(s32, sp8) & 0x7FFFFF)) {
        goto block_13;
    }
    var_r0 = 5;
    goto block_15;
block_13:
    var_r0 = 3;
    goto block_15;
block_14:
    var_r0 = 4;
block_15:
    if (var_r0 != 1) {
        goto block_17;
    }
    var_f3 = __float_nan;
block_17:
    angleABf(lbl_8042DF90, lbl_8042DF90, var_f3, -M2C_FIELD(arg0, f32 *, 4));
    return;
}

void partyHitCheck(void *arg0, void *arg1, void *arg2, void *arg3, void *arg4, f32 *arg5) {
    f32 sp38;
    f32 sp3C;
    f32 sp40;
    f32 sp44;
    f32 sp48;
    f32 sp4C;

    f32 sp50;
    f32 sp34;
    f32 sp30;
    f32 sp2C;
    s32 sp28;
    s32 sp24;
    f32 sp20;
    M2C_UNK sp14;
    f32 sp10;
    f32 spC;
    f32 sp8;
    f32 temp_r3;
    f32 temp_r5;
    f32 temp_r8;
    f32 temp_r9;
    f32 var_f0;
    f32 var_f0_2;
    f32 var_f0_3;
    f32 var_f0_4;
    u32 var_r3;

    temp_r9 = M2C_FIELD(arg2, f32 *, 0);
    temp_r8 = M2C_FIELD(arg2, f32 *, 4);
    temp_r5 = M2C_FIELD(arg2, f32 *, 8);
    temp_r3 = M2C_FIELD(arg1, f32 *, 0);
    sp8 = temp_r9;
    sp20 = temp_r3;
    sp24 = M2C_FIELD(arg1, s32 *, 4);
    spC = temp_r8;
    sp28 = M2C_FIELD(arg1, s32 *, 8);
    sp2C = temp_r9;
    sp30 = temp_r8;
    sp34 = temp_r5;
    sp10 = temp_r5;
    sp50 = *arg5;
    if (yoshiGetStatus(temp_r3, temp_r5, temp_r8, temp_r9) == 0) {
        goto block_2;
    }
    var_r3 = hitCheckVecFilter(&sp14, chkfilterYoshi);
    goto block_6;
block_2:
    if ((s32) M2C_FIELD(arg0, u8 *, 0x31) != 2) {
        goto block_5;
    }
    if (!(M2C_FIELD(arg0, s32 *, 0) & 0x100)) {
        goto block_5;
    }
    var_r3 = hitCheckVecFilter(&sp14, chkfilterNokotaro);
    goto block_6;
block_5:
    var_r3 = hitCheckVecFilter(&sp14, chkfilter);
block_6:
    if (var_r3 != 0U) {
        goto block_8;
    }
    return;
block_8:
    M2C_ERROR(/* unknown instruction: cror eq, gt, eq */);
    if (sp50 != lbl_8042DF90) {
        goto block_10;
    }
    var_f0 = lbl_8042DF8C;
    goto block_11;
block_10:
    var_f0 = lbl_8042DFB4;
block_11:
    *arg5 = (f32) (s32) ((lbl_8042DFB8 * sp50) + var_f0) / lbl_8042DFB8;
    M2C_ERROR(/* unknown instruction: cror eq, gt, eq */);
    if (sp38 != lbl_8042DF90) {
        goto block_13;
    }
    var_f0_2 = lbl_8042DF8C;
    goto block_14;
block_13:
    var_f0_2 = lbl_8042DFB4;
block_14:
    M2C_FIELD(arg3, f32 *, 0) = (f32) ((f32) (s32) ((lbl_8042DFB8 * sp38) + var_f0_2) / lbl_8042DFB8);
    M2C_ERROR(/* unknown instruction: cror eq, gt, eq */);
    if (sp3C != lbl_8042DF90) {
        goto block_16;
    }
    var_f0_3 = lbl_8042DF8C;
    goto block_17;
block_16:
    var_f0_3 = lbl_8042DFB4;
block_17:
    M2C_FIELD(arg3, f32 *, 4) = (f32) ((f32) (s32) ((lbl_8042DFB8 * sp3C) + var_f0_3) / lbl_8042DFB8);
    M2C_ERROR(/* unknown instruction: cror eq, gt, eq */);
    if (sp40 != lbl_8042DF90) {
        goto block_19;
    }
    var_f0_4 = lbl_8042DF8C;
    goto block_20;
block_19:
    var_f0_4 = lbl_8042DFB4;
block_20:
    M2C_FIELD(arg3, f32 *, 8) = (f32) ((f32) (s32) ((lbl_8042DFB8 * sp40) + var_f0_4) / lbl_8042DFB8);
    M2C_FIELD(arg4, f32 *, 0) = sp44;
    M2C_FIELD(arg4, f32 *, 4) = sp48;
    M2C_FIELD(arg4, f32 *, 8) = sp4C;
    return;
}

s32 chkfilterNokotaro(void *arg0, void *arg1) {
    s32 temp_r3;
    s32 temp_r4;

    marioGetPtr();
    temp_r4 = M2C_FIELD(arg1, s32 *, 4);
    if (!(temp_r4 & 0x800005)) {
        goto block_2;
    }
    return 0;
block_2:
    if (!(temp_r4 & 0x80080)) {
        goto block_4;
    }
    return 1;
block_4:
    if (temp_r4 & 0x80000000) {
        goto block_6;
    }
    return 1;
block_6:
    temp_r3 = mobjHitObjPtrToPtr(arg1);
    if (strcmp((char *)temp_r3 + 0x15, &lbl_802D73A8 + 0x130) == 0) {
        goto block_9;
    }
    if (strcmp((char *)temp_r3 + 0x15, &lbl_802D73A8 + 0x148) == 0) {
        goto block_9;
    }
    if (strcmp((char *)temp_r3 + 0x15, &lbl_802D73A8 + 0x160) != 0) {
        goto block_10;
    }
block_9:
    mobjHitObjPtrToPtr(arg1);
    return 0;
block_10:
    return 1;
}

s32 chkfilterYoshi(void *arg0, void *arg1) {
    s32 temp_r3;
    s32 temp_r4;

    marioGetPtr();
    temp_r4 = M2C_FIELD(arg1, s32 *, 4);
    if (!(temp_r4 & 0x80080)) {
        goto block_2;
    }
    return 1;
block_2:
    if (!(temp_r4 & 0x800000)) {
        goto block_4;
    }
    return 1;
block_4:
    if (!(temp_r4 & 4)) {
        goto block_6;
    }
    return 0;
block_6:
    if (temp_r4 & 0x80000000) {
        goto block_8;
    }
    return 1;
block_8:
    temp_r3 = mobjHitObjPtrToPtr(arg1);
    if (strcmp((char *)temp_r3 + 0x15, &lbl_802D73A8 + 0x130) == 0) {
        goto block_11;
    }
    if (strcmp((char *)temp_r3 + 0x15, &lbl_802D73A8 + 0x148) == 0) {
        goto block_11;
    }
    if (strcmp((char *)temp_r3 + 0x15, &lbl_802D73A8 + 0x160) != 0) {
        goto block_12;
    }
block_11:
    mobjHitObjPtrToPtr(arg1);
    return 0;
block_12:
    return 1;
}

s32 chkfilter(void *arg0, void *arg1) {
    s32 temp_r3;
    s32 temp_r4;

    marioGetPtr();
    temp_r4 = M2C_FIELD(arg1, s32 *, 4);
    if (!(temp_r4 & 0x80080)) {
        goto block_2;
    }
    return 1;
block_2:
    if (!(temp_r4 & 0x800004)) {
        goto block_4;
    }
    return 0;
block_4:
    if (temp_r4 & 0x80000000) {
        goto block_6;
    }
    return 1;
block_6:
    temp_r3 = mobjHitObjPtrToPtr(arg1);
    if (strcmp((char *)temp_r3 + 0x15, &lbl_802D73A8 + 0x130) == 0) {
        goto block_9;
    }
    if (strcmp((char *)temp_r3 + 0x15, &lbl_802D73A8 + 0x148) == 0) {
        goto block_9;
    }
    if (strcmp((char *)temp_r3 + 0x15, &lbl_802D73A8 + 0x160) != 0) {
        goto block_10;
    }
block_9:
    mobjHitObjPtrToPtr(arg1);
    return 0;
block_10:
    return 1;
}
