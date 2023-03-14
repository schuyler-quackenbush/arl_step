#include <stdio.h>
#include <string.h>
#include "wav.h"

#ifdef _MSC_VER
#include <fcntl.h>
#include <io.h>
#else
#define setmode(x, y)
#endif

#define big_endian (*((short *) "\1\2") == 0x0102)

#define WAVE_FORMAT_PCM	0x0001
#define WAVE_FORMAT_EXT	0xFFFE
static unsigned char GUID[] = { 0,0,0,0,0x10,0,0x80,0,0,0xAA,0,0x38,0x9B,0x71 };

/*
 * Copyright (c) 1999 Apple Computer, Inc. All rights reserved.
 *
 * @APPLE_LICENSE_HEADER_START@
 * 
 * The contents of this file constitute Original Code as defined in and
 * are subject to the Apple Public Source License Version 1.1 (the
 * "License").  You may not use this file except in compliance with the
 * License.  Please obtain a copy of the License at
 * http://www.apple.com/publicsource and read it before using this file.
 * 
 * This Original Code and all software distributed under the License are
 * distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY KIND, EITHER
 * EXPRESS OR IMPLIED, AND APPLE HEREBY DISCLAIMS ALL SUCH WARRANTIES,
 * INCLUDING WITHOUT LIMITATION, ANY WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE OR NON-INFRINGEMENT.  Please see the
 * License for the specific language governing rights and limitations
 * under the License.
 * 
 * @APPLE_LICENSE_HEADER_END@
 */
/*
 * Copyright (c) 1988, 1993
 *	The Regents of the University of California.  All rights reserved.
 *
 * This code is derived from software contributed to Berkeley by
 * Jeffrey Mogul.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *	This product includes software developed by the University of
 *	California, Berkeley and its contributors.
 * 4. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */


#include <string.h>

void
my_swap(const void *from, void *to, size_t len)
	//const void *from;
	//void *to;
	//size_t len;
{
	unsigned long temp;
	unsigned long n;
	char *fp, *tp;

	n = (len >> 1) + 1;
	fp = (char *)from;
	tp = (char *)to;
#define	STEP	temp = *fp++,*tp++ = *fp++,*tp++ = temp
	/* round to multiple of 8 */
	while ((--n) & 07)
		STEP;
	n >>= 3;
	while (--n >= 0) {
		STEP; STEP; STEP; STEP;
		STEP; STEP; STEP; STEP;
	}
}

static int
getshort( unsigned char *p )
{
    return p[0] + (p[1]<<8);
}

static int
getlong( unsigned char *p )
{
    return p[0] + (p[1]<<8) + (p[2]<<16) + (p[3]<<24);
}

static int
putstr( unsigned char *p, char *s )
{
	strcpy( (char *)p, s );
	return (int)strlen( s );
}

static int
putlong( unsigned char *p, int n )
{
	p[0] = (unsigned char) n;
	p[1] = (unsigned char) (n>>8);
	p[2] = (unsigned char) (n>>16);
	p[3] = (unsigned char) (n>>24);
	return 4;
}

static int
putshort( unsigned char *p, int n )
{
	p[0] = (unsigned char) n;
	p[1] = (unsigned char) (n>>8);
	return 2;
}

static int chan_mask[] = { 
	0, 0x01, 0x03, 0x07, 0x33, 0x37, 0x3f, 0x637, 0x63f, 0x73f
};

