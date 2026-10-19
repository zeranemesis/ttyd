#include "evt/evt_paper.h"
#include "mario/mariost.h"

extern GlobalWork* gp;
#define wp gp

USER_FUNC(evt_paper_delete) {
	animPaperPoseRelease(animPaperPoseGetId((const char*)evtGetValue(event, *event->args), wp->inBattle != 0));
	return EVT_RETURN_DONE;
}

USER_FUNC(evt_paper_entry) {
	const char* name;
	s32 group;

	name = (const char*)evtGetValue(event, *event->args);
	group = wp->inBattle != 0;
	if (!animGroupBaseAsync(name, group, NULL)) {
		return EVT_RETURN_BLOCK;
	}
	animPaperPoseEntry(name, group);
	return EVT_RETURN_DONE;
}
