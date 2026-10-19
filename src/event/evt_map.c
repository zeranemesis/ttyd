#include "evt/evt_cmd.h"
#include "manager/evtmgr_cmd.h"
#include "driver/mapdrv.h"
#include "driver/mobjdrv.h"
#include "driver/animdrv.h"
#include "driver/hitdrv.h"
#include "mario/mario.h"
#include <string.h>

extern void N_mapDispOn(void);
extern void N_mapDispOff(void);
extern void mapSetTevCallback(s32 type, s32 callback);
extern void mapSetPlayRate(s32 rate);
extern void mapReStartAnimation(void);
extern void mapReStartAnimationAll(void);
extern void mapPauseAnimation(void);
extern void mapPauseAnimationAll(void);
extern void fn_8002A160(s32 mapObj, s32* animId, f32* frame);
extern void fn_80029F18(s32 mapObjId, s32 animId, s32 mode);
extern MapObjectEntry* mobjNameToPtr(const char* name);
extern const u32 lbl_8042C4B8;
extern const f32 lbl_8042C4C8;
extern const GXColor lbl_80436070;
extern const GXColor lbl_80436074;
extern const GXColor lbl_80436078;
extern const GXColor lbl_8043607C;

USER_FUNC(N_evt_mapdisp_onoff) {
	if (evtGetValue(event, event->args[0])) {
		N_mapDispOn();
	} else {
		N_mapDispOff();
	}
	return EVT_RETURN_DONE;
}

USER_FUNC(check2) {
	s32* args = event->args;
	MarioWork* mario = marioGetPtr();
	evtSetValue(event, args[0], 0);
	if (mario->hitobjStandOn != NULL &&
	    strcmp((const char*)event->lwData[1], hitGetName((HitEntry*)mario->hitobjStandOn))) {
		evtSetValue(event, args[0], 1);
	}
	return EVT_RETURN_DONE;
}

USER_FUNC(check) {
	s32* args = event->args;
	MarioWork* mario = marioGetPtr();
	evtSetValue(event, args[0], 0);
	if (mario->hitobjStandOn != NULL &&
	    !strcmp((const char*)event->lwData[1], hitGetName((HitEntry*)mario->hitobjStandOn))) {
		evtSetValue(event, args[0], 1);
	}
	return EVT_RETURN_DONE;
}

USER_FUNC(evt_map_get_flush_color) {
	s32* args = event->args;
	u8 r, g, b, a;
	const char* name = (const char*)evtGetValue(event, args[0]);
	mapObjGetFlushColor(name, &r, &g, &b, &a);
	evtSetValue(event, args[1], r);
	evtSetValue(event, args[2], g);
	evtSetValue(event, args[3], b);
	evtSetValue(event, args[4], a);
	return EVT_RETURN_DONE;
}

USER_FUNC(evt_map_set_flush_color) {
	s32* args = event->args;
	s32 type = evtGetValue(event, args[0]);
	const char* name = (const char*)evtGetValue(event, args[1]);
	s32 r = evtGetValue(event, args[2]);
	s32 g = evtGetValue(event, args[3]);
	s32 b = evtGetValue(event, args[4]);
	s32 a = evtGetValue(event, args[5]);
	if (type == 0) mapObjSetFlushColor(name, (u8)r, (u8)g, (u8)b, (u8)a);
	else mapGrpSetFlushColor(name, (u8)r, (u8)g, (u8)b, (u8)a);
	return EVT_RETURN_DONE;
}

USER_FUNC(evt_map_get_fog) {
	s32* args = event->args;
	s32 type;
	f32 start;
	f32 end;
	GXColor color;
	mapGetFog(&type, &start, &end, &color);
	evtSetValue(event, args[0], type);
	evtSetFloat(event, args[1], start);
	evtSetFloat(event, args[2], end);
	evtSetValue(event, args[3], color.r);
	evtSetValue(event, args[4], color.g);
	evtSetValue(event, args[5], color.b);
	return EVT_RETURN_DONE;
}

USER_FUNC(evt_map_set_fog) {
	s32* args = event->args;
	s32 type = evtGetValue(event, args[0]);
	f32 start = evtGetFloat(event, args[1]);
	f32 end = evtGetFloat(event, args[2]);
	s32 red = evtGetValue(event, args[3]);
	s32 green = evtGetValue(event, args[4]);
	s32 blue = evtGetValue(event, args[5]);
	GXColor color;
	GXColor colorTemp;
	*(u32*)&colorTemp = lbl_8042C4B8;
	colorTemp.r = red;
	colorTemp.g = green;
	colorTemp.b = blue;
	color = colorTemp;
	mapSetFog(type, start, end, &color);
	return EVT_RETURN_DONE;
}

