#ifndef __PLAY_H
#define __PLAY_H

#include "session.h"
#include "trialAudioFiles.h"

class Play {
public:
	Play( );
	~Play( );
//    int get_audio_output_devices(char **device_name, int *num_devices, int *default_device);
	int initPlay( Session *p1, TrialAudioFiles *p2, int trans_mode );
//    void startPlay( int id );
    void alternatePlayPause( void );
    void stopPlay( void );
    void closePlay( void );
    int audioStatus( double *at, double *dur );
//    int audioPosition( double at );
    int audioWindow( double start, double stop );

	Session *session;
    TrialAudioFiles *ptf;
	AudioFiles *paf;
};

#endif /* __PLAY_H */