int
writeWavHeader( WAVHDR *wp )
{
	unsigned char hdr[68], *p;
    int fmt, fsize, xsize;
    unsigned long dsize;

	if( wp->fp == 0 )
		return -1;
	xsize = 20;
	if( wp->fmt == '0' || (wp->fmt != '1' && wp->channels <= 2 && wp->ssize <= 2) ) {
		fmt = WAVE_FORMAT_PCM;
		fsize = 16;
	}
	else {
		fmt = WAVE_FORMAT_EXT;
		fsize = 40;
	}
	dsize = wp->nsamp*wp->ssize;
	p = hdr;
	p += putstr( p, (char*)"RIFF" );
	p += putlong( p, (int)dsize+fsize+xsize ); /* size of complete file less 8 */
	p += putstr( p, (char*)"WAVEfmt " );
	p += putlong( p, fsize );  /* size of fmt chunk */
	p += putshort( p, fmt );
	p += putshort( p, wp->channels );
	p += putlong( p, wp->samplerate );
	p += putlong( p, wp->samplerate*wp->channels*wp->ssize ); /* bytes per sec */
	p += putshort( p, wp->channels*wp->ssize );  /* block align size */
	p += putshort( p, 8*wp->ssize );    /* bits per sample */
	if( fmt == WAVE_FORMAT_EXT ) {
		p += putshort( p, 22 );		/* extension size */
		p += putshort( p, 8*wp->ssize );    /* used bits per sample */
		p += putlong( p, chan_mask[wp->channels] ); /* speaker mask */
		p += putshort( p, WAVE_FORMAT_PCM );
		memcpy(p, GUID, sizeof(GUID));
		p += sizeof(GUID);
	}
	p += putstr( p, (char*)"data" );
	p += putlong( p, (int)dsize );
	wp->start = (int)(p-hdr);
	wp->pos = 0;
	wp->wcnt = 0;
	return fwrite( hdr, 1, p-hdr, wp->fp ) ==  p-hdr ? 0 : -1;
}

int
readWavHeader( WAVHDR *wp )
{
    unsigned char hdr[60];
	int fsize, fmt;
    if( fread( hdr, 1, 12, wp->fp ) != 12 ||
            memcmp( "RIFF", hdr, 4 ) != 0 ||
            memcmp( "WAVE", hdr+8, 4 ) != 0 )
		return -1;
    for(;;) {
        int d;
        if( fread( hdr, 1, 8, wp->fp ) != 8 )
            return -1;
        if( memcmp( "fmt ", hdr, 4 ) == 0 )
            break;
        d = getlong( hdr+4 );
        if( fseek( wp->fp, d, SEEK_CUR ) == -1 )
            return -1;
    }
	fsize = getlong( hdr+4 );
	if( fsize > 40 || fread( &hdr[20], 1, fsize, wp->fp ) != fsize )
		return -2;
	fmt = getshort( hdr+20 );
	if( fmt != WAVE_FORMAT_PCM && 
			(fmt != WAVE_FORMAT_EXT ||
			 getshort( hdr+44 ) != WAVE_FORMAT_PCM ) )
			/* || memcmp( hdr+46, GUID, sizeof(GUID) ) != 0) */
		return -3;
    wp->channels = getshort( hdr+22 );
	wp->vch = wp->channels;
	if( wp->channels == 0 )
		return -4;
    wp->samplerate = getlong( hdr+24 );
    /* avg_bit_rate = getlong( hdr+28 ); */
    /* block_align = getshort( hdr+32 ); */
    wp->ssize = (getshort( hdr+34 )+7)/8;
	if( wp->ssize == 0 )
		return -5;
	if (fsize == 40) {
		wp->channelMask = getlong(hdr+40);
	}
	else {
		wp->channelMask = 0;
	}

    for(;;) {
        int d;
        if( fread( hdr, 1, 8, wp->fp ) != 8 )
            return -6;
        if( memcmp( "data", hdr, 4 ) == 0 )
            break;
        d = getlong( hdr+4 );
        if( fseek( wp->fp, d, SEEK_CUR ) == -1 )
            return -7;
    }
    wp->nsamp = getlong( hdr+4 )/wp->ssize;
    wp->frames = wp->nsamp/wp->channels;
	wp->start = (int)ftell( wp->fp );
	wp->pos = 0;
	wp->wcnt = -1;
	return 0;
}

