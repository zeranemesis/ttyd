#include "battle/battle_camera.h"
#include "battle/battle.h"
#include <string.h>

extern BattleWork *_battleWorkPointer;
#ifndef TARGET_PC
extern const f32 lbl_8042F224;
#endif

// this file uses -O4,s
void battleCameraInit(void) {
  BattleWork *wp = _battleWorkPointer;
  BattleCameraWork *camera;

  camera = &wp->cameraWork;
  memset(camera, 0, sizeof(BattleCameraWork));
  camera->flags = 0;
  switch (wp->fbatInfo->partyId) {
  case 1:
  case 2:
  case 3:
  case 4:
  case 5:
  case 6:
  case 7:
  case 8:
    camera->mode = 15;
    break;
  case 9:
    camera->mode = 16;
    break;
  case 0:
    camera->mode = 14;
    break;
  }
  camera->priLimit = 0;
  camera->zoom = 0.0f;
  camera->moveSpeedLv = 1;
  camera->zoomSpeedLv = 1;
  camera->position = (Vec){0.0f, 0.0f, 0.0f};
  camera->unk4 = 0;
}

void btl_camera_set_prilimit(s32 limit) {
  _battleWorkPointer->cameraWork.priLimit = limit;
}

void btl_camera_off_posoffset_manual(s32 priority) {
  BattleWork *wp = _battleWorkPointer;
  if (priority < wp->cameraWork.priLimit) {
    return;
  }
#ifdef TARGET_PC
  wp->cameraWork.flags &= ~0x20;
  wp->cameraWork.position = (Vec){0.0f, 0.0f, 0.0f};
#else
  wp->cameraWork.flags &= ~0x20;
  wp->cameraWork.position.x = lbl_8042F224;
  wp->cameraWork.position.y = lbl_8042F224;
  wp->cameraWork.position.z = lbl_8042F224;
#endif
}

void btl_camera_set_posoffset(s32 priority, f32 x, f32 y, f32 z) {
  BattleCameraWork *camera = &_battleWorkPointer->cameraWork;
  if (priority < camera->priLimit) {
    return;
  }
  camera->flags |= 0x20;
  camera->position.x = x;
  camera->position.y = y;
  camera->position.z = z;
}

void btl_camera_nomove_z_onoff(s32 priority, s32 enable) {
  BattleCameraWork *camera = &_battleWorkPointer->cameraWork;
  if (priority < camera->priLimit) {
    return;
  }
  if (enable == 1) {
    camera->flags |= 0x10;
  } else {
    camera->flags &= ~0x10;
  }
}

void btl_camera_nomove_y_onoff(s32 priority, s32 enable) {
  BattleCameraWork *camera = &_battleWorkPointer->cameraWork;
  if (priority < camera->priLimit) {
    return;
  }
  if (enable == 1) {
    camera->flags |= 0x8;
  } else {
    camera->flags &= ~0x8;
  }
}

void btl_camera_nomove_x_onoff(s32 priority, s32 enable) {
  BattleCameraWork *camera = &_battleWorkPointer->cameraWork;
  if (priority < camera->priLimit) {
    return;
  }
  if (enable == 1) {
    camera->flags |= 0x4;
  } else {
    camera->flags &= ~0x4;
  }
}

void btl_camera_noshake(s32 priority) {
  BattleCameraWork *camera = &_battleWorkPointer->cameraWork;
  if (priority < camera->priLimit) {
    return;
  }
  *(volatile s32 *)&camera->flags &= ~1;
  *(volatile s32 *)&camera->flags &= ~2;
}

void btl_camera_set_zoom(s32 priority, f32 zoom) {
  BattleCameraWork *camera = &_battleWorkPointer->cameraWork;
  if (priority >= camera->priLimit) {
    camera->zoom = zoom;
  }
}

void btl_camera_add_zoom(s32 priority, f32 zoom) {
  BattleCameraWork *camera = &_battleWorkPointer->cameraWork;
  if (priority >= camera->priLimit) {
    camera->zoom += zoom;
  }
}

void btl_camera_set_zoomSpeedLv(s32 priority, s32 level) {
  BattleCameraWork *camera = &_battleWorkPointer->cameraWork;
  if (priority < camera->priLimit) {
    return;
  }
  if (level >= 0 && level <= 5) {
    camera->zoomSpeedLv = level;
    return;
  }
  camera->zoomSpeedLv = 1;
}

void btl_camera_set_moveSpeedLv(s32 priority, s32 level) {
  BattleCameraWork *camera = &_battleWorkPointer->cameraWork;
  if (priority < camera->priLimit) {
    return;
  }
  if (level >= 0 && level <= 5) {
    camera->moveSpeedLv = level;
  } else {
    camera->moveSpeedLv = 1;
  }
  camera->zoomSpeedLv = *(volatile s16 *)&camera->moveSpeedLv;
}
