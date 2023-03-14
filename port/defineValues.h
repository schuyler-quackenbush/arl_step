#ifndef __defineValues_H
#define __defineValues_H

#define STEP_NAME	"STEP"
#define VERSION     3		/* integer */
#define RELEASE     "01"	/* 2-character string */

#define MAX_DEV     16      /* maximum audio output devices */
#define MAX_CHN     32
#define MM_SYS		12	    //BS-1534-2 recommends at most 12 systems under test. Open reference is not counted
#define MAX_SYS     14
#define LAB_LEN		32
#define SUF_LEN		16
#define MSG_LEN		256
#define FILE_LEN	256
#define MAX_TRIAL_SEC	2	/* max playout time, sec, for demo mode */
#define LINE_LEN 	1024

#define SESSION_FILE_FILTER "Session Files\0*.asi\0All Files\0*.*\0"
#ifdef _WIN32
#define PATH_SEP	"\\"
#else
#define PATH_SEP	"/"
#endif
#define CONFIG_FILE	"config.ini"
#define CHECK_FILE	"check.txt"
#define WAV_EXT		".wav"

#define MUSHRA_MAX_COND	12
#define MIN_LOOP	0.5	/* seconds */
#define PLAY_TICKS	100	/* 10 ms */
#define ID_SCROLL_TIMER	1
#define ID_VIDEO_TIMER	2

#define SAME		1
#define DIFFERENT	0

#if 0
#define AUDIO_SAMP  long
#define READ_WAV    readWavLong
#else
#define AUDIO_SAMP  float
#define READ_WAV    readWavFloat
#endif

/* PortAudio support */
#define FRAMES_PER_BUFFER   1000


#define DEBUG_TAF   0
#define DEBUG_SES   0
#define DEBUG_CMD   1
#define DEBUG_TRB   0
#define DEBUG_PLY   0


#endif /* __defineValues_H */
