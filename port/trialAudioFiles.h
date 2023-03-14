#ifndef __TrialAudioFiles_H
#define __TrialAudioFiles_H

#include <stdatomic.h>
#include "defineValues.h"

/* information about component files of signal
 * defines SysFiles structure
 */
#include "session.h"
#include "sysFiles.h"

/* common information about audio signals in a Trial */
typedef struct {
	atomic_int channels;
    atomic_ulong ru_frames;     /* rounded up to integral number of prefSize output buffers */
	atomic_int samplerate;
	atomic_int prefSize;        /* (preferred) size of output buffer */
    atomic_ulong next_frame;    /* next frame for playout */
	atomic_ulong start_frame;   /* zoom start frame */
	atomic_ulong stop_frame;    /* zoom stop frame */
    atomic_int playing;         /* if 1 then play else pause (play zeros) */
	atomic_int loop;	        /* if 1 then loop else pause and end of signal */
    
	atomic_int prev_sys;
	atomic_int next_sys;

	SysFiles sys_files[MAX_SYS];
} AudioFiles;

class TrialAudioFiles {
public:
	TrialAudioFiles();
	~TrialAudioFiles();
	int openAudioFiles(SessionInfo *psi, Config *pc, int i);
	AudioFiles *get_pAudioFiles();
	void freeAudioFiles(void);

	AudioFiles audio_files;
	Config *config;
};

#endif /* __TrialAudioFiles_H */
