#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "portControls.h"
#include "defineValues.h"
#include "utils.h"
#include "video.h"

#define STARTER		'#'
#define LINE_LEN	1024
#define LABEL_LEN	32

/* local function prototypes */
static int getLine( FILE *fp, char *line );
static char *skip_whitespace(char *p);

/* class constructor */
Video::Video( )
{
	video_session = 0;
	playing = 0;
	ibuf = NULL;
	fbuf = NULL;
}

/* class desctructor */
Video::~Video()
{
	playing = 0;
	my_free(ibuf);
	my_free(fbuf);
}

/* member functions */
int 
Video::openVideoSession( char *video_session_file, int num_trials )
{
	int i, j;
	char line[LINE_LEN], *attribute, *value, *p;
	FILE *sfp;
	/* 
	video session file must match audio session file
	Video session file structure is:
		session=video
		format=yuv (or = bmp)
		height=123
		width=456
		fps=30
		# Video_Files
		file1.yuv (or .bmp]
		file2.yuv (or .bmp]
		file3.yuv (or .bmp]
	where header parameters are shared by all video files and
	number of video files must equal number of audio signals
	*/

	/* open session file */
	if ( (sfp = fopen(video_session_file, "r")) == NULL) {
		/* cannot open session file */
		fclose( sfp );
		return -1;
	}

	/* set number of trials from openSession() */
	ntrials = num_trials;
	videoFiles = (VideoFile *) malloc( ntrials * sizeof(*videoFiles) );
	
	/* set defaults */
	fmt = 0;
	height = 0;
	width = 0;
	fps = 0;
	frameNum = -1;

	/* read session file header */
	while (1) {
		if( getLine( sfp, line ) == 0 ) {
			fclose( sfp );
			return -1; /* error */
		}
		/* STARTER ends session header */
		if( *line == STARTER ) {
			break;
		}
		/* header lines in form of value=attribute */
		attribute = line;
		if( (p = strchr( line, '=' )) != 0 ) {
			value = &p[1];
			*p = 0;
			/* header lines */
			if ( my_strnicmp( attribute, "format", 6 ) == 0 ) {
				if( my_strnicmp( value, "yuv", 3 ) == 0 ) {
					fmt = FMT_YUV;
				}
				else if ( my_strnicmp( value, "bmp", 3 ) == 0 ) {
					fmt = FMT_BMP;
				}
			}
			else if ( my_strnicmp( attribute, "height", 6 ) == 0 ) {
				this->height = atoi(value);
			}
			else if ( my_strnicmp( attribute, "width", 5 ) == 0 ) {
				this->width = atoi(value);
			}
			else if ( my_strnicmp( attribute, "fps", 3 ) == 0 ) {
				this->fps = atof(value);
			}
		}
	}

	/* read session file body */
	for (i=0; i<ntrials; i++) {
		if ( getLine( sfp, line ) == 0 )
			break;
		if (line[0] == '\0')
			break;
		/* read filename */
			p = line;
			for (j=0; ( (*p != 0 && *p != ' ' && *p != '\t') || j==FILE_LEN ); j++) {
				videoFiles[i].video_filename[j] = *p++;
			}
			videoFiles[i].video_filename[j] = 0; /* NULL terminate */
		/* read optional label */
		if (*p != 0) {
			p++;
			/* read system label */
			p = skip_whitespace(p);
			my_strncpy(videoFiles[i].video_label, p, LAB_LEN);
		}
		else {
			/* copy in NULL string */
			my_strncpy(videoFiles[i].video_label, "", LAB_LEN);
		}
	}
	if (i != ntrials) {
		showError((char*)"Number of video files does not match number of session Trials");
		return -1;
	}

	fclose( sfp );

	video_session = 1; /* signal that video session is active */

	return 0;
}

void
Video::closeVideoSession( void )
{
	playing = 0;
	my_free(ibuf);
	my_free(fbuf);
}

int 
Video::nextVideo( int trial )
{
	if ( (fp = fopen(videoFiles[trial].video_filename, "rb")) == NULL ) {
		showError((char*)"Cannot open video file");
		return -1;
	}
	readVideoHeader();
	return 0;
}

int
Video::readVideoHeader( void )
{
	unsigned long n;

	/* determine file size in bytes */
	_fseeki64(fp, 0, SEEK_END); //fseek()
	fileSize = _ftelli64(fp); // ftell(fp);
	rewind(fp);

	switch (fmt) {
		case FMT_BMP:
			bmOrder = BMP_RGB;
			readDibHeader();
			break;
		case FMT_YUV:
			bmOrder = BMP_BGR;
			createDibHeader();
			break;
		default:
			/* error message */
			return -1;
	}
	if (height == 0 || width == 0 || fps == 0) {
		/* error message */
		return -1;
	}
	if ( fmt == FMT_YUV ) {
		if ( height%4 != 0 || width%4 != 0) {
			/* error message */
			return -1;
		}
	}

	/* allocate for one video frame */
	n = height*width*3;
	if ( (fbuf = (unsigned char *)malloc(n)) == NULL) {
		/* cannot malloc frame buffer */
		return -1;
	}
	/* zero frame buffer */
	memset(fbuf, 0, n);
	/* if YUV then */
	if ( fmt == FMT_YUV ) {
		/* allocate yuv frame buffer storage (1.5 bytes per pixel) */
		if ( (ibuf = (unsigned char *)malloc(height*width*3/2)) == NULL ) {
			/* cannot malloc frame buffer */
			return -1;
		}
		/* create fast table-based multiplications */
		create_yuv_tables( );
	}
	return 0;
}

