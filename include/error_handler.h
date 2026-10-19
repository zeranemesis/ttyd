#pragma once

#include <dolphin/types.h>

#ifdef TARGET_PC
void setupErrorHandler(void);
#else
void N_setupErrorHandler(void);
#define setupErrorHandler N_setupErrorHandler
#endif
