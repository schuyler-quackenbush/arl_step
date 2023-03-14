#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "defineValues.h"
#include "transBuff.h"
#include "transition.h"
#include "utils.h"

#if (DEBUG_TRB)
#include <stdio.h>
#include "wav.h"
void write_buf_trans(AUDIO_SAMP *buf, int samplerate, int channels, int frames, const char *filename) {
    WAVHDR wav_hdr, *wp = &wav_hdr;
    wp->fmt = '0';
    wp->ssize = 2;
    wp->samplerate = samplerate;
    wp->channels = channels;
    wp->frames = frames;
    openWavFile(filename, "w", wp);
    writeWavFloat( wp, buf, channels*frames );
    closeWavFile( wp );
}
#endif

/* class TransBuff */

/*
Definition of transition modes
Transitions are always using raised sin window
cross-fade has duration of FADE_LEN
fade-out has duration of FADE_LEN
fade-in has duration of FADE_LEN
ID_TRANS_CF     cross-fade
ID_TRANS_05MS   fade-out,  5ms zeros, fade-in
ID_TRANS_10MS   fade-out, 10ms zeros, fade-in
ID_TRANS_20MS   fade-out, 20ms zeros, fade-in
 */

/* class constructor */
TransBuff::TransBuff( )
{
	/* initialize storage pointers to NULL */
	trans_in = NULL;
	trans_out = NULL;
}

/* class desctructor */
TransBuff::~TransBuff()
{
	my_free(trans_in);
	my_free(trans_out);
}

/* member functions */

void
TransBuff::init_transition( int samplerate, int mode )
{
	int n, nz, na;
	double pi, d, x;

    /* transition mode */
    trans_mode = mode;
    
    /* fade-in and fade-out length, frames */
    n = (int)(FADE_LEN * samplerate + 0.5);;
    
    if (trans_mode == ID_TRANS_CF) {
        /* cross-fade over one buffer interval */
        trans_out = (float *)malloc( n * sizeof(float) );
        trans_in = (float *)malloc( n * sizeof(float) );
        
        trans_len = n;
        trans_pos = 0;

        /* fade-out
         * from pi/2 to -pi/2 (+1 to -1)
         * n must be even!
         */
        pi = 4*atan2(1.0, 1.0);
        d = pi/n; /* step in phase */
        x = pi/2 - d/2; /* initial phase is 1/2 phase step from -pi/2 */
        for (int i=0; i<n; i++) {
            /* from sin(pi-d/2) to sin(-pi+d/2)*/
            trans_out[i] = 0.5 + 0.5*sin(x);
            x -= d;
        }
        /* fade-in is mirror of fade-out */
        for (int i=0; i<n; i++) {
            trans_in[i] = trans_out[n-1-i];
        }
    }
    else {
        switch (trans_mode) {
            case ID_TRANS_05MS :
                /* fade-out, mute for 5 ms, fade-in */
                nz = (int)(0.005 * samplerate + 0.5); /* 5 ms */
                break;
            case ID_TRANS_10MS :
                /* fade-out, mute for 10 ms, fade-in */
                nz = (int)(0.010 * samplerate + 0.5); /* 10 ms */
                break;
            default: // ID_TRANS_20MS
                /* fade-out, mute for 20 ms, fade-in */
                nz = (int)(0.020 * samplerate + 0.5); /* 20 ms */
                break;
        }
        na = (n + nz + n); /* (fade-out, zeros, fade-in) */
        
        trans_out = (float *)malloc( na * sizeof(float) );
        trans_in = (float *)malloc( na * sizeof(float) );
        
        trans_len = na;
        trans_pos = 0;
        
        for (int i=0; i<na; i++) {
            /* initialize all to zero */
            trans_out[i] = 0.0;
            trans_in[i] = 0.0;
        }

        /* trans_this */
        pi = 4*atan2(1.0, 1.0);
        d = pi/n; /* step in phase */
        x = pi/2 - d/2; /* initial phase is 1/2 phase step from -pi/2 */
        for (int i=0; i<n; i++) {
            /* from sin(pi-d/2) to sin(-pi+d/2)*/
            trans_out[i] = 0.5 + 0.5*sin(x);
            x -= d;
        }
        /* trans_next is shifted mirror of trans_out */
        for (int i=0; i<n; i++) {
            trans_in[n+nz+i] = trans_out[n-1-i];
        }

    }
    
#if (DEBUG_TRB)
    /* write trans_out and trans_in */
    write_buf_trans(trans_out, samplerate, 1, trans_len, "trans_out.wav");
    write_buf_trans(trans_in, samplerate, 1, trans_len, "trans_in.wav");
#endif
}

/* free all TransBuff malloc'd storage */
void
TransBuff::free_trans_buff( void )
{
	my_free(trans_in);
	my_free(trans_out);
}

/* copy next sig to output buffer  */
void
TransBuff::copyBuf(AUDIO_SAMP *out, SysFiles *pns, int next_frame, int channels, unsigned long framesPerBuffer)
    {
    /* no transition, so just interleave into output buffer */
    for (unsigned long i=0; i<framesPerBuffer; i++) {
        for (int j=0; j<channels; j++) {
            out[i*channels + j] = pns->samples[j][next_frame+i];
        }
    }
}

/* transition from this sig to next sig using transition windows  */
int
TransBuff::transitionBuf(AUDIO_SAMP *out, SysFiles *pps, SysFiles *pns,
                         int next_frame, int channels, unsigned long framesPerBuffer)
{
    if (trans_pos == 0) {
        /* save local copies for using during transition */
        lpps = pps;
        lpns = pns;
    }
    int done = 0;
    for (unsigned long i=0; i<framesPerBuffer; i++) {
        if (trans_pos < trans_len) {
            /* transition */
            for (int j=0; j<channels; j++) {
                out[i*channels + j] =
                    lpps->samples[j][next_frame+i] * trans_out[trans_pos] +
                    lpns->samples[j][next_frame+i] * trans_in[trans_pos];
            }
            trans_pos++;
        }
        else {
            /* finished transition
             * so copy remainder of buffer
             */
            for (int j=0; j<channels; j++) {
                out[i*channels + j] =
                    lpns->samples[j][next_frame+i];
            }
            done = 1;
        }
    }
    if (done) {
        /* finished transition
         * so reset position to beginning
         */
        trans_pos = 0;
    }
    //printf("trans_pos %d %d\n", trans_pos, trans_len);
    return done;
}
