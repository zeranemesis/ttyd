#include "evt/evt_lottery.h"
#include "evt/evt_cmd.h"
#include "mario/mariost.h"
#include <dolphin/os.h>

extern GlobalWork* gp;
extern GlobalWork* wp_80422810;

//.sbss
BOOL dbg_lotteryinfo;

//local prototypes
USER_FUNC(evt_lottery);

lottery_info* lotteryGetPtr(void) {
	return &wp_80422810->mLotteryInfo;
}

USER_FUNC(evt_lottery_todaynum) {
	evtSetValue(event, *event->args, *(s16*)((u8*)wp_80422810 + 0xB8));
	return EVT_RETURN_DONE;
}

USER_FUNC(evt_lottery_flag) {
	evtSetValue(event, *event->args, *(u16*)((u8*)wp_80422810 + 0xA8));
	return EVT_RETURN_DONE;
}

USER_FUNC(evt_lottery_error_check) {
	s32* args = event->args;
	u8* global = (u8*)wp_80422810;
	OSTime now = OSGetTime();
	u16 flags = *(u16*)(global + 0xA8);
	BOOL result = FALSE;
	if (flags & 8) {
		result = TRUE;
	}
	if (flags & 4) {
		if (now >= *(OSTime*)(global + 0xC0) ||
		    now >= *(OSTime*)(global + 0xD8) ||
		    now >= *(OSTime*)(global + 0xE0) ||
		    now >= *(OSTime*)(global + 0xB0)) {
			result = TRUE;
		}
	}
	if ((flags & 0x10) && now <= *(OSTime*)(global + 0xC0)) {
		OSCalendarTime nowCalendar;
		OSCalendarTime savedCalendar;

		OSTicksToCalendarTime(now, &nowCalendar);
		OSTicksToCalendarTime(*(OSTime*)(global + 0xC0), &savedCalendar);
		if (nowCalendar.year == savedCalendar.year &&
		    nowCalendar.mon == savedCalendar.mon &&
		    nowCalendar.mday == savedCalendar.mday) {
			result = FALSE;
		}
	}
	evtSetValue(event, *args, result);
	return EVT_RETURN_DONE;
}

USER_FUNC(evt_lottery) {
	lottery_info* LotteryInfo;

	s32* args = event->args;
	s32 arg1 = evtGetValue(event, args[0]); //r25
	OSTime start = OSGetTime();
	dbg_lotteryinfo = TRUE;

	LotteryInfo = &gp->mLotteryInfo;

	LotteryInfo->mFlags &= ~0x1000;
	LotteryInfo->mFlags &= ~0x2000;
	LotteryInfo->mFlags &= ~0x4000;
	LotteryInfo->mFlags &= ~0x8000;

	if (LotteryInfo->mFlags & 8) {

	}
















	return EVT_RETURN_DONE;
}
