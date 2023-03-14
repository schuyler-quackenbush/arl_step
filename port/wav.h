#ifndef __WAV_H
#define __WAV_H

#include <stdio.h>

typedef struct {
	int channels;
	int samplerate;
	int ssize;
	unsigned long nsamp; //frames * channels
    unsigned long frames;
	unsigned long pos;
	int wcnt;
	int start;
	char fmt;
	unsigned long channelMask;
	FILE *fp;
	int vch;
} WAVHDR;

char *openWavFile( const char *file, const char *fmt, WAVHDR *wp );
int closeWavFile( WAVHDR *wp );
int seekWavFile( WAVHDR *wp, int nsamp );

unsigned long writeWavVoid( WAVHDR *wp, void *data, int bytes );
unsigned long writeWavDouble( WAVHDR *wp, double *data, int nsamp );
unsigned long writeWavFloat( WAVHDR *wp, float *data, int nsamp );
unsigned long writeWavLong( WAVHDR *wp, long *data, int nsamp );
unsigned long writeWavShort( WAVHDR *wp, short *data, int nsamp );

unsigned long readWavVoid( WAVHDR *wp, void *data, int bytes );
unsigned long readWavDouble( WAVHDR *wp, double *data, int nsamp );
unsigned long readWavFloat( WAVHDR *wp, float *data, int nsamp );
unsigned long readWavLong( WAVHDR *wp, long *data, int nsamp );
unsigned long readWavShort( WAVHDR *wp, short *data, int nsamp );

int readWavHeader( WAVHDR *wp );
int writeWavHeader( WAVHDR *wp );

#endif
