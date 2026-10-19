#include "driver/fadedrv.h"
#include "driver/animdrv.h"
#include "driver/arcdrv.h"
#include "driver/camdrv.h"
#include "driver/dispdrv.h"
#include "driver/imgdrv.h"
#include "evt/evt_img.h"
#include "manager/dvdmgr.h"
#include "mario/mario.h"
#include "mario/mariost.h"
#include "memory.h"
#include "pmario_sound.h"
#include "system.h"
#include <dolphin/dvd.h>
#include <dolphin/os.h>
#include <math.h>
#include <stdio.h>
#include <string.h>

#define Evt EvtEntry
#define pCurData args

static inline s32 imgEntryCheck(ImageEntry* image) {
	if (!image) return 1;
	if (image->poseId >= 0) {
		return animPoseGetPeraEnd(image->poseId) ? 1 : 0;
	}
	return 1;
}
#define imgMain(img) imgEntryCheck(img)

extern GlobalWork* gp;
extern const f32 dat_80419498;
extern const f32 dat_8041949c;
extern const char lbl_802CB31C[];
extern const char vec3_802c11a0[];

static u8 i4tex[32] ATTRIBUTE_ALIGN(32) = {0xFF};

static u32 depth_image[32] ATTRIBUTE_ALIGN(32) = {
	0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
	0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
	0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
	0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
	0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
	0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
	0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
	0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
};

FadeWork* lbl_8042AF40;
#define wp lbl_8042AF40
static int entry_n;

// local prototypes
void _callback(s32 error, DVDFileInfo* info);
void disp_texture(CameraId camId, void* param);
f32 z(void);
static void zFill(s32 wXluStage);
void disp_maku(CameraId camId, void* param);
void disp_tec(CameraId camId, void* param);
void disp_dummy_black(CameraId camId, void* param);

void _callback(s32 error, DVDFileInfo* info) {
	void* userData = info->cb.userData;
	UnpackTexPalette(wp->texture);
	DVDMgrClose(userData);
	wp->loaded = TRUE;
}

void fadeInit(void) {
	int i;

	wp = __memAlloc(HEAP_DEFAULT, sizeof(FadeWork));
	memset(wp, 0, sizeof(FadeWork));

	for (i = 0; i < 5; i++) {
		wp->entries[i].field_0x6C = -1;
		wp->entries[i].field_0x70 = -1;

		wp->entries[i].fadeSpotX = 0.0f;
		wp->entries[i].fadeSpotY = 0.0f;

		wp->entries[i].animOffsetX = 0.0f;
		wp->entries[i].animOffsetY = 0.0f;

		wp->entries[i].field_0x90 = 1.0f;

		wp->entries[i].field_0xA4 = 0;
	}

	wp->field_0x350 = -1;
	wp->texture = NULL;
	wp->loaded = FALSE;
}

void fadeTexSetup(void) {
	DVDEntry* dvd;
	u32 size;
	char path[128];

	wp->texture = NULL;
	wp->texture = arcOpen((char*)lbl_802CB31C, NULL, NULL);
	if (wp->texture) {
		goto loaded;
	}
	sprintf(path, vec3_802c11a0, getMarioStDvdRoot());
	dvd = DVDMgrOpen(path, 2, 0);
	size = OSRoundUp32B(DVDMgrGetLength(dvd));
	wp->texture = __memAlloc(HEAP_DEFAULT, size);
	dvd->info.cb.userData = dvd;
	DVDMgrReadAsync(dvd, wp->texture, size, 0, _callback);
	goto done;
loaded:
	UnpackTexPalette(wp->texture);
	wp->loaded = TRUE;
done:
	;
}