char *
openWavFile( const char *file, const char *fmt, WAVHDR *wp )
{
	if( *fmt == 'r' ) {
		if( strcmp( file, "-" ) == 0 ) {
			wp->fp = stdin;
			setmode(fileno(stdin), O_BINARY);
		}
		else if( (wp->fp = fopen( file, "rb" )) == 0 )
			return (char*)"can't open file";
		if( fmt[1] == 'r' ) {
			/* raw - no wav header */
			wp->start = 0;
			wp->pos = 0;
			wp->wcnt = -1;
			wp->nsamp = 0x7fffffff;
			/* user expected to fill out rest */
		}
		else if( readWavHeader( wp ) != 0 ) {
			fclose( wp->fp );
			wp->fp = 0;
			return (char*)"invalid wav file";
		}
		return 0;
	}
	else if( *fmt == 'w' ) {
		if( strcmp( file, "-" ) == 0 ) {
			wp->fp = stdout;
			setmode(fileno(stdout), O_BINARY);
		}
		else if( (wp->fp = fopen( file, "wb" )) == 0 )
			return (char*)"can't create file";
		wp->fmt = fmt[1];
		if( wp->fmt == 'r' ) {
			/* raw - no wav header */
			wp->start = 0;
			wp->pos = 0;
			wp->wcnt = 0;
		}
		else if( writeWavHeader( wp ) != 0 )
			return (char*)"can't write wav header";
		return 0;
	}
	return (char*)"invalid argument";
}

int
closeWavFile( WAVHDR *wp )
{
	if( wp->fp == 0 )
		return -1;
	if( wp->wcnt != -1 && wp->wcnt != wp->nsamp && wp->fmt != 'r' ) {
		wp->nsamp = wp->wcnt;
		if( fseek( wp->fp, 0, SEEK_SET ) == 0 )
			writeWavHeader( wp );
	}
	fclose( wp->fp );
	wp->fp = 0;
	return 0;
}

int
seekWavFile( WAVHDR *wp, int nsamp )
{
	if( wp->channels == 1 && wp->vch == 2 ) /* mono to stereo hack */
		nsamp >>= 1;
	if( wp->wcnt != -1 || nsamp < 0 || nsamp > wp->nsamp ||
			fseek( wp->fp, nsamp*wp->ssize+wp->start, SEEK_SET ) != 0 )
		return -1;
	wp->pos = nsamp;
	return 0;
}

unsigned long
readWavVoid( WAVHDR *wp, void *data, int bytes )
{
	unsigned long nsamp;
	nsamp = bytes/wp->ssize;
	if( wp->pos + nsamp > wp->nsamp )
		nsamp = wp->nsamp - wp->pos;
	if( (nsamp = fread( data, wp->ssize, nsamp, wp->fp )) > 0 )
		wp->wcnt += nsamp;
	return nsamp*wp->ssize;
}

unsigned long
readWavShort( WAVHDR *wp, short *data, int nsamp )
{
	unsigned char buf[2*3*1024], *bp, *ebp;
	short *dp = data;
	unsigned long n = 0;
	int dsize;
	int ssize;
	if( wp->pos + nsamp > wp->nsamp )
		nsamp = (int)(wp->nsamp - wp->pos);
	ssize = wp->ssize;
	switch( ssize ) {
	case 2:
		n = fread( data, ssize, nsamp, wp->fp );
		if( big_endian )
			my_swap( data, data, n*ssize );
		break;
	case 3:
	case 4:
		dsize = nsamp*ssize;
		while( dsize > 0 ) {
			n = dsize > sizeof(buf) ? sizeof(buf) : dsize;
			if( (n = fread( buf, 1, n, wp->fp )) <= 0 )
				break;
			dsize -= n;
			ebp = buf + n;
			bp = buf + ssize - 3;
			while( bp < ebp ) {
				*dp++ = (bp[1] | (bp[2]<<8)) + (bp[0]>>7);
				bp += ssize;
			}
		}
		n = dp - data;
		break;
	}
	wp->pos += n;
	return n;
}

