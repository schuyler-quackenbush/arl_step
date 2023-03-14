#ifndef __TRANSBUFF_H
#define __TRANSBUFF_H

#include "sysFiles.h"

/* fade window length
 * cross-fade is FADE_LEN
 * transitions are FADE_LEN, TRANS_xxMS, FADE_LEN
 */
#define FADE_LEN        0.020 /* seconds */

#define ID_FIRST_BUF	1
#define ID_MID_BUF		2
#define ID_LOCK_BUF		3
#define ID_LAST_BUF		4
#define ID_PAST_BUF		5

#define ulong unsigned long

class TransBuff {
public:
	TransBuff( );
	~TransBuff( );
	void init_transition( int srate, int trans_mode );
    void free_trans_buff( void );
    void copyBuf(AUDIO_SAMP *out, SysFiles *pns, int next_frame, int channels, unsigned long framesPerBuffer);
    int transitionBuf(AUDIO_SAMP *out, SysFiles *pps, SysFiles *pns,
        int next_frame, int channels, unsigned long framesPerBuffer);

    int trans_mode; //cross-fade or fade-out, zeros, fade-in
    int trans_len;  //length of transition, in samples
    int trans_pos;  //position of next frame within trans sequence
    
    /* local copies for use during stransition */
    SysFiles *lpps;
    SysFiles *lpns;
    
    /* pointers to transition windows */
    float *trans_out;
    float *trans_in;
};

#endif /* __TRANSBUFF_H */
