#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "config.h"
#include "transBuff.h"
#include "session.h"
#include "testBase.h"
#include "trialAudioFiles.h"
#include "play.h"

#undef USE_NCURSES
#ifdef USE_NCURSES
#include "curses.h"    //curses support
#define MVPRNT  mvprintw(j++, 0,
#else
#define MVPRNT  printf(
#define KEY_UP      '+'
#define KEY_DOWN    '-'
#define KEY_LEFT    '<'
#define KEY_RIGHT   '>'
#endif

int step_main(char *step_exe_path, char *session_file, char *lab_id, char *lis_id);

/* draw trial panel */
#define DRAW_FULL	1
#define DRAW_UPDATE	2
void
draw_trial_panel(Session *session, AudioFiles *paf, int mode)
{
    SessionInfo *psi = &session->session_info;
    Config *config = session->config;
    
	int i, j, k, tot_sys;
	extern double scores[MAX_SYS];
	j = 0;
	tot_sys = psi->has_open_ref + psi->num_systems;
	/* banner */
	MVPRNT "%s Test\n", psi->methodology);
	MVPRNT "\nTrial %d of %d: %s\n",
		psi->tnum+1, psi->num_trials, psi->trial[psi->tnum].sig_lab);
	j = 3;
	/* Systems under test */
	k=0;
	for (i=0; i<tot_sys; i++) {
		if (i==0 && psi->has_open_ref) {
			/* 0 is reference if has_open_ref == 1 */
			if (paf->playing == 1 && paf->next_sys == i) {
				MVPRNT "REF                 Playing\n");
			}
			else {
				MVPRNT "REF                        \n");
			}
		}
		else {
			/* systems under test */
			if (paf->playing == 1 && paf->next_sys == i) {
				MVPRNT "Sys%2d  Score: %5.1f Playing\n", k+1, scores[k]);
			}
			else {
				MVPRNT "Sys%2d  Score: %5.1f        \n", k+1, scores[k]);
			}
			k++;
		}
	}
	j++;
	/* footer */
	MVPRNT "%s %s %s\n",
		(paf->playing == 1) ? "Play " : "Pause",
		(paf->loop == 1) ? "Loop" : "    ",
		(config->show_system == 1) ? psi->trial[psi->trial_map[psi->tnum]].sig_lab : "                  ");
	j++;
	if (mode == DRAW_FULL) {
		MVPRNT "Left/Right Arrow keys to change score of system playing, or \n");
		MVPRNT "Up/Down Arrow keys to play another system or\n");
		MVPRNT "P)lay or pause, L)oop or no loop, N)ext trial, Q)uit test\n");
	}
}

int display_trial(Config *config, Session *session,
               TestBase *this_test, TrialAudioFiles *trial_audio_files, Play *play)
{
    SessionInfo *psi = &session->session_info;
    AudioFiles *paf = play->paf;
    int trial_done, session_done;
    extern double scores[MAX_SYS], score_top, score_bottom, score_inc;
    int tot_sys = psi->has_open_ref + psi->num_systems;

     /* Initialize curses */
    #ifdef USE_NCURSES
    initscr(); /* Start curses mode */
    keypad(stdscr, TRUE);      /* We get F1, F2 etc..      */
    cbreak();  /* Line buffering disabled */
    noecho(); /* Uncomment this if you don't want to echo characters when typing */
    #endif

    /* display Trial Panel */
    draw_trial_panel(session, paf, DRAW_FULL);
#ifdef USE_NCURSES
    refresh();
#endif
    trial_done = 0;
    while (!trial_done) {
        int ch;
#ifdef USE_NCURSES
        ch = getch();
#else
        ch = getchar();
//            printf("%c", ch);
#endif
        switch (ch) {
            case '\n':
//                    printf("CR");
                break;
            case 'p':
            case 'P':
                play->alternatePlayPause( );
                break;
            case 'l':
            case 'L':
                if (paf->loop) {
                    paf->loop = 0;
                }
                else {
                    paf->loop = 1;
                }
                break;
            case 'n':
            case 'N':
                this_test->getScores( );
                paf->playing = 0;
                paf->prev_sys = 0;
                paf->next_sys = 0;
                trial_done = 1;
                break;
            case 'q':
            case 'Q':
                paf->playing = 0;
                paf->prev_sys = 0;
                paf->next_sys = 0;
                trial_done = 1;
                session_done = 1;
                break;
            case KEY_LEFT:
                if (psi->has_open_ref && paf->next_sys == 0) {
                    ;/* no action if open reference is playing */
                }
                else {
                    int i = psi->has_open_ref ? paf->next_sys-1 : paf->next_sys;
                    scores[i] -= (score_top-score_bottom)/20;
                    if (scores[i] < score_bottom)
                        scores[i] = score_bottom;
                }
                break;
            case KEY_RIGHT:
                if (psi->has_open_ref && paf->next_sys == 0) {
                    ; /* no action if open reference is playing */
                }
                else {
                    int i = psi->has_open_ref ? paf->next_sys-1: paf->next_sys;
                    scores[i] += 5;
                    if (scores[i] > score_top)
                        scores[i] = score_top;
                }
                break;
            case KEY_UP:
                if (paf->playing == 1) {
                    /* process only if playing */
                    int tmp = paf->next_sys - 1;
                    if (tmp < 0)
                        tmp = 0;
                    paf->next_sys = tmp;
                }
                break;
            case KEY_DOWN:
                if (paf->playing == 1) {
                    /* process only if playing */
                    int tmp = paf->next_sys + 1;
                    if (tmp > tot_sys-1)
                        tmp = tot_sys-1;
                    paf->next_sys = tmp;
                }
               break;
            }
            draw_trial_panel(session, paf, DRAW_UPDATE);
    #ifdef USE_NCURSES
            refresh();
    #endif

            /* End curses mode  */
    #ifdef USE_NCURSES
            endwin();
    #endif
        } /* this trial */
        
    return 1;
}


int
main(int argc, char *argv[])
{

    char *session_file, *lab_id, *lis_id, *ses_id;

	/* parse command line */
	if (argc != 4) {
		char *argv_0 = strrchr(argv[0], PATH_SEP[0]); /* points to rightmost '/' */
		argv_0++; /* points to first char of executable name */
		printf("Usage: %s session_file lab_id listener_id\n",
			argv_0);
        return -1;
	}
    
	session_file = argv[1];
	lab_id = argv[2];
	lis_id = argv[3];

    /* set defaults */
    ses_id = (char *)"1";

    step_main(argv[0], session_file, lab_id, lis_id);

    return 0;
}
