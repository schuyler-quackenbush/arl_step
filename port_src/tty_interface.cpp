#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <ctype.h>

#include "config.h"
#include "allTestIncludes.h"
#include "tty_interface.h"

/* externs */
extern Config *config;
extern TestBase *this_test;

/* tty interface functions */
int
my_getchar()
{
	int i, c;
	i = 0;
	do {
		c = i; /* save prev char */
		i = getchar();
		//printf("0x%02x\n", i);
	} while (i != '\r' && i != '\n');
	c = toupper(c);
	//printf("0x%02x\n", c);
	return c;
}

void
my_getline(char *msg)
{
	int i, c;
	for (i=0; ; i++) {
		c = getchar();
		msg[i] = c;
		if (c == '\r' || c == '\n') {
			msg[i] = 0;
			break;
		}
	}
}

void
testButtons( int has_open_ref, int nsys, int *letter, int *index )
{
	int i, c;

	printf("========================\n");
	printf("Enter letter of system to play, L)oop, N)ext Trial, Q)uit followed by Enter\n");
	if (has_open_ref) {
		printf("R)ef ");
	}
	for (i=0; i<nsys; i++)
		printf("%c) ", 'A'+i);
	printf(": ");
	c = my_getchar();
	if (has_open_ref && c == 'R') {
		*index = 0;
	}
	else if (c == 'L' || c == 'N' || c == 'Q') {
		*index = -1;
	}
	else {
		if (c < 'A')
			c = 'A';
		if (c > 'A'+nsys-1)
			c = 'A'+nsys-1;
		*index = c-'A'+1;
	}
	*letter = c;
}

void
testScores(Session *session, TrialAudioFiles *trial_audio_files)
{
	int i, s;
	SessionInfo *psi = &session->session_info;
	if ( !config->valid_license || config->no_score )
		return;
	printf("========================\n");
	printf("Enter scores\n");
	for (i=0; i<psi->num_systems; i++) {
		printf("Enter score for %c: ", 'A'+i);
		scanf("%d", &s);
		printf("You entered: %d\n", s);
		this_test->trial_scores[psi->trial_map[psi->tnum]].score[psi->sys_map[i]] = s;
	}
}
