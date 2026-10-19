#include "evt/evt_cam.h"
#include "evt/evt_cmd.h"
#include "driver/camdrv.h"
#include "cam_road.h"
#include "manager/evtmgr.h"
#include "manager/evtmgr_cmd.h"
#include "mario/mario.h"

extern void camShiftReset(void);
extern void fn_800DCF0C(s32 arg);
extern void psndClearFlag(s32 flag);
extern const f32 lbl_8042DAC8;

USER_FUNC(evt_cam_letter_box_camid) {
	camGetPtr(CAMERA_2D)->letterBoxCamId = evtGetValue(event, event->args[0]);
	return EVT_RETURN_DONE;
}

USER_FUNC(evt_cam_letter_box_disable) {
	s32 onoff = evtGetValue(event, event->args[0]);
	CameraEntry* cam = camGetPtr(CAMERA_2D);
	if (onoff) {
		cam->flags |= 0x200;
		fn_800DCF0C(0x40);
	} else {
		cam->flags &= ~0x200;
		psndClearFlag(0x40);
	}
	return EVT_RETURN_DONE;
}

USER_FUNC(evt_cam_type_change) {
	s32* args = event->args;
	s32 mode = evtGetValue(event, args[0]);
	evtGetValue(event, args[1]);
	if (mode == 0) ((void (*)(void))camSetTypePersp)();
	else ((void (*)(void))camSetTypeOrtho)();
	return EVT_RETURN_DONE;
}

USER_FUNC(evt_cam_shift_reset) {
	marioGetPtr();
	camGetPtr(CAMERA_3D);
	camShiftReset();
	return EVT_RETURN_DONE;
}

USER_FUNC(evt_cam3d_road_shift_onoff) {
	s32 onoff = evtGetValue(event, event->args[0]);
	CameraEntry* cam = camGetPtr(CAMERA_3D);
	if (onoff) cam->flags &= ~0x20;
	else cam->flags |= 0x20;
	return EVT_RETURN_DONE;
}

USER_FUNC(evt_cam_get_at) {
	s32* args = event->args;
	s32 cameraId = evtGetValue(event, args[0]);
	CameraEntry* cam = camGetPtr(cameraId);
	evtSetFloat(event, args[1], cam->cameraAt.x);
	evtSetFloat(event, args[2], cam->cameraAt.y);
	evtSetFloat(event, args[3], cam->cameraAt.z);
	return EVT_RETURN_DONE;
}

USER_FUNC(evt_cam_get_pos) {
	s32* args = event->args;
	s32 cameraId = evtGetValue(event, args[0]);
	CameraEntry* cam = camGetPtr(cameraId);
	evtSetFloat(event, args[1], cam->cameraPos.x);
	evtSetFloat(event, args[2], cam->cameraPos.y);
	evtSetFloat(event, args[3], cam->cameraPos.z);
	return EVT_RETURN_DONE;
}

USER_FUNC(evt_cam3d_evt_set) {
    s32* args = event->args;
    CameraEntry* cam = camGetPtr(CAMERA_3D);
    f32 fov;
    if (cam) {
        cam->target.x = evtGetFloat(event, args[0]);
        cam->target.y = evtGetFloat(event, args[1]);
        cam->target.z = evtGetFloat(event, args[2]);
        cam->cameraAt = cam->target;
        cam->cameraPos.x = evtGetFloat(event, args[3]);
        cam->cameraPos.y = evtGetFloat(event, args[4]);
        cam->cameraPos.z = evtGetFloat(event, args[5]);
        fov = evtGetFloat(event, args[6]);
        if (fov > 0.0f) {
            cam->fovY = fov;
        }
        MTXLookAt(cam->view, &cam->cameraPos, &cam->cameraUp, &cam->cameraAt);
    }
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_cam3d_evt_off) {
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_cam_letter_box_onoff) {
	s32* args = event->args;
	s32 onoff = evtGetValue(event, args[0]);
	s32 enable = evtGetValue(event, args[1]);
	CameraEntry* cam = camGetPtr(CAMERA_2D);
	if (onoff) {
		cam->flags |= 0x100;
		fn_800DCF0C(0x20);
		cam->letterBoxAlpha = 0xFF;
		cam->letterBoxTop = 0;
		cam->letterBoxBottom = 0;
		cam->letterBoxCamId = 8;
	} else {
		cam->flags &= ~0x100;
		psndClearFlag(0x20);
	}
	if (enable) cam->flags |= 0x400;
	return EVT_RETURN_DONE;
}

USER_FUNC(evt_cam_ctrl_onoff) {
    s32* args = event->args;
    s32 cameraId = evtGetValue(event, args[0]);
    s32 onoff = evtGetValue(event, args[1]);
    if (onoff) {
        camCtrlOn(cameraId);
    } else {
        camCtrlOff(cameraId);
    }
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_cam_road_reset) {
	MarioWork* mario = marioGetPtr();
	CameraEntry* cam = camGetPtr(CAMERA_3D);

	cam->marioPos.x = mario->position.x;
	cam->marioPos.y = lbl_8042DAC8 + mario->position.y;
	cam->marioPos.z = mario->position.z;
	cam->marioAt.x = mario->position.x;
	cam->marioAt.y = mario->position.y;
	cam->marioAt.z = mario->position.z;
	mario->field_EC = mario->position;
	mario->field_F8 = mario->position;
	camRoadReset();
	camShiftReset();
	return EVT_RETURN_DONE;
}