void fadeEntry(s32 type, s32 duration, GXColor color) {
	FadeEntry* entry;

	if (!type) {
		return;
	}
	if (type == 16) {
		type = 9;
		color = (GXColor){0xF0, 0xF0, 0xF0, 0xFF};
	}
	if (type == 17) {
		type = 10;
		color = (GXColor){0xF0, 0xF0, 0xF0, 0xFF};
	}
	if (type == 18) {
		type = 11;
		color = (GXColor){0xF0, 0xF0, 0xF0, 0xFF};
	}
	if (type == 19) {
		type = 12;
		color = (GXColor){0xF0, 0xF0, 0xF0, 0xFF};
	}
	if (type == 20) {
		type = 14;
		color = (GXColor){0xF0, 0xF0, 0xF0, 0xFF};
	}
	if (type == 21) {
		type = 15;
		color = (GXColor){0xF0, 0xF0, 0xF0, 0xFF};
	}
	if ((type == 1 || type == 2) && duration < 1000) {
		duration = 1000;
	}
	if (type == 7) {
		type = 3;
	}
	else if (type == 8) {
		type = 4;
	}
	else if (((u32)(type - 3) <= 2 || type == 6) && duration < 1000) {
		duration = 1000;
	}

	switch (type) {
		case 1:
		case 2:
		case 3:
		case 4:
		case 5:
		case 6:
		case 9:
		case 10:
		case 11:
		case 12:
		case 13:
		case 14:
		case 15:
			entry = &wp->entries[0];
			if (entry->field_0x6C >= 0) {
				animPoseRelease(entry->field_0x6C);
				entry->field_0x6C = -1;
			}
			wp->field_0x350 = 0;
			switch (type) {
				case 2:
				case 4:
				case 6:
				case 10:
				case 12:
				case 15:
					if ((entry->flags & 1) && !(entry->flags & 2)) {
						switch (entry->type) {
							case 1:
							case 3:
							case 5:
							case 9:
							case 11:
							case 14:
								entry->flags |= 4;
								break;
							default:
								break;
						}
					}
					break;
				default:
					break;
			}
			break;

		case 22:
		case 23:
			entry = &wp->entries[2];
			if (entry->field_0x6C >= 0) {
				animPoseRelease(entry->field_0x6C);
				entry->field_0x6C = -1;
			}
			entry->field_0x94 = 2;
			if (type == 22) {
				entry->field_0x90 = 0.0f;
			}
			else {
				entry->field_0x90 = 1.0f;
			}
			wp->field_0x350 = 2;
			break;

		case 50:
		case 51:
		case 53:
		case 54:
		case 55:
		case 56:
		case 57:
		case 58:
		case 59:
		case 60:
		case 61:
		case 62:
			entry = &wp->entries[1];
			switch (type) {
				case 51:
				case 54:
				case 55:
				case 57:
				case 59:
					if ((entry->flags & 1) && !(entry->flags & 2)) {
						switch (entry->type) {
							case 50:
							case 53:
							case 56:
							case 58:
								break;
							default:
								entry->flags |= 4;
								break;
						}
					}
					else {
						entry->flags |= 4;
					}
					break;
				default:
					break;
			}
			wp->field_0x350 = 1;
			break;

		case 41:
		case 42:
		case 43:
		case 44:
		case 45:
		case 46:
		case 47:
		case 48:
			entry = &wp->entries[1];
			if (entry->field_0x6C >= 0) {
				animPoseRelease(entry->field_0x6C);
				entry->field_0x6C = -1;
			}
			wp->field_0x350 = 1;
			break;

		case 24:
		case 25:
		case 26:
		case 27:
		case 28:
		case 29:
		case 30:
		case 31:
		case 32:
		case 34:
		case 35:
		case 36:
		case 37:
		case 38:
		case 40:
		case 52:
		case 63:
		case 64:
			entry = &wp->entries[3];
			if (entry->field_0x6C >= 0) {
				animPoseRelease(entry->field_0x6C);
				entry->field_0x6C = -1;
			}
			wp->field_0x350 = 3;
			break;

		case 33:
		case 39:
			entry = &wp->entries[4];
			if (entry->field_0x6C >= 0) {
				animPoseRelease(entry->field_0x6C);
				entry->field_0x6C = -1;
			}
			wp->field_0x350 = 4;
			break;

		default:
			entry = NULL;
			break;
	}

	entry->flags |= 1;
	entry->flags &= ~2;
	entry->type = (FadeType)type;
	entry->field_0x8 = 0;
	entry->field_0xC = 0;
	entry->field_0x10 = (s64)duration;
	entry->color = color;
	entry->field_0x5C = 0.0f;

	switch (type) {
		case 1:
		case 3:
		case 5:
			entry->field_0x24 = 1.0f;
			entry->field_0x28 = 0.0f;
			entry->color = (GXColor){0x19, 0x23, 0x2A, 0xFF};
			entry->field_0xA4 = 5;
			break;
		case 2:
		case 4:
		case 6:
			entry->field_0x24 = 0.0f;
			entry->field_0x28 = 0.0f;
			entry->color = (GXColor){0x19, 0x23, 0x2A, 0xFF};
			break;
		case 9:
			entry->field_0x24 = 1.0f;
			entry->color = (GXColor){0x00, 0x00, 0x00, 0xFF};
			entry->field_0xA4 = 5;
			break;
		case 10:
			entry->field_0x24 = 0.0f;
			entry->color = (GXColor){0x00, 0x00, 0x00, 0xFF};
			break;
		case 11:
			entry->field_0x24 = 1.0f;
			entry->field_0xA4 = 5;
			break;
		case 12:
		case 15:
			entry->field_0x24 = 0.0f;
			break;
		case 13:
			entry->field_0x24 = 1.0f;
			break;
		case 14:
			entry->field_0x24 = 1.0f;
			entry->field_0xA4 = 5;
			break;
		case 22:
			entry->field_0x60 = "OFF_d_maku_left_right";
			entry->field_0x64 = "A_1";
			entry->imageType = 1;
			break;
		case 23:
			entry->field_0x60 = "OFF_d_maku_left_right";
			entry->field_0x64 = "A_2";
			entry->imageType = 1;
			break;
		case 24:
		case 63:
			entry->field_0x60 = "OFF_d_maku_up_down";
			entry->field_0x64 = "Z_1";
			entry->imageType = 2;
			break;
		case 25:
		case 64:
			entry->field_0x60 = "OFF_d_maku_up_down";
			entry->field_0x64 = "Z_2";
			entry->imageType = 2;
			break;
		case 26:
		case 52:
			entry->field_0x60 = "OFF_d_maku_left_right";
			entry->field_0x64 = "Z_1";
			entry->imageType = 2;
			break;
		case 27:
			entry->field_0x60 = "OFF_d_maku_left_right";
			entry->field_0x64 = "Z_2";
			entry->imageType = 2;
			break;
		case 28:
			entry->field_0x60 = "OFF_d_maku_center";
			entry->field_0x64 = "Z_1";
			entry->imageType = 2;
			break;
		case 29:
			entry->field_0x60 = "OFF_d_maku_center";
			entry->field_0x64 = "Z_2";
			entry->imageType = 2;
			break;
		case 30:
			entry->field_0x60 = "OFF_d_meku";
			entry->field_0x64 = "Z_1";
			entry->imageType = 2;
			break;
		case 31:
			entry->field_0x60 = "OFF_d_meku";
			entry->field_0x64 = "Z_2";
			entry->imageType = 2;
			break;
		case 32:
			entry->field_0x60 = "OFF_d_four_meku_5sec";
			entry->field_0x64 = "Z_1";
			entry->imageType = 2;
			break;
		case 34:
			entry->field_0x60 = "OFF_d_maku_left_right";
			entry->field_0x64 = "A_1";
			entry->imageType = 2;
			break;
		case 35:
			entry->field_0x60 = "OFF_d_maku_left_right";
			entry->field_0x64 = "A_2";
			entry->imageType = 2;
			break;
		case 36:
			entry->field_0x60 = "OFF_d_maku_up_down";
			entry->field_0x64 = "A_3";
			entry->imageType = 2;
			break;
		case 37:
			entry->field_0x60 = "OFF_d_maku_up_down";
			entry->field_0x64 = "A_1";
			entry->imageType = 2;
			break;
		case 38:
			entry->field_0x60 = "OFF_d_maku_up_down";
			entry->field_0x64 = "A_2";
			entry->imageType = 2;
			break;
		case 40:
			entry->field_0x60 = "OFF_f_in_out_mario";
			entry->field_0x64 = "Z_1";
			entry->imageType = 2;
			break;
		case 41:
			entry->field_0x60 = "OFF_d_dokan_down";
			entry->field_0x64 = "A_1";
			entry->imageType = 0;
			break;
		case 42:
			entry->field_0x60 = "OFF_d_dokan_down";
			entry->field_0x64 = "A_2";
			entry->imageType = 0;
			break;
		case 43:
			entry->field_0x60 = "OFF_d_dokan_up";
			entry->field_0x64 = "A_1";
			entry->imageType = 0;
			break;
		case 44:
			entry->field_0x60 = "OFF_d_dokan_up";
			entry->field_0x64 = "A_2";
			entry->imageType = 0;
			break;
		case 45:
			entry->field_0x60 = "OFF_d_dokan_left";
			entry->field_0x64 = "A_1";
			entry->imageType = 0;
			break;
		case 46:
			entry->field_0x60 = "OFF_d_dokan_left";
			entry->field_0x64 = "A_2";
			entry->imageType = 0;
			break;
		case 47:
			entry->field_0x60 = "OFF_d_dokan_right";
			entry->field_0x64 = "A_1";
			entry->imageType = 0;
			break;
		case 48:
			entry->field_0x60 = "OFF_d_dokan_right";
			entry->field_0x64 = "A_2";
			entry->imageType = 0;
			break;
		case 49:
			entry->field_0x60 = "OFF_f_in_out_mario";
			entry->field_0x64 = "A_1";
			entry->imageType = 1;
			break;
		case 50:
			entry->field_0x60 = "OFF_f_in_out_mario";
			entry->field_0x64 = "A_2";
			entry->imageType = 1;
			break;
		case 51:
			entry->field_0x60 = "OFF_f_in_out_peach";
			entry->field_0x64 = "Z_1";
			entry->imageType = 2;
			break;
		case 53:
			entry->field_0x60 = "OFF_f_in_out_peach";
			entry->field_0x64 = "A_1";
			entry->imageType = 1;
			break;
		case 54:
			entry->field_0x60 = "OFF_f_in_out_peach";
			entry->field_0x64 = "A_2";
			entry->imageType = 1;
			break;
		case 55:
			entry->field_0x60 = "OFF_f_in_out_kuppa";
			entry->field_0x64 = "Z_1";
			entry->imageType = 2;
			break;
		case 56:
			entry->field_0x60 = "OFF_f_in_out_kuppa";
			entry->field_0x64 = "A_1";
			entry->imageType = 1;
			break;
		case 57:
			entry->field_0x60 = "OFF_f_in_out_kuppa";
			entry->field_0x64 = "A_2";
			entry->imageType = 1;
			break;
		case 58:
			entry->field_0x60 = "OFF_f_in_out_mario";
			entry->field_0x64 = "A_1";
			entry->imageType = 2;
			break;
		case 59:
			entry->field_0x60 = "OFF_f_in_out_mario";
			entry->field_0x64 = "A_2";
			entry->imageType = 2;
			break;
		case 60:
			entry->field_0x60 = "OFF_f_in_out_peach";
			entry->field_0x64 = "A_2";
			entry->imageType = 2;
			break;
		case 61:
			entry->field_0x60 = "OFF_f_in_out_kuppa";
			entry->field_0x64 = "A_2";
			entry->imageType = 2;
			break;
		default:
			break;
	}
}

void fadeTecOn(void) {
	wp->flags |= 0x1000;
}

void fadeTecOff(void) {
	wp->flags &= ~0x1000;
}

void fadeTecSoftOn(void) {
	wp->flags |= 0x1010;
	wp->field_0x354 = dat_80419498;
}

void fadeTecSoftOff(void) {
	wp->flags |= 0x1020;
	wp->field_0x354 = dat_8041949c;
}

void fadeSoftFocusOn(void) {
	wp->flags |= 0x2000;
}

void fadeSoftFocusOff(void) {
	wp->flags &= ~0x2000;
}

void fadeWhiteOutOn(void) {
	wp->flags |= 0x4000;
}

void fadeWhiteOutOff(void) {
	wp->flags &= ~0x4000;
}

