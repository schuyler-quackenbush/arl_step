#ifndef __PAUTILS_H
#define __PAUTILS_H

#include "portaudio.h"

/* Port Audio Utilities */
int pa_get_audio_dev(PaDeviceInfo *deviceInfo, int *numDevices, int *defaultDevice);

PaStream *startupPa(int inputChanCount, int outputChanCount, 
    int samp_rate, int frames_per_buffer, PaStreamCallback *paCallback, void *data);

void shutdownPa(PaStream *stream);

#endif //__PAUTILS_H
