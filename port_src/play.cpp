#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdatomic.h>
#include <portaudio.h>
#include "defineValues.h"
#include "config.h"
#include "license.h"
#include "paUtils.h"
#include "play.h"
#include "session.h"
#include "trialAudioFiles.h"
#include "transBuff.h"
#include "portControls.h"
#include "testBase.h"
#if WIN32
#include "video.h"
extern Video *video;
#endif

extern TestBase *this_test;
extern TransBuff *trans_buff;

#if (DEBUG_PLY)
#define DEBUG_NUM_BUF 20
AUDIO_SAMP *debug_out;
int debug_blk = 0;
#include "wav.h"
void write_buf_ply(AUDIO_SAMP *buf, int samplerate, int channels, int frames, const char *filename) {
    WAVHDR wav_hdr, *wp = &wav_hdr;
    wp->fmt = '0';
    wp->ssize = 2;
    wp->samplerate = samplerate;
    wp->channels = channels;
    wp->nsamp = frames;
    wp->frames = frames;
    openWavFile(filename, "w", wp);
    writeWavFloat( wp, buf, channels*frames );
    closeWavFile( wp );
}
#endif

/* PortAudio stream pointer */
static PaStream *stream;

/* PortAudio callback */
int paCallback( const void *inputBuffer, void *outputBuffer,
    unsigned long framesPerBuffer,
    const PaStreamCallbackTimeInfo* timeInfo,
    PaStreamCallbackFlags statusFlags,
    void *userData );

/* local helper functions */
static int buff_boundary( AudioFiles *paf, double time );

/* class definition */

/* class constructor */
Play::Play( )
{
    ptf = NULL;
    paf = NULL;
}

/* class desctructor */
Play::~Play()
{
	;
}

/* member functions */
int
Play::initPlay( Session *ps, TrialAudioFiles *ptf, int trans_mode )
{
    /* initialize variables and pointers */
    int max_channels_in_session = ps->session_info.max_channels_in_session;
    int framesPerBuffer = ptf->audio_files.prefSize;
    int samplerate = ps->session_info.samplerate;
	this->session = ps;
    this->ptf = ptf;
    this->paf = ptf->get_pAudioFiles();

#if (DEBUG_PLY)
#define DEBUG_NUM_BUF 10
    debug_out = (AUDIO_SAMP *)malloc( DEBUG_NUM_BUF*max_channels_in_session*framesPerBuffer*sizeof(AUDIO_SAMP));
#endif
    
    /* initialize transition windows */
    trans_buff->init_transition( samplerate, trans_mode );
    
	/* not playing (actually playing out zeros) */
	paf->playing = 0;
    paf->next_sys = 0;

    /* start up Port Audio */
    stream = startupPa(
                       1,                       //input channels
                       max_channels_in_session, //output channels
                       samplerate,
                       framesPerBuffer,         //input and output buffer size, in frames
                       paCallback,
                       paf);

	/* Now everything is up and running but playing out zeros */
	return 1;
}

//void
//Play::startPlay( int idx )
//{
//    int at_sample;
//    double at, dur;
//
//    /* special treatment for ABX test */
//    if ( this_test->test_id == T_ABX && idx == 2 ) {
//        /* re-map X to A or B as indicatd in sys_map table */
//        if ( session->session_info.sys_map[2] == session->session_info.sys_map[0] ) {
//            /* if X is A */
//            idx = 0;
//        }
//        else {
//            /* X must be B */
//            idx = 1;
//        }
//    }
//    
//    paf->next_sys = idx;
//    paf->playing = 1;
//    
//    audioStatus( &at, &dur );
//    at_sample = buff_boundary( paf, at );
//    /* start playing at start */
//    if ( at_sample < paf->start_frame) {
//        paf->next_frame = at_sample;
//    }
//    if ( !paf->loop && (at_sample == paf->stop_frame) ) {
//        paf->next_frame = paf->start_frame;
//    }
//    paf->next_sys = idx;
//
//    /* set timer to animate scroll bar */
//    setScrollTimer( );
//
//#if WIN32
//    /* if video, then play */
//    if ( video->video_session != 0 ) {
//        video->doPlay( );
//    }
//#endif
//}

void Play::alternatePlayPause( void )
{
    if (paf->playing) {
        paf->playing = 0;
    }
    else {
        paf->playing = 1;
    }
#if WIN32
    /* if video, then pause */
    if ( video != NULL ) {
        video->pausePlay( );
    }
#endif
}