void fadeSetSpotPos(f32 x, f32 y) {
	FadeWork* fwp = wp;
	fwp->entries[0].fadeSpotX = x;
	fwp->entries[0].fadeSpotY = y;
}

void fadeSetAnimOfsPos(f32 x, f32 y) {
	FadeWork* fwp = wp;
	fwp->entries[1].animOffsetX = x;
	fwp->entries[1].animOffsetY = y;
}

void fadeSetAnimVirtualPos(f32 x, f32 y, f32 z_pos) {
	FadeEntry* entry = &wp->entries[1];
	entry->VirtualPos = (Vec){x, y, z_pos};
	entry->flags |= 4;
}

void fadeSetOffscreenCallback(OffscreenCallback callback, void* param) {
	FadeWork* fwp = wp;
	fwp->entries[3].callback = callback;
	fwp->entries[3].offparam = param;
}

void fadeMain(void) {
	int i;
	FadeEntry* entry;

	for (i = 0, entry = &wp->entries[0]; i < 5; i++, entry++) {
		if (!(entry->flags & 1)) {
			continue;
		}
		switch (entry->type) {
			case 1:
			case 3:
			case 5:
			case 9:
			case 11:
			case 14:
				if (entry->field_0x8 == 0) {
					if (entry->field_0x10 != 0) {
						if (entry->field_0xA4 != 0) {
							entry->field_0xA4--;
						}
						else {
							entry->field_0x5C += 1000.0f / (f32)(s32)gp->framerate;
						}
						entry->field_0x24 = 1.0f - (entry->field_0x5C / (f32)entry->field_0x10);
						entry->field_0x28 += 10.0f;
						if (entry->field_0x24 < 0.0f) {
							entry->field_0x24 = 0.0f;
							entry->flags |= 2;
							entry->field_0x8++;
						}
					}
					else {
						entry->field_0x24 = 0.0f;
						entry->flags |= 2;
						entry->field_0x8++;
					}
				}
				break;

			case 2:
			case 4:
			case 6:
			case 10:
			case 12:
			case 13:
			case 15:
				if (entry->field_0x8 == 0) {
					if (entry->field_0x10 != 0) {
						entry->field_0x5C += 1000.0f / (f32)(s32)gp->framerate;
						entry->field_0x24 = entry->field_0x5C / (f32)entry->field_0x10;
						entry->field_0x28 -= 10.0f;
						if (entry->field_0x24 > 1.0f) {
							entry->field_0x24 = 1.0f;
							entry->flags |= 2;
							entry->field_0x8++;
						}
					}
					else {
						entry->field_0x24 = 1.0f;
						entry->flags |= 2;
						entry->field_0x8++;
					}
				}
				break;

			case 41:
			case 42:
			case 43:
			case 44:
			case 45:
			case 46:
			case 47:
			case 48:
				if (entry->field_0x8 == 0) {
					if (!animGroupBaseAsync(entry->field_0x60, entry->imageType, NULL)) {
						if (entry->field_0x70 < 0) {
							if (entry->type == 42 || entry->type == 44 || entry->type == 46 || entry->type == 48) {
								wp->flags |= 0x8000;
							}
						}
					}
					else {
						wp->flags &= ~0x8000;
						if (entry->field_0x70 >= 0) {
							imgRelease(entry->image, entry->imageType);
							animPaperPoseRelease(entry->field_0x70);
							entry->field_0x70 = -1;
							entry->image = NULL;
						}
						entry->field_0x70 = animPaperPoseEntry(entry->field_0x60, entry->imageType);
						imgEntry("fImg_dokan", entry->imageType);
						entry->image = imgNameToPtr("fImg_dokan", entry->imageType);
						entry->image->paperPoseId = animPaperPoseGetId(entry->field_0x60, entry->imageType);
						entry->image->time = animTimeGetTime(entry->imageType);
						entry->image->animName = entry->field_0x64;
						imgClearVirtualPoint(entry->image);
						{
							Evt dummyEvt;
							s32 args[8];
							memset(&dummyEvt, 0, sizeof(dummyEvt));
							args[0] = (s32)(intptr_t)"fImg_dokan";
							args[1] = 0;
							args[2] = 0;
							args[3] = 0;
							args[4] = 0;
							args[5] = 0;
							args[6] = 608;
							args[7] = 480;
							dummyEvt.pCurData = args;
							evt_img_alloc_capture(&dummyEvt, 1);
						}
						switch (entry->type) {
							case 41:
								entry->field_0x98 = psndSFXOn((const char*)0x403A9);
								break;
							case 42:
								entry->field_0x98 = psndSFXOn((const char*)0x403AA);
								break;
							case 43:
								entry->field_0x98 = psndSFXOn((const char*)0x403A9);
								break;
							case 44:
								entry->field_0x98 = psndSFXOn((const char*)0x403AA);
								break;
							case 45:
								entry->field_0x98 = psndSFXOn((const char*)0x403AB);
								break;
							case 46:
								entry->field_0x98 = psndSFXOn((const char*)0x403AC);
								break;
							case 47:
								entry->field_0x98 = psndSFXOn((const char*)0x403AB);
								break;
							case 48:
								entry->field_0x98 = psndSFXOn((const char*)0x403AC);
								break;
							default:
								break;
						}
						entry->field_0x8++;
					}
				}
				else if (entry->field_0x8 == 1) {
					if (imgMain(entry->image) == 1) {
						switch (entry->type) {
							case 41:
							case 43:
							case 45:
							case 47:
								entry->image->flags |= 0x2000;
								entry->flags |= 2;
								break;
							case 42:
							case 44:
							case 46:
							case 48:
								entry->flags &= ~1;
								entry->flags |= 2;
								break;
							default:
								break;
						}
						entry->field_0x8++;
					}
				}
				break;

			case 49:
			case 50:
			case 52:
			case 55:
			case 57:
			case 59:
			case 60:
			case 61:
			case 62:
				if (entry->field_0x8 == 0) {
					if (animGroupBaseAsync(entry->field_0x60, entry->imageType, NULL)) {
						if (entry->field_0x6C >= 0) {
							animPoseRelease(entry->field_0x6C);
							entry->field_0x6C = -1;
						}
						entry->field_0x6C = animPoseEntry(entry->field_0x60, entry->imageType);
						animPoseSetAnim(entry->field_0x6C, entry->field_0x64, 1);
						animPoseSetMaterialFlagOn(entry->field_0x6C, 0x1800);
						animPoseSetGXFunc(entry->field_0x6C, zFill, 0);
						if ((entry->flags & 4) && !(entry->flags & 8)) {
							entry->field_0x24 = 0.0f;
							entry->field_0x8++;
							if (entry->type == 50) {
								entry->field_0x98 = psndSFXOn((const char*)0x1013F);
							}
							else if (entry->type == 51) {
								entry->field_0x98 = psndSFXOn((const char*)0x10140);
							}
						}
						else {
							entry->field_0x24 = 13.9497f;
							entry->field_0x8 += 2;
							switch (entry->type) {
								case 49:
									entry->field_0x98 = psndSFXOn((const char*)0x10140);
									break;
								case 52:
								case 55:
								case 57:
									entry->field_0x98 = psndSFXOn((const char*)0x1013F);
									break;
								case 59:
								case 60:
								case 61:
									entry->field_0x98 = psndSFXOn((const char*)0x2009C);
									break;
								default:
									break;
							}
						}
						switch (entry->type) {
							case 49:
							case 50:
							case 51:
							case 52:
							case 53:
							case 54:
							case 55:
							case 56:
							case 57:
								entry->field_0x94 = 1;
								break;
							default:
								entry->field_0x94 = 2;
								break;
						}
					}
				}
				else if (entry->field_0x8 == 1) {
					animPoseMain(entry->field_0x6C);
					entry->field_0x24 += 1.0f;
					if (entry->field_0x24 >= 13.9497f) {
						entry->flags |= 2;
						entry->field_0x8 += 2;
						entry->flags &= ~4;
					}
				}
				else if (entry->field_0x8 == 2) {
					if (entry->flags & 8) {
						entry->field_0x24 += 3.0f;
					}
					else if (entry->flags & 4) {
						entry->field_0x24 += 1.5f;
					}
					else {
						entry->field_0x24 += 1.0f;
					}
					animPoseMain(entry->field_0x6C);
					if (animPoseGetLoopTimes(entry->field_0x6C) >= 999.0f) {
						if (entry->type == 59 || entry->type == 60 || entry->type == 61) {
							entry->flags &= ~1;
						}
						entry->flags |= 2;
						entry->field_0x8++;
						entry->flags &= ~4;
						entry->flags &= ~8;
					}
				}
				break;

			case 51:
			case 53:
			case 54:
			case 56:
			case 58:
				if (entry->field_0x8 == 0) {
					if (!animGroupBaseAsync(entry->field_0x60, entry->imageType, NULL)) {
						if (entry->field_0x6C < 0) {
							wp->flags |= 0x8000;
						}
					}
					else {
						wp->flags &= ~0x8000;
						if (entry->field_0x6C >= 0) {
							animPoseRelease(entry->field_0x6C);
							entry->field_0x6C = -1;
						}
						entry->field_0x6C = animPoseEntry(entry->field_0x60, entry->imageType);
						animPoseSetAnim(entry->field_0x6C, entry->field_0x64, 1);
						animPoseSetMaterialFlagOn(entry->field_0x6C, 0x1800);
						animPoseSetGXFunc(entry->field_0x6C, zFill, 0);
						entry->field_0x24 = 0.0f;
						entry->field_0x8++;
						switch (entry->type) {
							case 50:
							case 53:
							case 56:
								entry->field_0x98 = psndSFXOn((const char*)0x1013F);
								break;
							case 54:
							case 58:
							case 60:
							case 62:
								entry->field_0x98 = psndSFXOn((const char*)0x10140);
								break;
							default:
								break;
						}
						entry->field_0x94 = 1;
					}
				}
				else if (entry->field_0x8 == 1) {
					entry->field_0x24 += 1.0f;
					animPoseMain(entry->field_0x6C);
					if (animPoseGetLoopTimes(entry->field_0x6C) >= 999.0f) {
						animPoseRelease(entry->field_0x6C);
						entry->field_0x6C = -1;
						entry->flags &= ~1;
						entry->flags |= 2;
						entry->field_0x8++;
						entry->flags &= ~4;
					}
				}
				break;

			case 22:
				if (entry->field_0x8 == 0) {
					if (animGroupBaseAsync(entry->field_0x60, entry->imageType, NULL)) {
						entry->field_0x6C = animPoseEntry(entry->field_0x60, entry->imageType);
						animPoseSetAnim(entry->field_0x6C, entry->field_0x64, 1);
						entry->field_0x24 = 0.0f;
						entry->field_0x8++;
					}
				}
				else if (entry->field_0x8 == 1) {
					entry->field_0x24 += 1.0f;
					animPoseMain(entry->field_0x6C);
					entry->field_0x90 += 0.04f;
					if (entry->field_0x90 > 1.0f) {
						entry->field_0x90 = 1.0f;
					}
					if (animPoseGetLoopTimes(entry->field_0x6C) >= 999.0f) {
						entry->flags |= 2;
						entry->field_0x8++;
					}
				}
				break;

			case 23:
				if (entry->field_0x8 == 0) {
					if (!animGroupBaseAsync(entry->field_0x60, entry->imageType, NULL)) {
						if (entry->field_0x6C < 0) {
							wp->flags |= 0x8000;
						}
					}
					else {
						wp->flags &= ~0x8000;
						if (entry->field_0x6C >= 0) {
							animPoseRelease(entry->field_0x6C);
							entry->field_0x6C = -1;
						}
						entry->field_0x6C = animPoseEntry(entry->field_0x60, entry->imageType);
						animPoseSetAnim(entry->field_0x6C, entry->field_0x64, 1);
						entry->field_0x24 = 0.0f;
						entry->field_0x8++;
					}
				}
				else if (entry->field_0x8 == 1) {
					entry->field_0x24 += 1.0f;
					animPoseMain(entry->field_0x6C);
					if (entry->field_0x24 >= 24.0f) {
						entry->field_0x90 -= 0.04f;
						if (entry->field_0x90 < 0.0f) {
							entry->field_0x90 = 0.0f;
						}
					}
					if (animPoseGetLoopTimes(entry->field_0x6C) >= 999.0f) {
						animPoseRelease(entry->field_0x6C);
						entry->field_0x6C = -1;
						entry->flags &= ~1;
						entry->flags |= 2;
						entry->field_0x8++;
					}
				}
				else if (entry->field_0x8 == 2) {
					if (!animGroupBaseAsync(entry->field_0x60, entry->imageType, NULL)) {
						if (entry->field_0x6C < 0) {
							wp->flags |= 0x8000;
						}
					}
					else {
						wp->flags &= ~0x8000;
						if (entry->field_0x6C >= 0) {
							animPoseRelease(entry->field_0x6C);
							entry->field_0x6C = -1;
						}
						entry->field_0x6C = animPoseEntry(entry->field_0x60, entry->imageType);
						animPoseSetAnim(entry->field_0x6C, entry->field_0x64, 1);
						entry->field_0x24 = 0.0f;
						entry->field_0x8++;
					}
				}
				else if (entry->field_0x8 == 3) {
					entry->field_0x24 += 1.0f;
					animPoseMain(entry->field_0x6C);
					if (entry->field_0x24 >= 24.0f) {
						entry->field_0x90 -= 0.04f;
						if (entry->field_0x90 < 0.0f) {
							entry->field_0x90 = 0.0f;
						}
					}
					if (animPoseGetLoopTimes(entry->field_0x6C) >= 999.0f) {
						animPoseRelease(entry->field_0x6C);
						entry->field_0x6C = -1;
						entry->flags &= ~1;
						entry->flags |= 2;
						entry->field_0x8++;
					}
				}
				break;

			case 24:
			case 25:
			case 26:
			case 27:
			case 28:
			case 29:
			case 30:
			case 31:
			case 32:
			case 34:
			case 35:
			case 36:
			case 37:
			case 38:
			case 40:
			case 63:
			case 64:
				if (entry->field_0x8 == 0) {
					if (animGroupBaseAsync(entry->field_0x60, entry->imageType, NULL)) {
						switch (entry->type) {
							case 24:
							case 27:
							case 63:
								entry->field_0x98 = psndSFXOn((const char*)0x20137);
								break;
							case 25:
							case 28:
							case 64:
								entry->field_0x98 = psndSFXOn((const char*)0x20138);
								break;
							case 29:
							case 35:
								entry->field_0x98 = psndSFXOn((const char*)0x20139);
								break;
							case 30:
							case 36:
								entry->field_0x98 = psndSFXOn((const char*)0x2013A);
								break;
							default:
								break;
						}
						if (entry->field_0x70 >= 0) {
							imgRelease(entry->image, entry->imageType);
							animPaperPoseRelease(entry->field_0x70);
							entry->field_0x70 = -1;
							entry->image = NULL;
						}
						entry->field_0x70 = animPaperPoseEntry(entry->field_0x60, entry->imageType);
						if (entry->type == 24 || entry->type == 25 || entry->type == 63 || entry->type == 64) {
							imgEntry("fOff_peron", entry->imageType);
							entry->image = imgNameToPtr("fOff_peron", entry->imageType);
						}
						else {
							imgEntry("fImg_peron", entry->imageType);
							entry->image = imgNameToPtr("fImg_peron", entry->imageType);
						}
						entry->image->paperPoseId = animPaperPoseGetId(entry->field_0x60, entry->imageType);
						entry->image->time = animTimeGetTime(entry->imageType);
						entry->image->animName = entry->field_0x64;
						if (entry->type != 24 && entry->type != 25 && entry->type != 63 && entry->type != 64) {
							Evt dummyEvt;
							s32 args[8];
							memset(&dummyEvt, 0, sizeof(dummyEvt));
							args[0] = (s32)(intptr_t)"fImg_peron";
							args[1] = 0;
							args[2] = 0;
							args[3] = 0;
							args[4] = 0;
							args[5] = 0;
							args[6] = 608;
							args[7] = 480;
							dummyEvt.pCurData = args;
							evt_img_alloc_capture(&dummyEvt, 1);
							if (entry->type == 34 || entry->type == 37) {
								entry->image->flags |= 0x10;
							}
							else if (entry->type == 35 || entry->type == 38) {
								entry->image->flags |= 0x20;
							}
							if (entry->type == 36 || entry->type == 37 || entry->type == 38) {
								Evt shadowEvt;
								s32 sargs[10];
								memset(&shadowEvt, 0, sizeof(shadowEvt));
								sargs[0] = (s32)(intptr_t)"fImg_peron";
								sargs[1] = 1;
								sargs[2] = 0;
								sargs[3] = 0;
								sargs[4] = 0;
								sargs[5] = 608;
								sargs[6] = 480;
								sargs[7] = 0;
								sargs[8] = -100;
								sargs[9] = 0;
								shadowEvt.pCurData = sargs;
								evt_img_set_paper_anim(&shadowEvt, 1);
							}
						}
						else {
							imgSetCallback(entry->image, entry->callback, entry->offparam);
						}
						entry->field_0x8++;
					}
				}
				else if (entry->field_0x8 == 1) {
					if (imgMain(entry->image) == 1) {
						switch (entry->type) {
							case 34:
							case 36:
							case 37:
								entry->image->flags |= 0x2000;
								entry->flags |= 2;
								break;
							default:
								imgRelease(entry->image, entry->imageType);
								animPaperPoseRelease(entry->field_0x70);
								entry->field_0x70 = -1;
								entry->image = NULL;
								entry->flags &= ~1;
								entry->flags |= 2;
								break;
						}
						entry->field_0x8++;
					}
				}
				break;

			case 33:
			case 39:
				if (entry->image != NULL) {
					imgRelease(entry->image, entry->imageType);
					animPaperPoseRelease(entry->field_0x70);
					entry->field_0x70 = -1;
					entry->image = NULL;
				}
				else {
					FadeEntry* e3 = &wp->entries[3];
					if (e3->image != NULL) {
						imgRelease(e3->image, e3->imageType);
						animPaperPoseRelease(e3->field_0x70);
						e3->field_0x70 = -1;
						e3->image = NULL;
						e3->flags &= ~1;
						e3->flags |= 2;
					}
				}
				entry->flags &= ~1;
				entry->flags |= 2;
				break;

			default:
				break;
		}

		if (entry->flags & 1) {
			switch (entry->type) {
				case 1:
				case 2:
				case 3:
				case 4:
				case 5:
				case 6:
				case 9:
				case 10:
				case 11:
				case 12:
				case 13:
				case 14:
				case 15:
					dispEntry(CAMERA_FADE, 0.0f, 0, disp_texture, wp);
					break;
				case 22:
				case 23:
				case 49:
				case 50:
				case 51:
				case 52:
				case 53:
				case 54:
				case 55:
				case 56:
				case 57:
				case 58:
				case 59:
				case 60:
				case 61:
				case 62:
					if (entry->field_0x6C >= 0) {
						switch (entry->field_0x94) {
							case 0:
								dispEntry(CAMERA_3D_IMAGE, 0.0f, 0, disp_maku, entry);
								break;
							case 1:
								dispEntry(CAMERA_2D, 0.0f, 0, disp_maku, entry);
								break;
							case 2:
								dispEntry(CAMERA_FADE, 0.0f, 0, disp_maku, entry);
								break;
						}
					}
					break;
				default:
					break;
			}
		}
	}

	if (wp->flags & 0x7000) {
		if (wp->flags & 0x10) {
			wp->field_0x354 += 0.04f;
			if (wp->field_0x354 > 1.0f) {
				wp->field_0x354 = 1.0f;
				wp->flags &= ~0x10;
			}
		}
		if (wp->flags & 0x20) {
			wp->field_0x354 -= 0.04f;
			if (wp->field_0x354 < 0.0f) {
				wp->field_0x354 = 0.0f;
				wp->flags &= ~0x1020;
			}
		}
		dispEntry(CAMERA_3D_EFFECTB, 0.0f, 0, disp_tec, wp);
	}

	if (wp->flags & 0x8000) {
		dispEntry(CAMERA_2D, 0.0f, 0, disp_dummy_black, wp);
	}
}

