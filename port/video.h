#ifndef __VIDEO_H
#define __VIDEO_H

/* video class */

#include <stdio.h> /* for FILE */
#include "defineValues.h"

/* bit map headers */
typedef struct tagBitMapFileHeader {
        short			bfType;		/* BM or 0x4D42 */
        unsigned long	bfSize;		/* size of BMP file, bytes */
        short			bfReserved1;
        short			bfReserved2;
        unsigned long	bfOffBits;	/* offset to BMP data, bytes */
} BitMapFileHeader;
#define BitMapFileHeaderLen	14

typedef struct tagBitMapInfoHeader{
        long	biSize;
        long	biWidth;
        long	biHeight;
        short	biPlanes;
        short	biBitCount;
        long	biCompression;
        long	biSizeImage;
        long	biXPelsPerMeter;
        long	biYPelsPerMeter;
        long	biClrUsed;
        long	biClrImportant;
} BitMapInfoHeader;

typedef struct tagRGBQuad {
        char    rgbBlue;
        char    rgbGreen;
        char    rgbRed;
        char    rgbReserved;
} RGBQuad;

//The above provide a portable equivalent to the following
//Windows struct
//typedef struct tagBITMAPINFO {
//    BITMAPINFOHEADER    bmiHeader;
//    RGBQUAD             bmiColors[1];
//} BITMAPINFO, FAR *LPBITMAPINFO, *PBITMAPINFO;

/* defines */
#define	MAX_TRIALS	64

/* file types */
#define FMT_BMP	1
#define FMT_YUV	2

/* RGB order */
#define BMP_RGB	1
#define BMP_BGR	2

/* optimization levels for yuv to rgb conversion */
#define YUV_FLOAT			0	/* floating point math */
#define YUV_INTEGER			1	/* integer math */
#define YUV_TABLE			2	/* table lookup math */
#define YUV_TABLE_4PIXELS	3	/* table lookup math in main loop */
/* 24 fps is 
 *	42 ms per frame
 * Optimization			720p	1080p
 * YUV_TABLE			20		48	ms per Frame
 * YUV_TABLE_4PIXELS	15		32	ms per Frame
 */
#define YUV_OPTIM	YUV_TABLE_4PIXELS 

typedef struct tagVideoFile {
	char video_filename[FILE_LEN];	/* video files for this session */
	char video_label[LAB_LEN];		/* label for each video file */
} VideoFile;

class Video {
public:
	Video();
	~Video();
	int openVideoSession( char *video_session_file, int ntrials );
	void closeVideoSession( void );
	int nextVideo( int trial );
	int readVideoHeader( void );
	int readDibHeader( void );
	int createDibHeader( void );
	int readFrame( void );
	void getFrame( int frame_num );
	void doPlay( void );
	void pausePlay( void );
	void stopPlay( void );
	unsigned long videoPosition( double at );

	int video_session;	/* flag to signal this session has video */
	int fmt;		/* file format (bmp or yuv) */
	long height;		/* frame height, pixels */
	long width;		/* frame width, pixels */
	long fsize;		/* frame size, pixels */
	double fps;		/* frames per second (can have fractional part) */
	int ntrials;	/* number of trials (from Session) */
	VideoFile *videoFiles;
	FILE *fp;		/* video file */
#if WIN32
	__int64 fileSize;	/* file size, bytes */
	__int64 vdOffset;	/* offset to video data, bytes */
#else //Apple
	int64_t fileSize;	/* file size, bytes */
	int64_t vdOffset;	/* offset to video data, bytes */
#endif
	unsigned long nFrames;	/* number of video frames in file */
	BitMapInfoHeader bmih;	/* bit map info header */
	RGBQuad rgbQuad;		/* RGB quad array */
	unsigned char *ibuf;	/* input buffer */
	unsigned char *fbuf;	/* frame buffer */
	int bmOrder;
	int playing;	/* flag to signal playing is active */
	int frameNum;	/* frame currently in frame buffer, indexed from 1 */
};

/* function prototypes */
void convert_ybr_rgb(unsigned char *ibuf, unsigned char *obuf,
		long width, long height, int bmp_order);
void convert_ybr_rgb_integer(unsigned char *ibuf, unsigned char *obuf,
	int width, int height, int bmp_order);
void convert_ybr_rgb_table(unsigned char *ibuf, unsigned char *obuf,
	int width, int height, int bmp_order);
int create_yuv_tables( void );

#endif /* __VIDEO_H */