/* "seqdrv" - Sequence Driver
 * Status: Mostly complete, need function pointers in seq_data and
 * proper names for seqdrv_work
 *
 * Function: used for swapping between game states (logo, game, battle, demo/e3)
 */
#include "driver/seqdrv.h"
#include "system.h"
#include <stdio.h>
#include <string.h> //memset prototype

extern SequenceCallback seq_data[8][3];

//.sdata
SequenceType now_seq = SEQ_UNDEFINED;
SequenceType next_seq = SEQ_UNDEFINED;
SequenceType prev_seq[2] = { SEQ_UNDEFINED, 0 };
s32 seq_range[2] = { 0x01000000, 0 };

//.sbss
SequenceWork seqWork;
const char* next_p1;
const char* next_p0;
static BOOL s_seqForceChange = FALSE;

void seqInit_MARIOSTORY(void) {
	memset(&seqWork, 0, sizeof(SequenceWork));
	now_seq = SEQ_UNDEFINED;
	next_seq = SEQ_UNDEFINED;
	prev_seq[0] = SEQ_UNDEFINED;
	s_seqForceChange = FALSE;
}

void seqMain(void) {
	if (next_seq != now_seq || s_seqForceChange) {
		s_seqForceChange = FALSE;
		printf("[seqMain] Transitioning from %d to %d (map=%s, bero=%s)\n",
		       now_seq, next_seq, next_p0 ? next_p0 : "(null)", next_p1 ? next_p1 : "(null)");
		fflush(stdout);
		if (now_seq != SEQ_UNDEFINED) {
			sysWaitDrawSync();
			seq_data[now_seq][2](&seqWork); //call Exit function
		}
		prev_seq[0] = now_seq; //store previous
		memset(&seqWork, 0, sizeof(SequenceWork));
		now_seq = next_seq;
		seqWork.sequence = next_seq;
		seqWork.state = 0;
		seqWork.mapName = next_p0;
		seqWork.beroName = next_p1;
		printf("[seqMain] Calling init for seq %d...\n", next_seq); fflush(stdout);
		seq_data[next_seq][0](&seqWork); //call Init function
		printf("[seqMain] Init for seq %d done!\n", next_seq); fflush(stdout);
	}
	seq_data[now_seq][1](&seqWork); //call Main function
}

void seqSetSeq(SequenceType seq, const char* map, const char* bero) {
	if (seq == now_seq && seq == SEQ_MAP_CHANGE && map != NULL) {
		s_seqForceChange = TRUE;
	}
	next_seq = seq;
	next_p0 = map;
	next_p1 = bero;
}

SequenceType seqGetSeq(void) {
	return now_seq;
}

SequenceType seqGetPrevSeq(void) {
	return prev_seq[0];
}

SequenceType seqGetNextSeq(void) {
	return next_seq;
}

BOOL seqCheckSeq(void) {
	return now_seq != next_seq;
}