void disp_texture(CameraId camId, void* param) {
	CameraEntry* cam = camGetPtr(CAMERA_2D);
	FadeWork* fwp = wp;
	f32 projSave[7];
	GXTexObj texObj;
	GXColor tevColor;

	extern int seqGetSeq(void);
	int curSeq = seqGetSeq();
	if (curSeq == 2 || curSeq == 3) {
		if (fwp->entries[0].flags & 2) {
			return;
		}
	}

	switch (fwp->entries[0].type) {
		case 1:
		case 3:
		case 5:
		case 9:
		case 10:
		case 11:
		case 14:
			if (fwp->entries[0].flags & 2) {
				return;
			}
			break;
		default:
			break;
	}

	GXGetProjectionv(projSave);
	GXSetProjection(cam->projection, cam->projType);
	GXSetZCompLoc(GX_TRUE);
	GXSetAlphaCompare(GX_ALWAYS, 0, GX_AOP_AND, GX_ALWAYS, 0);
	GXSetBlendMode(GX_BM_BLEND, GX_BL_SRCALPHA, GX_BL_INVSRCALPHA, GX_LO_SET);
	GXSetZMode(GX_FALSE, GX_ALWAYS, GX_FALSE);
	GXSetNumChans(0);
	GXSetNumTevStages(1);
	GXSetNumTexGens(1);
	GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR_NULL);
	GXSetTevColorOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
	GXSetTevAlphaOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
	GXSetTevColorIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_C0, GX_CC_TEXC, GX_CC_ZERO);
	GXSetTevAlphaIn(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_A0, GX_CA_TEXA, GX_CA_ZERO);
	GXSetCullMode(GX_CULL_BACK);
	GXClearVtxDesc();
	GXSetVtxDesc(GX_VA_POS, GX_DIRECT);
	GXSetVtxDesc(GX_VA_TEX0, GX_DIRECT);
	GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_POS, GX_POS_XYZ, GX_F32, 0);
	GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_TEX0, GX_TEX_ST, GX_F32, 0);

	if (fwp->entries[0].type >= 1 && fwp->entries[0].type < 3) {
		GXLoadPosMtxImm(cam->view, GX_PNMTX0);
		GXSetCurrentMtx(GX_PNMTX0);
		TEXGetGXTexObjFromPalette(wp->texture, &texObj, 0);
		GXLoadTexObj(&texObj, GX_TEXMAP0);
		if (!(fwp->entries[0].flags & 2)) {
			Mtx mTrans1, mScale, mTrans2;
			f32 t = fwp->entries[0].field_0x24;
			f32 s = 100.0f * t * t * t * t * t;
			PSMTXTrans(mTrans1, 0.5f, 0.5f, 0.0f);
			PSMTXScale(mScale, s, s, 1.0f);
			PSMTXTrans(mTrans2, -0.5f, -0.5f, 0.0f);
			PSMTXConcat(mTrans1, mScale, mScale);
			PSMTXConcat(mScale, mTrans2, fwp->entries[0].field_0x2C);
		}
		else if (fwp->entries[0].type == 2) {
			PSMTXTrans(fwp->entries[0].field_0x2C, 1.0f, 1.0f, 0.0f);
		}
	}
	else if (fwp->entries[0].type >= 3 && fwp->entries[0].type < 7) {
		GXLoadPosMtxImm(cam->view, GX_PNMTX0);
		GXSetCurrentMtx(GX_PNMTX0);
		TEXGetGXTexObjFromPalette(wp->texture, &texObj, 1);
		GXLoadTexObj(&texObj, GX_TEXMAP0);
		if (!(fwp->entries[0].flags & 2)) {
			Mtx mTrans1, mScale, mRot, mTrans2;
			f32 t = fwp->entries[0].field_0x24;
			f32 s = 100.0f * t * t * t * t * t;
			PSMTXTrans(mTrans1, 0.5f, 0.5f, 0.0f);
			PSMTXScale(mScale, s, s, 1.0f);
			PSMTXRotRad(mRot, 'z', 0.017453292f * fwp->entries[0].field_0x28);
			PSMTXTrans(mTrans2,
			           -0.5f - (fwp->entries[0].fadeSpotX / (f32)gp->fbWidth),
			           -0.5f + (fwp->entries[0].fadeSpotY / (f32)gp->fbHeight),
			           0.0f);
			PSMTXConcat(mTrans1, mScale, mScale);
			PSMTXConcat(mScale, mRot, mScale);
			PSMTXConcat(mScale, mTrans2, fwp->entries[0].field_0x2C);
		}
		else if (fwp->entries[0].type == 4 || fwp->entries[0].type == 6) {
			PSMTXTrans(fwp->entries[0].field_0x2C, 1.0f, 1.0f, 0.0f);
		}
	}
	else if (fwp->entries[0].type == 13) {
		GXLoadPosMtxImm(cam->view, GX_PNMTX0);
		GXSetCurrentMtx(GX_PNMTX0);
		GXInitTexObj(&texObj, i4tex, 1, 1, GX_TF_I4, GX_CLAMP, GX_CLAMP, GX_FALSE);
		GXLoadTexObj(&texObj, GX_TEXMAP0);
		PSMTXIdentity(fwp->entries[0].field_0x2C);
	}
	else if (fwp->entries[0].type >= 9 && fwp->entries[0].type < 16) {
		GXLoadPosMtxImm(cam->view, GX_PNMTX0);
		GXSetCurrentMtx(GX_PNMTX0);
		GXInitTexObj(&texObj, i4tex, 1, 1, GX_TF_I4, GX_CLAMP, GX_CLAMP, GX_FALSE);
		GXLoadTexObj(&texObj, GX_TEXMAP0);
		PSMTXIdentity(fwp->entries[0].field_0x2C);
		fwp->entries[0].color.a = (u8)(s32)(255.0f * fwp->entries[0].field_0x24);
	}

	GXLoadTexMtxImm(fwp->entries[0].field_0x2C, GX_TEXMTX0, GX_MTX2x4);
	GXSetTexCoordGen2(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, GX_TEXMTX0, GX_FALSE, GX_PTIDENTITY);
	tevColor = fwp->entries[0].color;
	GXSetTevColor(GX_TEVREG0, tevColor);

	GXBegin(GX_QUADS, GX_VTXFMT0, 4);
	GXPosition3f32(-304.0f, 304.0f, 0.0f);
	GXTexCoord2f32(0.0f, 0.0f);
	GXPosition3f32(304.0f, 304.0f, 0.0f);
	GXTexCoord2f32(1.0f, 0.0f);
	GXPosition3f32(304.0f, -304.0f, 0.0f);
	GXTexCoord2f32(1.0f, 1.0f);
	GXPosition3f32(-304.0f, -304.0f, 0.0f);
	GXTexCoord2f32(0.0f, 1.0f);
	GXEnd();

	GXSetProjectionv(projSave);
}