unsigned long
readWavLong( WAVHDR *wp, long *data, int nsamp )
{
	unsigned char buf[2*3*1024], *bp, *ebp;
	long *dp = data;
    unsigned long n, i, j;
	int dsize;
	if( wp->channels == 1 && wp->vch == 2 ) { /* mono to stereo hack */
		nsamp >>= 1;
	}
	if( wp->pos + nsamp > wp->nsamp )
		nsamp = (int)(wp->nsamp - wp->pos);
	dsize = nsamp*wp->ssize;
	switch( wp->ssize ) {
	case 2:
		while( dsize > 0 ) {
			n = dsize > sizeof(buf) ? sizeof(buf) : dsize;
			if( (n = fread( buf, 1, n, wp->fp )) <= 0 )
				break;
			dsize -= n;
			ebp = buf + n;
			bp = buf;
			while( bp < ebp ) {
				*dp++ = (bp[0]<<16) | (bp[1]<<24);
				bp += 2;
			}
		}
		break;
	case 3:
		while( dsize > 0 ) {
			n = dsize > sizeof(buf) ? sizeof(buf) : dsize;
			if( (n = fread( buf, 1, n, wp->fp )) <= 0 )
				break;
			dsize -= n;
			ebp = buf + n;
			bp = buf;
			while( bp < ebp ) {
				*dp++ = (bp[0]<<8) | (bp[1]<<16) | (bp[2]<<24);
				bp += 3;
			}
		}
		break;
	case 4:
		while( dsize > 0 ) {
			n = dsize > sizeof(buf) ? sizeof(buf) : dsize;
			if( (n = fread( buf, 1, n, wp->fp )) <= 0 )
				break;
			dsize -= n;
			ebp = buf + n;
			bp = buf;
			while( bp < ebp ) {
				*dp++ = bp[0] | (bp[1]<<8) | (bp[2]<<16) | (bp[3]<<24);
				bp += 4;
			}
		}
		break;
	}
	n = dp - data;
	wp->pos += n;
	if( wp->channels == 1 && wp->vch == 2 ) { /* mono to stereo hack */
		for( j=n-1, i=2*n-1; j>=0; i-=2, j-- )
			data[i] = data[i-1] = data[j];
		n *= 2;
	}
	return n;
}

#define FAC16	(1.0/2147418112.0)
#define FAC24	(1.0/2147483392.0)
#define FAC32	(1.0/2147483647.0)

unsigned long
readWavDouble( WAVHDR *wp, double *data, int nsamp )
{
	unsigned char buf[2*3*1024], *bp, *ebp;
	double *dp = data;
	unsigned long n;
	int dsize;
	if( wp->pos + nsamp > wp->nsamp )
		nsamp = (int)(wp->nsamp - wp->pos);
	dsize = nsamp*wp->ssize;
	switch( wp->ssize ) {
	case 2:
		while( dsize > 0 ) {
			n = dsize > sizeof(buf) ? sizeof(buf) : dsize;
			if( (n = fread( buf, 1, n, wp->fp )) <= 0 )
				break;
			dsize -= n;
			ebp = buf + n;
			bp = buf;
			while( bp < ebp ) {
				*dp++ = ((bp[0]<<16) | (bp[1]<<24))*FAC16;
				bp += 2;
			}
		}
		break;
	case 3:
		while( dsize > 0 ) {
			n = dsize > sizeof(buf) ? sizeof(buf) : dsize;
			if( (n = fread( buf, 1, n, wp->fp )) <= 0 )
				break;
			dsize -= n;
			ebp = buf + n;
			bp = buf;
			while( bp < ebp ) {
				*dp++ = ((bp[0]<<8) | (bp[1]<<16) | (bp[2]<<24))*FAC24;
				bp += 3;
			}
		}
		break;
	case 4:
		while( dsize > 0 ) {
			n = dsize > sizeof(buf) ? sizeof(buf) : dsize;
			if( (n = fread( buf, 1, n, wp->fp )) <= 0 )
				break;
			dsize -= n;
			ebp = buf + n;
			bp = buf;
			while( bp < ebp ) {
				*dp++ = (bp[0] | (bp[1]<<8) | (bp[2]<<16) | (bp[3]<<24))*FAC32;
				bp += 4;
			}
		}
		break;
	}
	n = dp - data;
	wp->pos += n;
	return n;
}