USER_FUNC(evt_map_set_flush_onoff) {
	s32* args = event->args;
	s32 type = evtGetValue(event, args[0]);
	s32 onoff = evtGetValue(event, args[1]);
	const char* name = (const char*)evtGetValue(event, args[2]);
	if (type == 0) {
		if (onoff) mapObjFlushOn(name); else mapObjFlushOff(name);
	} else {
		if (onoff) mapGrpFlushOn(name); else mapGrpFlushOff(name);
	}
	return EVT_RETURN_DONE;
}

USER_FUNC(evt_map_set_tevcallback) {
	s32* args = event->args;
	s32 type = evtGetValue(event, args[0]);
	s32 callback = evtGetValue(event, args[1]);
	mapSetTevCallback(type, callback);
	return EVT_RETURN_DONE;
}

USER_FUNC(evt_mapobj_clear_offscreen) {
	s32* args = event->args;
	s32 type = evtGetValue(event, args[0]);
	const char* name = (const char*)evtGetValue(event, args[1]);
	if (type == 0) mapObjClearOffScreen(name); else mapGrpClearOffScreen(name);
	return EVT_RETURN_DONE;
}

USER_FUNC(evt_mapobj_set_offscreen) {
	s32* args = event->args;
	s32 type = evtGetValue(event, args[0]);
	const char* name = (const char*)evtGetValue(event, args[1]);
	const char* offscreen = (const char*)evtGetValue(event, args[2]);
	if (type == 0) mapObjSetOffScreen(name, offscreen); else mapGrpSetOffScreen(name, offscreen);
	return EVT_RETURN_DONE;
}

USER_FUNC(evt_mapobj_flag_onoff) {
	s32* args = event->args;
	s32 type = evtGetValue(event, args[0]);
	s32 onoff = evtGetValue(event, args[1]);
	const char* name = (const char*)evtGetValue(event, args[2]);
	u32 flags = args[3];
	if (type == 0) {
		if (onoff == 0) mapObjFlagOff(name, flags); else mapObjFlagOn(name, flags);
	} else {
		if (onoff == 0) mapGrpFlagOff(name, flags); else mapGrpFlagOn(name, flags);
	}
	return EVT_RETURN_DONE;
}

USER_FUNC(evt_map_blend_set_mobj_flag) {
	s32* args = event->args;
	s32 onoff = evtGetValue(event, args[0]);
	const char* name = (const char*)evtGetValue(event, args[1]);
	s32 mask = evtGetValue(event, args[2]);
	MapObjectEntry* obj = mobjNameToPtr(name);
	if (onoff == 1) animPoseSetMaterialFlagOn(obj->poseId, mask);
	else animPoseSetMaterialFlagOff(obj->poseId, mask);
	return EVT_RETURN_DONE;
}

USER_FUNC(evt_map_set_playrate) {
	s32* args = event->args;
	s32 playrate = evtGetValue(event, args[0]);
	evtGetFloat(event, args[1]);
	mapSetPlayRate(playrate);
	return EVT_RETURN_DONE;
}

USER_FUNC(evt_map_replayanim) {
	s32* args = event->args;
	s32 type = evtGetValue(event, args[0]);
	evtGetValue(event, args[1]);
	if (type) mapReStartAnimationAll(); else mapReStartAnimation();
	return EVT_RETURN_DONE;
}

USER_FUNC(evt_map_pauseanim) {
	s32* args = event->args;
	s32 type = evtGetValue(event, args[0]);
	evtGetValue(event, args[1]);
	if (type) mapPauseAnimationAll(); else mapPauseAnimation();
	return EVT_RETURN_DONE;
}

USER_FUNC(evt_map_checkanim) {
	s32* args = event->args;
	s32 mapObj;
	s32 animId;
	f32 frame;
	mapObj = evtGetValue(event, args[0]);
	fn_8002A160(mapObj, &animId, &frame);
	evtSetValue(event, args[1], animId);
	evtSetValue(event, args[2], (s32)frame);
	return EVT_RETURN_DONE;
}

