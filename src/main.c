#include "main.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mario/mariost.h"
#include "sdk/DEMOInit.h"

extern GlobalWork* gp;

#ifndef TARGET_PC
extern GlobalWork* wp_80422810;
#define gp wp_80422810
#endif

#ifdef TARGET_PC
#include <aurora/aurora.h>
#include <aurora/event.h>
#include <stdbool.h>

bool g_gameRunning = true;

int game_main(void) {
    OSTick base_tick;

    marioStInit();
    printf("[game_main] Initial marioStMain()...\n"); fflush(stdout);
    marioStMain();
    printf("[game_main] Initial marioStMain() done! Entering main loop...\n"); fflush(stdout);

    const char* exitFramesEnv = getenv("TTYD_AUTO_EXIT_FRAMES");
    int autoExitFrames = exitFramesEnv ? atoi(exitFramesEnv) : 0;

    int loopCount = 0;
    while (g_gameRunning) {
        const AuroraEvent* event = aurora_update();
        while (event != NULL && event->type != AURORA_NONE) {
            if (event->type == AURORA_EXIT) {
                g_gameRunning = false;
                break;
            }
            event++;
        }
        if (!g_gameRunning) break;

        if (!aurora_begin_frame()) {
            continue;
        }

        if (loopCount < 5) {
            printf("[game_main] Frame %d: DEMOBeforeRender...\n", loopCount); fflush(stdout);
        }
        base_tick = OSGetTick();
        DEMOBeforeRender();
        if (loopCount < 5) {
            printf("[game_main] Frame %d: marioStDisp...\n", loopCount); fflush(stdout);
        }
        marioStDisp();
        if (loopCount < 5) {
            printf("[game_main] Frame %d: marioStMain...\n", loopCount); fflush(stdout);
        }
        marioStMain();
        gp->mDeltaGame = OSGetTick() - base_tick;
        if (loopCount < 5) {
            printf("[game_main] Frame %d: DEMODoneRender...\n", loopCount); fflush(stdout);
        }
        DEMODoneRender();
        if (loopCount < 5) {
            printf("[game_main] Frame %d: after DEMODoneRender, calling aurora_end_frame...\n", loopCount); fflush(stdout);
        }
        gp->mDeltaRender = OSGetTick() - base_tick;
        aurora_end_frame();
        if (loopCount < 5) {
            printf("[game_main] Frame %d: after aurora_end_frame!\n", loopCount); fflush(stdout);
        }

        extern void SaveWindowScreenshot(const char* filename);
        if (loopCount == 30 || loopCount == 120 || loopCount == 210 || loopCount == 320 ||
            loopCount == 420 || loopCount == 520) {
            char scrPath[64];
            sprintf(scrPath, "C:\\ttyd\\screenshot_frame_%d.bmp", loopCount);
            SaveWindowScreenshot(scrPath);
        }

        if (autoExitFrames > 0 && loopCount >= autoExitFrames) {
            printf("[game_main] Reached TTYD_AUTO_EXIT_FRAMES (%d), exiting for verification...\n", loopCount);
            fflush(stdout);
            g_gameRunning = false;
            break;
        }

        if (loopCount % 60 == 0) {
            printf("[Game Loop] Frame %d, Seq: %d, renderTime: %lld ms\n",
                loopCount, seqGetSeq(), (long long)OSTicksToMilliseconds(gp->renderTime));
            fflush(stdout);
        }
        loopCount++;
    }

    return 0;
}
#else
int main(void) {
    OSTick base_tick;

    marioStInit();
    marioStMain();
    while (1) {
        base_tick = OSGetTick();
        DEMOBeforeRender();
        marioStDisp();
        marioStMain();
        gp->mDeltaGame = OSGetTick() - base_tick;
        DEMODoneRender();
        gp->mDeltaRender = OSGetTick() - base_tick;
    }

    return 0;
}
#endif