f32 z(void) {
	CameraEntry* cam3d = camGetPtr(CAMERA_3D);
	CameraEntry* camFade;
	Vec vFade, v3d;
	f32 zFade;

	if (cam3d->projType == GX_PERSPECTIVE) {
		return 1.0f;
	}
	camFade = camGetPtr(CAMERA_FADE);
	PSMTXMultVec(camFade->view, &wp->entries[1].VirtualPos, &vFade);
	zFade = camFade->viewport[5] + ((camFade->viewport[5] - camFade->viewport[4]) * ((1.0f / -vFade.z) * ((camFade->projection[2][2] * vFade.z) + camFade->projection[2][3])));

	cam3d = camGetPtr(CAMERA_3D);
	PSMTXMultVec(cam3d->view, &wp->entries[1].VirtualPos, &v3d);
	return (cam3d->viewport[5] + ((cam3d->viewport[5] - cam3d->viewport[4]) * (1.0f * ((cam3d->projection[2][2] * v3d.z) + cam3d->projection[2][3])))) / zFade;
}

static void zFill(s32 wXluStage) {
	CameraEntry* cam;
	f32 halfDist;
	f32 top;
	f32 right;
	f32 bottom;
	f32 left;
	GXTexObj texObj;
	Mtx m;

	cam = camGetCurPtr();
	halfDist = 0.5f * (cam->near + cam->far);
	top = halfDist * (f32)tan(0.5f * ((6.2831855f * cam->fovY) / 360.0f));
	right = top * cam->aspect;
	bottom = -top;
	left = -right;

	GXInitTexObj(&texObj, depth_image, 4, 4, GX_TF_Z8, GX_CLAMP, GX_CLAMP, GX_FALSE);
	GXLoadTexObj(&texObj, GX_TEXMAP0);
	GXSetNumTexGens(1);
	GXSetTexCoordGen2(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, GX_IDENTITY, GX_FALSE, GX_PTIDENTITY);
	GXSetNumTevStages(1);
	GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR_NULL);
	GXSetTevOp(GX_TEVSTAGE0, GX_PASSCLR);
	GXSetZTexture(GX_ZT_REPLACE, GX_TF_Z8, 0);
	GXSetNumChans(0);
	GXSetAlphaCompare(GX_ALWAYS, 0, GX_AOP_OR, GX_ALWAYS, 0);
	GXSetZCompLoc(GX_TRUE);
	GXSetBlendMode(GX_BM_LOGIC, GX_BL_ONE, GX_BL_ZERO, GX_LO_COPY);
	GXSetZMode(GX_TRUE, GX_ALWAYS, GX_TRUE);
	GXSetColorUpdate(GX_FALSE);
	GXSetAlphaUpdate(GX_FALSE);
	GXSetCullMode(GX_CULL_NONE);
	GXClearVtxDesc();
	GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_POS, GX_POS_XYZ, GX_F32, 0);
	GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_TEX0, GX_TEX_ST, GX_U8, 0);
	GXSetVtxDesc(GX_VA_POS, GX_DIRECT);
	GXSetVtxDesc(GX_VA_TEX0, GX_DIRECT);
	PSMTXIdentity(m);
	GXLoadPosMtxImm(cam->view, GX_PNMTX0);
	GXSetCurrentMtx(GX_PNMTX0);

	GXBegin(GX_QUADS, GX_VTXFMT0, 4);
	GXPosition3f32(left, top, -10000.0f);
	GXTexCoord2u8(0, 0);
	GXPosition3f32(right, top, -10000.0f);
	GXTexCoord2u8(1, 0);
	GXPosition3f32(right, bottom, -10000.0f);
	GXTexCoord2u8(1, 1);
	GXPosition3f32(left, bottom, -10000.0f);
	GXTexCoord2u8(0, 1);
	GXEnd();

	GXSetZTexture(GX_ZT_DISABLE, GX_TF_Z8, 0);
	GXSetColorUpdate(GX_TRUE);
}