unsigned long
readWavFloat( WAVHDR *wp, float *data, int nsamp )
{
	unsigned char buf[2*3*1024], *bp, *ebp;
	float *dp = data;
	unsigned long n;
	int dsize;
	if( wp->pos + nsamp > wp->nsamp )
		nsamp = (int)(wp->nsamp - wp->pos);
	dsize = nsamp*wp->ssize;
	switch( wp->ssize ) {
	case 2:
		while( dsize > 0 ) {
			n = dsize > sizeof(buf) ? sizeof(buf) : dsize;
			if( (n = fread( buf, 1, n, wp->fp )) <= 0 )
				break;
			dsize -= n;
			ebp = buf + n;
			bp = buf;
			while( bp < ebp ) {
				*dp++ = ((bp[0]<<16) | (bp[1]<<24))*FAC16;
				bp += 2;
			}
		}
		break;
	case 3:
		while( dsize > 0 ) {
			n = dsize > sizeof(buf) ? sizeof(buf) : dsize;
			if( (n = fread( buf, 1, n, wp->fp )) <= 0 )
				break;
			dsize -= n;
			ebp = buf + n;
			bp = buf;
			while( bp < ebp ) {
				*dp++ = ((bp[0]<<8) | (bp[1]<<16) | (bp[2]<<24))*FAC24;
				bp += 3;
			}
		}
		break;
	case 4:
		while( dsize > 0 ) {
			n = dsize > sizeof(buf) ? sizeof(buf) : dsize;
			if( (n = fread( buf, 1, n, wp->fp )) <= 0 )
				break;
			dsize -= n;
			ebp = buf + n;
			bp = buf;
			while( bp < ebp ) {
				*dp++ = (bp[0] | (bp[1]<<8) | (bp[2]<<16) | (bp[3]<<24))*FAC32;
				bp += 4;
			}
		}
		break;
	}
	n = dp - data;
	wp->pos += n;
	return n;
}

unsigned long
writeWavVoid( WAVHDR *wp, void *data, int bytes )
{
	unsigned long n;
	if( (n = fwrite( data, 1, bytes, wp->fp )) > 0 )
		wp->wcnt += n/wp->ssize;
	return n;
}

unsigned long
writeWavShort( WAVHDR *wp, short *data, int nsamp )
{
	unsigned char buf[2*3*1024], *bp, *ebp;
	short *dp = data, d;
	int n;
	int dsize;
	switch( wp->ssize ) {
	case 2:
		if( big_endian )
			my_swap( data, data, nsamp*2 );
		if( fwrite( data, 2, nsamp, wp->fp ) != nsamp )
			return -1;
		wp->wcnt += nsamp;
		return nsamp;
	case 3:
		dsize = nsamp*wp->ssize;
		while( dsize > 0 ) {
			n = dsize > sizeof(buf) ? sizeof(buf) : dsize;
			dsize -= n;
			ebp = buf + n;
			bp = buf;
			while( bp < ebp ) {
				d = *dp++;
				bp[0] = 0;
				bp[1] = d;
				bp[2] = d>>8;
				bp += 3;
			}
			if( fwrite( buf, 1, n, wp->fp ) != n )
				return -1;
		}
		wp->wcnt += nsamp;
		return nsamp;
	}
	return 0;
}