int
Video::readDibHeader( void )
{
	size_t count;
	unsigned char header[BitMapFileHeaderLen];

	/* read bit map file header */
	if ( (count = fread(header, BitMapFileHeaderLen, 1, fp)) != 1 )  {
		return -1;
	}
	if ( !(header[0] == 'B' && header[1] == 'M') ) { /* "BM" */
		/* not a bit map file */
		return -1;
	}
	/* file size */ 
	//following is only true for a bmp image consisting of one frame
	//bfSize = (header[5]<<24 | header[4]<<16 | header[3]<<8 | header[2]); 
	/* offset to BMP data */
	vdOffset = (header[13]<<24 | header[12]<<16 | header[11]<<8 | header[10]); 

	/* read bit map information header */
	if ( (count = fread(&bmih, sizeof(bmih), 1, fp)) != 1 )  {
		return -1;
	}
	if ( !(bmih.biSize == 40 && bmih.biBitCount == 24) ) {
		/* not a BITMAPINFOHEADER or not 8-bits per RGB color */
		return -1;
	}

	/* seek to start of BMP data */
	if (_fseeki64(fp, vdOffset, SEEK_SET) != 0) {
		/* error */
		return -1;
	}

	/* set values in video object */
	width = bmih.biWidth;
	height = bmih.biHeight;
	fsize = width*height;
	nFrames = (fileSize - vdOffset)/(fsize*3);

	rgbQuad.rgbBlue = 0;
	rgbQuad.rgbGreen = 0;
	rgbQuad.rgbRed = 0;
	rgbQuad.rgbReserved = 0;

	return 0;
}

int
Video::createDibHeader( )
{
	bmih.biSize = 40;
	bmih.biWidth = width;
	bmih.biHeight = height;
	bmih.biPlanes = 1;
	bmih.biBitCount = 24;
	bmih.biCompression = 0;
	bmih.biSizeImage = 0;
	bmih.biXPelsPerMeter = 0;
	bmih.biYPelsPerMeter = 0;
	bmih.biClrUsed = 0;
	bmih.biClrImportant = 0;

	fsize = width*height;
	vdOffset = 0;
	/* YUV file */
	nFrames = (fileSize - vdOffset)/(fsize*3/2);

	rgbQuad.rgbBlue = 0;
	rgbQuad.rgbGreen = 0;
	rgbQuad.rgbRed = 0;
	rgbQuad.rgbReserved = 0;

	return 0;
}

int 
Video::readFrame( void )
{
	size_t n, count;

	switch (fmt) {
		case FMT_BMP:
			n = fsize*3;
			if ( (count = fread(fbuf, 1, n, fp)) != n) {
				/* rewind */
				_fseeki64(fp, vdOffset, SEEK_SET);
				fread(fbuf, 1, n, fp);
			}
			break;
		case FMT_YUV:
			n = fsize*3/2;
			if ( (count = fread(ibuf, 1, n, fp)) != n) {
				/* rewind */
				_fseeki64(fp, vdOffset, SEEK_SET);
				fread(fbuf, 1, n, fp);
			}
			convert_ybr_rgb( ibuf,  fbuf, width, height, bmOrder);
			break;
	}

	return 0;
}

void
Video::getFrame( int frame_num )
{
	int status;
	__int64 frame_offset = 0;
	/* frame_num indexed from 0 */

	if (frameNum == frame_num) {
		/* everything is good to go */
		status = 1;
	}
	else {
		if (frame_num == frameNum+1) {
			/* get next frame */
			frameNum++;
			readFrame();
			status=2;
		}
		else {
			/* seek to correct frame and read frame */
			switch (fmt) {
				case FMT_BMP:
					frame_offset = ((__int64)frame_num)*fsize;
					break;
				case FMT_YUV:
					frame_offset = ((__int64)frame_num)*fsize*3/2;
					break;
			}
			_fseeki64(fp, vdOffset + frame_offset, SEEK_SET);
			frameNum = frame_num;
			readFrame();
			status=3;
		}
	}
	/* 
	 * for test 
	 */
	static FILE *fp = NULL;
	static int done = 0;
	if (0 && !done) {
		if (fp == NULL)
			fp = fopen("frames.txt", "w");
		fprintf(fp, "%d %d\n", frameNum, status);
		if (frameNum > 400) {
			done = 1;
			fclose(fp);
		}
	}
}

void 
Video::doPlay( void )
{
	playing = 1;
}

void 
Video::pausePlay( void )
{
	playing = 0;
}

void 
Video::stopPlay( void )
{
	playing = 0;
}

unsigned long
Video::videoPosition ( double at )
{
	unsigned long frame;
	/* find nearest video frame */
	frame = (unsigned int)(at*fps + 0.5);
	if (frame > nFrames) {
		frame = nFrames;
	}
	return frame;
}

/* local functions */
int
getLine( FILE *fp, char *buf )
{
	char *p;
	do	{
		if( fgets( buf, LINE_LEN, fp ) == 0 )
			return 0;
		if( (p = strchr(buf, '\r')) != 0 || (p = strchr(buf, '\n')) != 0 )
			*p = 0;
	} while( *buf == 0 );
	return 1;
}

static char *
skip_whitespace(char *p)
{
	while (*p != 0 && (*p == ' ' || *p == '\t'))
		p++;
	if (*p == '\n' || *p == '\r') {
		return NULL; /* at end of line */
	}
	else {
		return p;
	}
}