void disp_maku(CameraId camId, void* param) {
	FadeEntry* entry = param;
	f32 scaleZ = 1.0f;
	Mtx mScale, mTrans, mModel;

	if (entry->flags & 4) {
		MarioWork* mp = marioGetPtr();
		Vec screenPt;
		getScreenPoint(&mp->position, &screenPt);
		entry->animOffsetX = screenPt.x - 304.0f;
		entry->animOffsetY = -(screenPt.y - 240.0f) + 24.0f;
		scaleZ = z();
	}

	if (entry->field_0x90 > 0.0f) {
		CameraEntry* cam = camGetPtr(CAMERA_2D);
		GXTexObj texObj;
		GXColor c0, c1;

		GXLoadPosMtxImm(cam->view, GX_PNMTX0);
		GXSetCurrentMtx(GX_PNMTX0);
		GXSetZCompLoc(GX_TRUE);
		GXSetAlphaCompare(GX_ALWAYS, 0, GX_AOP_AND, GX_ALWAYS, 0);
		GXSetBlendMode(GX_BM_BLEND, GX_BL_SRCALPHA, GX_BL_INVSRCALPHA, GX_LO_SET);
		GXSetZMode(GX_FALSE, GX_ALWAYS, GX_FALSE);
		GXSetCullMode(GX_CULL_NONE);
		GXSetNumChans(1);
		GXSetChanCtrl(GX_COLOR0A0, GX_FALSE, GX_SRC_VTX, GX_SRC_VTX, GX_LIGHT_NULL, GX_DF_NONE, GX_AF_NONE);
		GXSetNumTexGens(1);
		GXSetTexCoordGen2(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, GX_IDENTITY, GX_FALSE, GX_PTIDENTITY);
		TEXGetGXTexObjFromPalette(wp->texture, &texObj, 2);
		GXLoadTexObj(&texObj, GX_TEXMAP0);
		GXSetNumTevStages(1);
		GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR0A0);
		GXSetTevSwapModeTable(GX_TEV_SWAP0, GX_CH_RED, GX_CH_GREEN, GX_CH_BLUE, GX_CH_RED);
		GXSetTevSwapMode(GX_TEVSTAGE0, GX_TEV_SWAP0, GX_TEV_SWAP0);
		GXSetTevColorOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
		GXSetTevColorIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_ZERO, GX_CC_ZERO, GX_CC_C0);
		GXSetTevAlphaOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
		GXSetTevAlphaIn(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_TEXA, GX_CA_RASA, GX_CA_ZERO);
		c0 = (GXColor){0, 0, 0, 0};
		GXSetTevColor(GX_TEVREG0, c0);
		GXClearVtxDesc();
		GXSetVtxDesc(GX_VA_POS, GX_DIRECT);
		GXSetVtxDesc(GX_VA_CLR0, GX_DIRECT);
		GXSetVtxDesc(GX_VA_TEX0, GX_DIRECT);
		GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_POS, GX_POS_XYZ, GX_F32, 0);
		GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_CLR0, GX_CLR_RGBA, GX_RGBA8, 0);
		GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_TEX0, GX_TEX_ST, GX_F32, 0);

		c1 = (GXColor){0, 0, 0, (u8)(s32)(255.0f * entry->field_0x90)};
		c0 = (GXColor){0, 0, 0, 0};

		GXBegin(GX_QUADS, GX_VTXFMT0, 4);
		GXPosition3f32(-304.0f, 0.0f, 0.0f);
		GXColor4u8(c0.r, c0.g, c0.b, c0.a);
		GXTexCoord2f32(0.0f, 0.0f);

		GXPosition3f32(304.0f, 0.0f, 0.0f);
		GXColor4u8(c0.r, c0.g, c0.b, c0.a);
		GXTexCoord2f32(19.0f, 0.0f);

		GXPosition3f32(304.0f, -240.0f, 0.0f);
		GXColor4u8(c1.r, c1.g, c1.b, c1.a);
		GXTexCoord2f32(19.0f, 7.5f);

		GXPosition3f32(-304.0f, -240.0f, 0.0f);
		GXColor4u8(c1.r, c1.g, c1.b, c1.a);
		GXTexCoord2f32(0.0f, 7.5f);
		GXEnd();

		GXSetTevSwapMode(GX_TEVSTAGE0, GX_TEV_SWAP0, GX_TEV_SWAP0);
	}

	if (entry->field_0x6C >= 0) {
		PSMTXScale(mScale, 1.0f, 1.0f, scaleZ);
		PSMTXTrans(mTrans, entry->animOffsetX, entry->animOffsetY, 0.0f);
		PSMTXConcat(mTrans, mScale, mModel);
		animPoseSetLocalTime(entry->field_0x24, entry->field_0x6C);
		animPoseDrawMtx(entry->field_0x6C, mModel, 1, 0.0f, scaleZ);
		animPoseDrawMtx(entry->field_0x6C, mModel, 2, 0.0f, scaleZ);
		animPoseDrawMtx(entry->field_0x6C, mModel, 3, 0.0f, scaleZ);
		sysWaitDrawSync();
	}
}