USER_FUNC(evt_map_playanim) {
	s32* args = event->args;
	s32 mapObj = evtGetValue(event, args[0]);
	s32 anim = evtGetValue(event, args[1]);
	s32 mode = evtGetValue(event, args[2]);
	fn_80029F18(mapObj, anim, mode);
	return EVT_RETURN_DONE;
}

USER_FUNC(evt_map_blend_off) {
	if (evtGetValue(event, event->args[0]) == 0) mapBlendOff(); else mapBlendOff2();
	return EVT_RETURN_DONE;
}

USER_FUNC(evt_map_fog_onoff) {
	if (evtGetValue(event, event->args[0])) mapFogOn(); else mapFogOff();
	return EVT_RETURN_DONE;
}

USER_FUNC(evt_mapobj_scale) {
	s32* args = event->args;
	s32 type = evtGetValue(event, args[0]);
	const char* name = (const char*)evtGetValue(event, args[1]);
	f32 x = evtGetFloat(event, args[2]);
	f32 y = evtGetFloat(event, args[3]);
	f32 z = evtGetFloat(event, args[4]);
	if (type == 0) mapObjScale(name, x, y, z);
	else mapObjScale(name, x, y, z);
	return EVT_RETURN_DONE;
}

USER_FUNC(evt_mapobj_rotate) {
	s32* args = event->args;
	s32 type = evtGetValue(event, args[0]);
	const char* name = (const char*)evtGetValue(event, args[1]);
	f32 x = evtGetFloat(event, args[2]);
	f32 y = evtGetFloat(event, args[3]);
	f32 z = evtGetFloat(event, args[4]);
	if (type == 0) mapObjRotate(name, x, y, z);
	else mapObjRotate(name, x, y, z);
	return EVT_RETURN_DONE;
}

USER_FUNC(evt_mapobj_trans) {
	s32* args = event->args;
	s32 type = evtGetValue(event, args[0]);
	const char* name = (const char*)evtGetValue(event, args[1]);
	f32 x = evtGetFloat(event, args[2]);
	f32 y = evtGetFloat(event, args[3]);
	f32 z = evtGetFloat(event, args[4]);
	if (type == 0) mapObjTranslate(name, x, y, z);
	else mapObjTranslate(name, x, y, z);
	return EVT_RETURN_DONE;
}

USER_FUNC(evt_mapobj_get_position) {
    s32* args = event->args;
    const char* name = (const char*)evtGetValue(event, args[0]);
    Vec pos;
    mapObjGetPos(name, &pos);
    evtSetValue(event, args[1], (s32)(lbl_8042C4C8 * pos.x) - 0x0DB58580);
    evtSetValue(event, args[2], (s32)(lbl_8042C4C8 * pos.y) - 0x0DB58580);
    evtSetValue(event, args[3], (s32)(lbl_8042C4C8 * pos.z) - 0x0DB58580);
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_mapobj_color) {
    s32* args = event->args;
    s32 type = evtGetValue(event, args[0]);
    const char* name = (const char*)evtGetValue(event, args[1]);
    u8 r = (u8)evtGetValue(event, args[2]);
    u8 g = (u8)evtGetValue(event, args[3]);
    u8 b = (u8)evtGetValue(event, args[4]);
    u8 a = (u8)evtGetValue(event, args[5]);
    GXColor color;
    if (type == 0) {
        color = lbl_80436078;
        color.r = r;
        color.g = g;
        color.b = b;
        color.a = a;
        mapObjSetColor(name, color);
    } else {
        color = lbl_8043607C;
        color.r = r;
        color.g = g;
        color.b = b;
        color.a = a;
        mapGrpSetColor(name, color);
    }
    return EVT_RETURN_DONE;
}
USER_FUNC(evt_map_set_blend) {
    s32* args = event->args;
    s32 type = evtGetValue(event, args[0]);
    s32 r = evtGetValue(event, args[1]);
    s32 g = evtGetValue(event, args[2]);
    s32 b = evtGetValue(event, args[3]);
    s32 a = evtGetValue(event, args[4]);
    if (type == 0) {
        GXColor copy;
        GXColor color = lbl_80436070;
        color.r = (u8)r;
        color.g = (u8)g;
        color.b = (u8)b;
        color.a = (u8)a;
        copy = color;
        mapSetBlend(&copy);
    } else {
        GXColor copy;
        GXColor color = lbl_80436074;
        color.r = (u8)r;
        color.g = (u8)g;
        color.b = (u8)b;
        color.a = (u8)a;
        copy = color;
        mapSetBlend2(&copy);
    }
    return EVT_RETURN_DONE;
}
