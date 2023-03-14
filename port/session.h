#ifndef __SESSION_H
#define __SESSION_H

#include "defineValues.h" 
#include "config.h"
#include "license.h"
#include "wav.h"

#define FF_MONO		1
#define FF_INTER	2
#define CHN_LAB		1
#define CHN_MAP		2

#define T_MUSHRA	1
#define T_BS1116	2
#define T_AB		3
#define T_TRAIN		4
#define T_ABX		5
#define T_AB5		6
#define T_AB7		7
#define T_MOS		8
#define T_P835		9
#define T_CUSTOM	10
#define T_PLAYLIST	11

/* information about interleaved or component files in a system signal */
typedef struct {
	char sys_lab[LAB_LEN];			/* system label */
	char basefilename[FILE_LEN];	/* base filename */
	char componentname[MAX_CHN][FILE_LEN];	/* component file pathname name */
	WAVHDR wavHdr[MAX_CHN];	        /* wav header for each component file */
	char *wavStatus[MAX_CHN];       /* status string from openWavFile */
    int active_channels; /* number of active channels in this signal */
    int max_channels_in_signal;     /* max number of channels in this signal (which might include zero channels) */
    int max_frames_in_signal;       /* max number of frames in this signal */
} System;

/* information about signal files in one Trial */
typedef struct {
    System system[MAX_SYS];      /* set of system files in a trial */
	char sig_lab[LAB_LEN];			/* signal label for the trial, e.g. Sig1, Sig2 */
    int max_channels_in_trial;      /* max number of channels in this trial */
    int max_frames_in_trial;        /* max number of frames in this trial */
} Trial;

/* information about Session */
typedef struct {
	int valid_license;				/* 1 if valid */

	char methodology[LAB_LEN];		/* test type, as string */
	int test_id;					/* test type, as integer */
    int trans_mode;                 /* signal to signal transition mode */
	int file_format;				/* mono or interleaved */
	int has_open_ref;				/* 1 if open reference is listed in session file */	
	int speaker_configuration;		/* SAME or DIFFERENT */
	int num_trials;					/* number of trials */
	int num_systems;				/* number of systems under test */
    int has_video;                  /* there is associated video */
    int max_channels_in_session;    /* max number of channels in this session */
    int samplerate;               /* common sampling rate for all items in session */

	/* index is output buffer index, value is interleaved channel index */
	int cmp_chn[MAX_CHN];			/* interleaved channel map */
	char cmp_lab[MAX_CHN][SUF_LEN];	/* mono file label suffix map */
	char map_file[FILE_LEN];		/* file from session file */
	int map_channels;				/* number of lines in channel map or file label map */
    
	int tnum;						/* current trial number */
	int *trial_map;					/* trail presentation order */
	int *sys_map;					/* system or condition mapping to sliders */

	Trial *trial;                   /* all trials in session */
} SessionInfo;

class Session {
public:
	Session();
	~Session();
	void initSession( int valid_license, char *session_file, Config *p1);
	int openSession( void );
    int printSessionConfig ( void );
	int startSession( void );
	void endSession( void );
	int *mkmap( int n );
	void permute( int *map, int n );
	void reset_map( int *map, int n );
	int coin_toss(void);

	SessionInfo session_info;
	Config *config;
};

#endif /* __SESSION_H */