void disp_tec(CameraId camId, void* param) {
	FadeWork* fwp = wp;
	CameraEntry* cam = camGetPtr(CAMERA_2D);

	if (fwp->flags & 0x1000) {
		CameraEntry* camShadow = camGetPtr(CAMERA_SHADOW);
		CameraEntry* camEffA = camGetPtr(CAMERA_3D_EFFECTA);
		GXColor c0, c1, c2;

		GXSetZCompLoc(GX_TRUE);
		GXSetAlphaCompare(GX_ALWAYS, 0, GX_AOP_AND, GX_ALWAYS, 0);
		GXSetBlendMode(GX_BM_BLEND, GX_BL_SRCALPHA, GX_BL_INVSRCALPHA, GX_LO_SET);
		GXSetZMode(GX_FALSE, GX_ALWAYS, GX_FALSE);
		GXSetCullMode(GX_CULL_NONE);
		GXSetNumChans(0);
		GXSetNumTexGens(2);
		GXSetTexCoordGen2(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, GX_IDENTITY, GX_FALSE, GX_PTIDENTITY);
		GXSetTexCoordGen2(GX_TEXCOORD1, GX_TG_MTX2x4, GX_TG_TEX0, GX_IDENTITY, GX_FALSE, GX_PTIDENTITY);
		GXSetNumTevStages(2);
		GXLoadTexObj((GXTexObj*)&camShadow->field_200, GX_TEXMAP0);
		GXLoadTexObj((GXTexObj*)&camEffA->field_200, GX_TEXMAP1);

		c0 = (GXColor){0xFF, 0xFF, 0xFF, 0xFF};
		if (fwp->flags & 0x30) {
			c0.a = (u8)(s32)(255.0f * fwp->field_0x354);
		}
		GXSetTevColor(GX_TEVREG0, c0);
		c1 = (GXColor){0x00, 0xFF, 0xFF, 0xFF};
		GXSetTevKColor(GX_KCOLOR0, c1);
		c2 = (GXColor){0x00, 0x00, 0xAF, 0xFF};
		GXSetTevColor(GX_TEVREG2, c2);

		GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR_NULL);
		GXSetTevSwapModeTable(GX_TEV_SWAP1, GX_CH_RED, GX_CH_RED, GX_CH_RED, GX_CH_ALPHA);
		GXSetTevSwapModeTable(GX_TEV_SWAP2, GX_CH_GREEN, GX_CH_GREEN, GX_CH_GREEN, GX_CH_ALPHA);
		GXSetTevSwapModeTable(GX_TEV_SWAP3, GX_CH_BLUE, GX_CH_BLUE, GX_CH_BLUE, GX_CH_ALPHA);
		GXSetTevSwapMode(GX_TEVSTAGE0, GX_TEV_SWAP0, GX_TEV_SWAP1);
		GXSetTevKColorSel(GX_TEVSTAGE0, GX_TEV_KCSEL_K0);
		GXSetTevColorOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVREG1);
		GXSetTevColorIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_TEXC, GX_CC_KONST, GX_CC_ZERO);
		GXSetTevAlphaOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVREG1);
		GXSetTevAlphaIn(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_A0, GX_CA_TEXA, GX_CA_ZERO);

		GXSetTevOrder(GX_TEVSTAGE1, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR_NULL);
		GXSetTevSwapMode(GX_TEVSTAGE1, GX_TEV_SWAP0, GX_TEV_SWAP2);
		GXSetTevKColorSel(GX_TEVSTAGE1, GX_TEV_KCSEL_K0);
		GXSetTevColorOp(GX_TEVSTAGE1, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_2, GX_TRUE, GX_TEVPREV);
		GXSetTevColorIn(GX_TEVSTAGE1, GX_CC_C2, GX_CC_TEXC, GX_CC_KONST, GX_CC_C1);
		GXSetTevAlphaOp(GX_TEVSTAGE1, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
		GXSetTevAlphaIn(GX_TEVSTAGE1, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_A1);

		GXClearVtxDesc();
		GXSetVtxDesc(GX_VA_POS, GX_DIRECT);
		GXSetVtxDesc(GX_VA_TEX0, GX_DIRECT);
		GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_POS, GX_POS_XYZ, GX_F32, 0);
		GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_TEX0, GX_TEX_ST, GX_F32, 0);
		GXLoadPosMtxImm(cam->view, GX_PNMTX0);
		GXSetCurrentMtx(GX_PNMTX0);

		{
			s32 y;
			for (y = 240; y > -240; y -= 4) {
				f32 y0 = (f32)y;
				f32 y1 = (f32)(y - 2);
				f32 v0 = -((y0 / 480.0f) - 0.5f);
				f32 v1 = -((y1 / 480.0f) - 0.5f);
				GXBegin(GX_QUADS, GX_VTXFMT0, 4);
				GXPosition3f32(-304.0f, y0, 0.0f);
				GXTexCoord2f32(0.0f, v0);
				GXPosition3f32(304.0f, y0, 0.0f);
				GXTexCoord2f32(1.0f, v0);
				GXPosition3f32(304.0f, y1, 0.0f);
				GXTexCoord2f32(1.0f, v1);
				GXPosition3f32(-304.0f, y1, 0.0f);
				GXTexCoord2f32(0.0f, v1);
				GXEnd();
			}
		}

		c1 = (GXColor){0x00, 0xB4, 0xB4, 0xFF};
		GXSetTevKColor(GX_KCOLOR0, c1);
		c2 = (GXColor){0x00, 0x00, 0x00, 0xFF};
		GXSetTevColor(GX_TEVREG2, c2);

		{
			s32 y;
			for (y = 238; y > -240; y -= 4) {
				f32 y0 = (f32)y;
				f32 y1 = (f32)(y - 2);
				f32 v0 = -((y0 / 480.0f) - 0.5f);
				f32 v1 = -((y1 / 480.0f) - 0.5f);
				GXBegin(GX_QUADS, GX_VTXFMT0, 4);
				GXPosition3f32(-304.0f, y0, 0.0f);
				GXTexCoord2f32(0.0f, v0);
				GXPosition3f32(304.0f, y0, 0.0f);
				GXTexCoord2f32(1.0f, v0);
				GXPosition3f32(304.0f, y1, 0.0f);
				GXTexCoord2f32(1.0f, v1);
				GXPosition3f32(-304.0f, y1, 0.0f);
				GXTexCoord2f32(0.0f, v1);
				GXEnd();
			}
		}

		GXSetTevSwapMode(GX_TEVSTAGE0, GX_TEV_SWAP0, GX_TEV_SWAP0);
		GXSetTevSwapMode(GX_TEVSTAGE1, GX_TEV_SWAP0, GX_TEV_SWAP0);
	}

	if (fwp->flags & 0x2000) {
		CameraEntry* camEffB = camGetPtr(CAMERA_3D_EFFECTB);
		GXColor c0;

		GXSetZCompLoc(GX_TRUE);
		GXSetAlphaCompare(GX_ALWAYS, 0, GX_AOP_AND, GX_ALWAYS, 0);
		GXSetBlendMode(GX_BM_BLEND, GX_BL_SRCALPHA, GX_BL_INVSRCALPHA, GX_LO_SET);
		GXSetZMode(GX_FALSE, GX_ALWAYS, GX_FALSE);
		GXSetCullMode(GX_CULL_NONE);
		GXSetNumChans(0);
		GXSetNumTexGens(1);
		GXSetTexCoordGen2(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, GX_IDENTITY, GX_FALSE, GX_PTIDENTITY);
		GXSetNumTevStages(1);
		GXLoadTexObj((GXTexObj*)&camEffB->field_200, GX_TEXMAP0);
		GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR_NULL);
		c0 = (GXColor){0xE6, 0xE6, 0xE6, 0xFF};
		GXSetTevColor(GX_TEVREG0, c0);
		GXSetTevColorOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
		GXSetTevColorIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_TEXC, GX_CC_C0, GX_CC_ZERO);
		GXSetTevAlphaOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
		GXSetTevAlphaIn(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_A0);
		GXClearVtxDesc();
		GXSetVtxDesc(GX_VA_POS, GX_DIRECT);
		GXSetVtxDesc(GX_VA_TEX0, GX_DIRECT);
		GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_POS, GX_POS_XYZ, GX_F32, 0);
		GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_TEX0, GX_TEX_ST, GX_F32, 0);
		GXLoadPosMtxImm(cam->view, GX_PNMTX0);
		GXSetCurrentMtx(GX_PNMTX0);

		GXBegin(GX_QUADS, GX_VTXFMT0, 4);
		GXPosition3f32(-304.0f, 240.0f, 0.0f);
		GXTexCoord2f32(0.0f, 0.0f);
		GXPosition3f32(304.0f, 240.0f, 0.0f);
		GXTexCoord2f32(1.0f, 0.0f);
		GXPosition3f32(304.0f, -240.0f, 0.0f);
		GXTexCoord2f32(1.0f, 1.0f);
		GXPosition3f32(-304.0f, -240.0f, 0.0f);
		GXTexCoord2f32(0.0f, 1.0f);
		GXEnd();
	}
}

