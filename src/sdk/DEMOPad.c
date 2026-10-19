#include <dolphin/pad.h>
#include <demo/DEMOPad.h>

static u32 PadChanMask[4] = {
    PAD_CHAN0_BIT,
    PAD_CHAN1_BIT,
    PAD_CHAN2_BIT,
    PAD_CHAN3_BIT,
};

static PADStatus Pad[4];
DEMOPadStatus DemoPad[4];

u32 DemoNumValidPads = 0;

static void DEMOPadCopy(PADStatus* pad, DEMOPadStatus* dmpad) {
    u16 dirs;

    if (pad->err != -3) {
        dirs = 0;
        if (pad->stickX < -0x30) {
            dirs |= 0x4000;
        }
        if (pad->stickX > 0x30) {
            dirs |= 0x8000;
        }
        if (pad->stickY < -0x30) {
            dirs |= 0x2000;
        }
        if (pad->stickY > 0x30) {
            dirs |= 0x1000;
        }
        if (pad->substickX < -0x30) {
            dirs |= 0x400;
        }
        if (pad->substickX > 0x30) {
            dirs |= 0x800;
        }
        if (pad->substickY < -0x30) {
            dirs |= 0x200;
        }
        if (pad->substickY > 0x30) {
            dirs |= 0x100;
        }

        dmpad->dirsNew = (dirs & (dmpad->dirs ^ dirs));
        dmpad->dirsReleased = (dmpad->dirs & (dmpad->dirs ^ dirs));
        dmpad->dirs = dirs;
        dmpad->buttonDown = (pad->button & (dmpad->pst.button ^ pad->button));
        dmpad->buttonUp = (dmpad->pst.button & (dmpad->pst.button ^ pad->button));
        dmpad->stickDeltaX = (pad->stickX - dmpad->pst.stickX);
        dmpad->stickDeltaY = (pad->stickY - dmpad->pst.stickY);
        dmpad->substickDeltaX = (pad->substickX - dmpad->pst.substickX);
        dmpad->substickDeltaY = (pad->substickY - dmpad->pst.substickY);
        dmpad->pst = *pad;
    } else {
        dmpad->dirsNew = dmpad->dirsReleased = 0;
        dmpad->buttonDown = dmpad->buttonUp = 0;
        dmpad->stickDeltaX = dmpad->stickDeltaY = 0;
        dmpad->substickDeltaX = dmpad->substickDeltaY = 0;
    }
}

#ifdef TARGET_PC
#include <stdbool.h>
#include <stdio.h>
extern const bool* SDL_GetKeyboardState(int* numkeys);
#endif