unsigned long
writeWavLong( WAVHDR *wp, long *data, int nsamp )
{
	unsigned char buf[2*3*1024], *bp, *ebp;
	long *dp = data, d;
	int n;
	int dsize;
	dsize = nsamp*wp->ssize;
	switch( wp->ssize ) {
	case 2:
		while( dsize > 0 ) {
			n = dsize > sizeof(buf) ? sizeof(buf) : dsize;
			dsize -= n;
			ebp = buf + n;
			bp = buf;
			while( bp < ebp ) {
				d = *dp++;
				bp[0] = d>>16;
				bp[1] = d>>24;
				bp += 2;
			}
			if( fwrite( buf, 1, n, wp->fp ) != n )
				return -1;
		}
		wp->wcnt += nsamp;
		return nsamp;
	case 3:
		while( dsize > 0 ) {
			n = dsize > sizeof(buf) ? sizeof(buf) : dsize;
			dsize -= n;
			ebp = buf + n;
			bp = buf;
			while( bp < ebp ) {
				d = *dp++;
				bp[0] = d>>8;
				bp[1] = d>>16;
				bp[2] = d>>24;
				bp += 3;
			}
			if( fwrite( buf, 1, n, wp->fp ) != n )
				return -1;
		}
		wp->wcnt += nsamp;
		return nsamp;
	}
	return 0;
}

unsigned long
writeWavDouble( WAVHDR *wp, double *data, int nsamp )
{
	unsigned char buf[2*3*1024], *bp, *ebp;
	double *dp = data, d;
	short s;
	unsigned long i;
	int n;
	int dsize;
	dsize = nsamp*wp->ssize;
	switch( wp->ssize ) {
	case 2:
		while( dsize > 0 ) {
			n = dsize > sizeof(buf) ? sizeof(buf) : dsize;
			dsize -= n;
			ebp = buf + n;
			bp = buf;
			while( bp < ebp ) {
				d = 32767.0 * *dp++;
				s = (short) (d < 0 ? d-.5 : d+.5);
				bp[0] = s;
				bp[1] = s>>8;
				bp += 2;
			}
			if( fwrite( buf, 1, n, wp->fp ) != n )
				return -1;
		}
		wp->wcnt += nsamp;
		return nsamp;
	case 3:
		while( dsize > 0 ) {
			n = dsize > sizeof(buf) ? sizeof(buf) : dsize;
			dsize -= n;
			ebp = buf + n;
			bp = buf;
			while( bp < ebp ) {
				d = 8388607.0 * *dp++;
				i = (long) (d < 0 ? d-.5 : d+.5);
				bp[0] = i;
				bp[1] = i>>8;
				bp[2] = i>>16;
				bp += 3;
			}
			if( fwrite( buf, 1, n, wp->fp ) != n )
				return -1;
		}
		wp->wcnt += nsamp;
		return nsamp;
	}
	return 0;
}

unsigned long
writeWavFloat( WAVHDR *wp, float *data, int nsamp )
{
	unsigned char buf[2*3*1024], *bp, *ebp;
	float *dp = data, d;
	short s;
	unsigned long i;
	int n;
	int dsize;
	dsize = nsamp*wp->ssize;
	switch( wp->ssize ) {
	case 2:
		while( dsize > 0 ) {
			n = dsize > sizeof(buf) ? sizeof(buf) : dsize;
			dsize -= n;
			ebp = buf + n;
			bp = buf;
			while( bp < ebp ) {
				d = 32767.0 * *dp++;
				s = (short) (d < 0 ? d-.5 : d+.5);
				bp[0] = s;
				bp[1] = s>>8;
				bp += 2;
			}
			if( fwrite( buf, 1, n, wp->fp ) != n )
				return -1;
		}
		wp->wcnt += nsamp;
		return nsamp;
	case 3:
		while( dsize > 0 ) {
			n = dsize > sizeof(buf) ? sizeof(buf) : dsize;
			dsize -= n;
			ebp = buf + n;
			bp = buf;
			while( bp < ebp ) {
				d = 8388607.0 * *dp++;
				i = (long) (d < 0 ? d-.5 : d+.5);
				bp[0] = i;
				bp[1] = i>>8;
				bp[2] = i>>16;
				bp += 3;
			}
			if( fwrite( buf, 1, n, wp->fp ) != n )
				return -1;
		}
		wp->wcnt += nsamp;
		return nsamp;
	}
	return 0;
}
