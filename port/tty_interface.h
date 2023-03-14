#ifndef __TTY_INTERFACE_H
#define __TTY_INTERFACE_H

#include "session.h"
#include "trialAudioFiles.h"

/* function prototypes */
int my_getchar( void );
void my_getline(char *msg);
void testButtons( int has_open_ref, int nsys, int *letter, int *index );
void testScores(Session *session, TrialAudioFiles *trial_audio_files);

#endif /* __TTY_INTERFACE_H */