void disp_dummy_black(CameraId camId, void* param) {
	extern int seqGetSeq(void);
	int curSeq;
	CameraEntry* cam;
	GXColor c0;

	curSeq = seqGetSeq();
	// Never blank out overworld (SEQ_GAME = 2) or story cutscenes (SEQ_MAP_CHANGE = 3)
	if (curSeq == 2 || curSeq == 3) {
		return;
	}

	cam = camGetPtr(CAMERA_2D);

	GXSetZCompLoc(GX_TRUE);
	GXSetAlphaCompare(GX_ALWAYS, 0, GX_AOP_AND, GX_ALWAYS, 0);
	GXSetBlendMode(GX_BM_BLEND, GX_BL_SRCALPHA, GX_BL_INVSRCALPHA, GX_LO_SET);
	GXSetZMode(GX_FALSE, GX_ALWAYS, GX_FALSE);
	GXSetCullMode(GX_CULL_NONE);
	GXSetNumChans(0);
	GXSetNumTexGens(0);
	GXSetNumTevStages(1);
	GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD_NULL, GX_TEXMAP_NULL, GX_COLOR_NULL);
	c0.r = 0; c0.g = 0; c0.b = 0; c0.a = 0xFF;
	GXSetTevColor(GX_TEVREG0, c0);
	GXSetTevColorOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
	GXSetTevColorIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_ZERO, GX_CC_ZERO, GX_CC_C0);
	GXSetTevAlphaOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
	GXSetTevAlphaIn(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_A0);
	GXClearVtxDesc();
	GXSetVtxDesc(GX_VA_POS, GX_DIRECT);
	GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_POS, GX_POS_XYZ, GX_F32, 0);
	GXLoadPosMtxImm(cam->view, GX_PNMTX0);
	GXSetCurrentMtx(GX_PNMTX0);

	GXBegin(GX_QUADS, GX_VTXFMT0, 4);
	GXPosition3f32(-304.0f, 240.0f, 0.0f);
	GXPosition3f32(304.0f, 240.0f, 0.0f);
	GXPosition3f32(304.0f, -240.0f, 0.0f);
	GXPosition3f32(-304.0f, -240.0f, 0.0f);
	GXEnd();
}

BOOL fadeIsFinish(void) {
	FadeWork* fwp = wp;
	FadeEntry* entry;
	s32 idx = fwp->field_0x350;
	if (idx == -1) {
		return TRUE;
	}
	entry = &fwp->entries[idx];
	return (entry->flags >> 1) & 1;
}

void fadeReset(s32 id) {
	FadeEntry* entry;

	entry = &wp->entries[id];
	if (entry->field_0x6C >= 0) {
		animPoseRelease(entry->field_0x6C);
	}
	if (entry->field_0x70 >= 0) {
		animPaperPoseRelease(entry->field_0x70);
	}
	if (entry->image) {
		imgRelease(entry->image, entry->imageType);
	}
	memset(entry, 0, sizeof(FadeEntry));
	entry->field_0x6C = -1;
	entry->field_0x70 = -1;
	entry->fadeSpotX = dat_80419498;
	entry->fadeSpotY = dat_80419498;
	entry->animOffsetX = dat_80419498;
	entry->animOffsetY = dat_80419498;
	entry->field_0x90 = dat_8041949c;
}

TEXPalette* fadeGetTpl(void) {
	return wp->texture;
}

void fadeSetNarrowFast(void) {
	wp->entries[1].flags |= 8;
}