void Play::stopPlay( void )
{
    if (paf->playing == 1) {
#if WIN32
        if ( video != NULL ) {
            video->stopPlay( );
        }
#endif
        paf->playing = 0;
        ptf->freeAudioFiles();
    }
}

void
Play::closePlay( void )
{
    /* shut down Port Audio */
    shutdownPa(stream);
}

int Play::audioStatus( double *at, double *duration )
{
    *duration = (double) paf->ru_frames / (double) paf->samplerate;
    *at = (double) paf->next_frame / (double) paf->samplerate;
    return (paf->playing == 1) ? 1 : 0;
}

//int Play::audioPosition( double at )
//{
//    paf->next_frame = (int)buff_boundary( paf, at );
//    return 1;
//}

int Play::audioWindow( double start, double stop )
{
    paf->start_frame = (int)buff_boundary( paf, start );
    paf->stop_frame = (int)buff_boundary( paf, stop );
    if (paf->next_frame < paf->start_frame) {
        paf->next_frame = paf->start_frame;
    }
    if (paf->next_frame > paf->stop_frame) {
        paf->next_frame = paf->stop_frame;
    }
    return 1;
}

/* helper functions */
static int
buff_boundary( AudioFiles *paf, double time )
{
    int frame = (int)( time * paf->samplerate );
    /* force to audio output buffer boundary */
    int i = frame / paf->prefSize;
    int j = frame % paf->prefSize;
    /* round to nearest buffer boundary */
    if (j > paf->prefSize/2) {
        i += 1;
    }
    frame = i * paf->prefSize;
    /* force position to be within signal range */
    frame = (frame < 0) ? 0 : frame; //max( 0, sample );
    frame = (frame > (int)paf->ru_frames) ? (int)paf->ru_frames : frame;
    return frame;
}

/* PortAudio callback */
int paCallback(
    const void *inputBuffer,    //not used
    void *outputBuffer,         //interleaved output signal
    unsigned long framesPerBuffer,
    const PaStreamCallbackTimeInfo* timeInfo,
    PaStreamCallbackFlags statusFlags,
    void *userData)
{
    float *out = (float *)outputBuffer;
    AudioFiles *paf = (AudioFiles *)userData;

    /* copy variables from callback data structure */
    int channels = paf->channels;
    int next_frame = paf->next_frame;
    int start_frame = paf->start_frame;
    int stop_frame = paf->stop_frame;
    int playing = paf->playing;
    int loop = paf->loop;
    int prev_sys = paf->prev_sys;
    int next_sys = paf->next_sys;

    if (playing == 0) {
        /* paused, so just playout zeros */
        memset(out, 0.0, channels*framesPerBuffer*sizeof(AUDIO_SAMP));
        return 0;
    }

    if (prev_sys == next_sys) {
        /* copy next buf to output */
        SysFiles *pns = &paf->sys_files[next_sys];
        trans_buff->copyBuf(out, pns, next_frame, channels, framesPerBuffer);
    }
    else {
        /* transition output from prev to next */
        SysFiles *pps = &paf->sys_files[prev_sys];
        SysFiles *pns = &paf->sys_files[next_sys];
        int done;
        if ( (done = trans_buff->transitionBuf(out, pps, pns,
            next_frame, channels, framesPerBuffer)) == 1 ) {
            /* transition complete, so update prev_sys */
            paf->prev_sys = next_sys;
        };
    }
#if (DEBUG_PLY)
    if (debug_blk < DEBUG_NUM_BUF) {
        if (debug_blk == 1) {
            paf->next_sys = 1; //initiate transition
        }
        for (int i=0; i<framesPerBuffer; i++) {
            for (int j=0; j<channels; j++) {
                debug_out[debug_blk*channels*(int)framesPerBuffer + i*channels + j] = out[i*channels + j];
            }
        }
        debug_blk++;
        if (debug_blk == DEBUG_NUM_BUF)
            write_buf_ply(debug_out, 48000, channels, DEBUG_NUM_BUF*(int)framesPerBuffer, "debug_out.wav");
    }
#endif
    
    /* update next_frame with looping */
    next_frame += framesPerBuffer;
    if (next_frame < stop_frame) {
        /* update */
        paf->next_frame = next_frame;
    }
    else {
        /* loop */
        paf->next_frame = start_frame;
        if (loop == 0) {
            /* if no looping, then stop playing */
            paf->playing = 0;
        }
    }
        
    return 0;
}