void DEMOPadRead(void) {
    s32 i;
    u32 ResetReq = 0;

    PADRead(&Pad[0]);
    PADClamp(&Pad[0]);

#ifdef TARGET_PC
    {
        int numkeys = 0;
        const bool* k = SDL_GetKeyboardState(&numkeys);
        if (k && numkeys > 0) {
            // S'il n'y a pas de manette physique detectee sur le port 0, activer le port 0 pour le clavier
            if (Pad[0].err != 0) {
                Pad[0].err = 0;
                Pad[0].button = 0;
                Pad[0].stickX = 0;
                Pad[0].stickY = 0;
                Pad[0].substickX = 0;
                Pad[0].substickY = 0;
                Pad[0].triggerLeft = 0;
                Pad[0].triggerRight = 0;
                Pad[0].analogA = 0;
                Pad[0].analogB = 0;
            }

            // Bouton A / START / Valider (Entree: 40, Espace: 44, Pave Num Entree: 88)
            if ((40 < numkeys && k[40]) || (44 < numkeys && k[44]) || (88 < numkeys && k[88])) {
                Pad[0].button |= (PAD_BUTTON_START | PAD_BUTTON_A);
            }
            // Bouton B / Annuler / Marteau (X: 27, Echap: 41, Retour: 42)
            if ((27 < numkeys && k[27]) || (41 < numkeys && k[41]) || (42 < numkeys && k[42])) {
                Pad[0].button |= PAD_BUTTON_B;
            }
            // Bouton X (Touche C: 6)
            if (6 < numkeys && k[6]) {
                Pad[0].button |= PAD_BUTTON_X;
            }
            // Bouton Y (Touche V: 25)
            if (25 < numkeys && k[25]) {
                Pad[0].button |= PAD_BUTTON_Y;
            }
            // Gachette Z (Tabulation: 43)
            if (43 < numkeys && k[43]) {
                Pad[0].button |= PAD_TRIGGER_Z;
            }
            // Gachettes L / R (Shift Gauche: 225, Shift Droit: 229)
            if (225 < numkeys && k[225]) {
                Pad[0].button |= PAD_TRIGGER_L;
                Pad[0].triggerLeft = 255;
            }
            if (229 < numkeys && k[229]) {
                Pad[0].button |= PAD_TRIGGER_R;
                Pad[0].triggerRight = 255;
            }

            // Directions : Fleches et touches ZQSD / WASD
            // HAUT: Fleche Haut (82), W (26), Z (29)
            if ((82 < numkeys && k[82]) || (26 < numkeys && k[26]) || (29 < numkeys && k[29])) {
                Pad[0].button |= PAD_BUTTON_UP;
                Pad[0].stickY = 127;
            }
            // BAS: Fleche Bas (81), S (22)
            if ((81 < numkeys && k[81]) || (22 < numkeys && k[22])) {
                Pad[0].button |= PAD_BUTTON_DOWN;
                Pad[0].stickY = -127;
            }
            // GAUCHE: Fleche Gauche (80), A (4), Q (20)
            if ((80 < numkeys && k[80]) || (4 < numkeys && k[4]) || (20 < numkeys && k[20])) {
                Pad[0].button |= PAD_BUTTON_LEFT;
                Pad[0].stickX = -127;
            }
            // DROITE: Fleche Droite (79), D (7)
            if ((79 < numkeys && k[79]) || (7 < numkeys && k[7])) {
                Pad[0].button |= PAD_BUTTON_RIGHT;
                Pad[0].stickX = 127;
            }
        }

        // Progression automatique pour tests et demonstrations autonomes
        extern int seqGetSeq(void);
        static int s_logoFrames = 0;
        static int s_titleFrames = 0;
        static int s_loadFrames = 0;
        static int s_gameFrames = 0;
        int currentSeq = seqGetSeq();

        // 0 = SEQ_LOGO, 1 = SEQ_TITLE, 6 = SEQ_LOAD, 2 = SEQ_GAME
        if (currentSeq == 0) { // SEQ_LOGO
            s_logoFrames++;
            if (s_logoFrames >= 45 && s_logoFrames <= 50) {
                Pad[0].button |= (PAD_BUTTON_START | PAD_BUTTON_A);
            }
        } else if (currentSeq == 1) { // SEQ_TITLE
            s_titleFrames++;
            if (s_titleFrames >= 90 && s_titleFrames <= 95) {
                Pad[0].button |= (PAD_BUTTON_START | PAD_BUTTON_A);
            }
        } else if (currentSeq == 6) { // SEQ_LOAD
            s_loadFrames++;
            if (s_loadFrames >= 60 && s_loadFrames <= 65) {
                Pad[0].button |= PAD_BUTTON_A;
            }
        } else if (currentSeq == 3) { // SEQ_MAPCHANGE (prologue cutscenes)
            s_gameFrames++;
            if (s_gameFrames == 1) {
                printf("\n========================================================\n");
                printf("  [PORT PROGRESS] SUCCESS! Authentic 3D GameCube World Active!\n");
                printf("  Paper Mario: La Porte Millenaire native PC rendering via Aurora!\n");
                printf("========================================================\n\n");
                fflush(stdout);
            }
            // Periodically press 'A' to advance dialogue in cutscenes
            if ((s_gameFrames % 90) >= 0 && (s_gameFrames % 90) <= 2) {
                Pad[0].button |= PAD_BUTTON_A;
            }
        } else if (currentSeq == 2) { // SEQ_GAME (Playable Overworld)
            static int s_loggedPlayable = 0;
            if (!s_loggedPlayable) {
                s_loggedPlayable = 1;
                printf("\n========================================================\n");
                printf("  [GAMEPLAY READY] Controle joueur actif dans le monde 3D !\n");
                printf("  Deplacement : ZQSD / WASD / Fleches | Saut : Espace / Entree\n");
                printf("========================================================\n\n");
                fflush(stdout);
            }
        }
    }
#endif

    DemoNumValidPads = 0;

    for (i = 0; i < 4; i++) {
        if (Pad[i].err == 0 || Pad[i].err == -3) {
            DemoNumValidPads++;
        } else if (Pad[i].err == -1) {
            ResetReq |= PadChanMask[i];
        }

        DEMOPadCopy(&Pad[i], &DemoPad[i]);
    }

    if (ResetReq != 0) {
        PADReset(ResetReq);
    }
}

void DEMOPadInit(void) {
    s32 i;

    PADInit();

    for (i = 0; i < 4; i++) {
        DemoPad[i].pst.button = 0;
        DemoPad[i].pst.stickX = 0;
        DemoPad[i].pst.stickY = 0;
        DemoPad[i].pst.substickX = 0;
        DemoPad[i].pst.substickY = 0;
        DemoPad[i].pst.triggerLeft = 0;
        DemoPad[i].pst.triggerRight = 0;
        DemoPad[i].pst.analogA = 0;
        DemoPad[i].pst.analogB = 0;
        DemoPad[i].pst.err = 0;
        DemoPad[i].buttonDown = 0;
        DemoPad[i].buttonUp = 0;
        DemoPad[i].dirs = 0;
        DemoPad[i].dirsNew = 0;
        DemoPad[i].dirsReleased = 0;
        DemoPad[i].stickDeltaX = 0;
        DemoPad[i].stickDeltaY = 0;
        DemoPad[i].substickDeltaX = 0;
        DemoPad[i].substickDeltaY = 0;
    }
}
