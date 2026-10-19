#pragma once

#include <dolphin/types.h>

void camRoadCurveOff(char* name);
void camRoadCurveOn(char* name);
void camRoadReset(void);
f32 camRoadGetCurrentShiftXRate(void);
void* camRoadGetWorkPtr(void);
void camRoadMain();
void calcHokanCamRoad(void);
void compareCamRoad(void);
void calcCamRoad(void);
void collisionCurve(void);
void collisionTri_simple(void);
void calcCurrentCamState(void);
void fn_8000E2C8(void);
