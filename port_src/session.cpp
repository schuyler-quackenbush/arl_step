#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "defineValues.h"
#include "wav.h"
#include "rngs.h"
#include "license.h"
#include "config.h"
#include "session.h"
#include "transition.h"
#include "allTestIncludes.h"
#include "portControls.h"
#include "utils.h"
#if WIN32
#include "video.h"
#endif

#define STARTER		'#'

/* external globals */
extern TestBase *this_test;

/* internal function prototypes */
static char *getLine( FILE *fp );
static int getTrials( FILE *fp );
static int parse_chn_map_file(char *file, SessionInfo *psi);
static int parse_chn_label_file(char *p, SessionInfo *psi);
static char *skip_whitespace(char *p);
static void initRandom(void);
static int get_interleaved_file_info(SessionInfo *psi, System *p);
static int get_mono_file_info(SessionInfo *psi, System *p);
static int check_session( SessionInfo *psi, Config *config, const char *check_file );

/* class constructor */
Session::Session( )
{
	/* clear session_info values 
	 * this also clears pointers to malloc'd storate
	 */
	memset(&session_info, 0, sizeof(SessionInfo));
}

/* class desctructor */
Session::~Session()
{
	my_free(this->session_info.trial_map);
	my_free(this->session_info.sys_map);
	my_free(this->session_info.trial);
}

/* member functions */
void 
Session::initSession( 
	int valid_license, 
	char *session_file,
	Config *p1)
{
	this->session_info.valid_license = valid_license;
	this->config = p1;
	my_strncpy(this->config->session_file, session_file, FILE_LEN);
}

int
Session::openSession( void )
{
	char *line, *value, *attribute, *p;
	char msg[MSG_LEN];
	int nsys, nfiles;
	int i, trial_idx, sys;
	SessionInfo *psi = &session_info;
    Trial *pt;     /* pointer to session trials */
	System *ps;    /* pointer to trial systems */
	FILE *fp;
#if WIN32
	extern Video *video;
    video->video_session = 0;     /* default is no video */
#endif
    
	if( (fp = fopen( this->config->session_file, "r" )) == 0 )
		return 0; /* error */

	/* determine number of trials and allocate trial memory */
	psi->num_trials = getTrials( fp );
	psi->trial = (Trial *)malloc(psi->num_trials * sizeof(Trial));
	memset(psi->trial, 0, psi->num_trials*sizeof(Trial));

	/* initialize random number generator */
	initRandom();

	/* initialize channel map */
	for (i=0; i<MAX_CHN; i++) {
		psi->cmp_chn[i] = i; /* default is ascening map */
		psi->cmp_lab[i][0] = '\0';	/* no default */
	}
    /* no map file */
    psi->map_file[0] = '\0';
    psi->map_channels = 0;

	/* initialze to default values */ 
	psi->file_format = FF_INTER; /* interleaved WAV files */
	psi->speaker_configuration = SAME; /* speaker configuration */
//	psi->has_open_ref = 1; /* default for open reference is set in testXX.cpp */
	nfiles = 1; /* interleaved files per signal */
    
	/* no default session type */
	psi->methodology[0] = 0;
	psi->test_id = 0;

	psi->num_systems = 0; 
	/* parse session file header */
	for(;;)	{
		if( (line = getLine( fp )) == NULL ) {
			fclose( fp );
			return 0; /* error */
		}
		/* STARTER ends session header */
		if( *line == STARTER ) {
			break;
		}
		/* lines in form value=attribute */
		attribute = line;
		if( (p = strchr( line, '=' )) != 0 ) {
			value = &p[1];
			*p = 0;

			/* header lines */

			/* Session type  */
			if( my_strnicmp( attribute, "session", 7 ) == 0 ) {
				if( my_strnicmp( value, "MUSHRA", 6 ) == 0 ) {
					this_test = new TestMushra( );
				}
				else if( my_strnicmp( value, "BS.1116", 7 ) == 0 ) {
					this_test = new TestBS1116( );
				}
				else if( my_strnicmp( value, "AB5", 3 ) == 0 ) {
					this_test = new TestAB( 5 );
                }
				else if( my_strnicmp( value, "AB7", 3 ) == 0 ) {
					this_test = new TestAB( 7 );
                }
				else if( my_strnicmp( value, "ABX", 3 ) == 0 ) {
					this_test = new TestABX;
				}
				else if( my_strnicmp( value, "MOS", 7 ) == 0 ) {
					this_test = new TestMOS;
				}
				else if( my_strnicmp( value, "TRAIN", 5 ) == 0 ) {
					this_test = new TestTrain( T_TRAIN );
				}
				else {
					sprintf(msg, "Unknown Test Methodology: %s", value);
					showError( msg );
					return 0;
				}
				my_strncpy(psi->methodology, value, LAB_LEN); /* methodology as string */
				/* init config for this test */
                this_test->initThisTest( &this->session_info, this->config );
			}

			/* associate with video presentation 
			 * value is video session filename
			 */
#if WIN32
			if( my_strnicmp( attribute, "video_session", 13 ) == 0 ) {
				if (video->openVideoSession( value, psi->num_trials ) < 0) {
					showError((char*)"Error reading video session");
					return 0;
				}
			}
#endif
			/* over-ride default configuration values */
			/* has_open_reference */
			if( my_strnicmp( attribute, "has_open_reference", 14 ) == 0 ) {
				if( my_strnicmp( value, "true", 4 ) == 0 ) {
					//config->show_ref = 1;
					psi->has_open_ref = 1;
				}
				if( my_strnicmp( value, "false", 5 ) == 0 ) {
					//config->show_ref = 0;
					psi->has_open_ref = 0;
				}
			}
            /* transition mode */
            if( my_strnicmp( attribute, "trans_mode", 10 ) == 0 ) {
                if( my_strnicmp( value, "CF", 2 ) == 0 )
                    psi->trans_mode = ID_TRANS_CF;
                else if( my_strnicmp( value, "5MS", 3 ) == 0 )
                    psi->trans_mode = ID_TRANS_05MS;
                else if( my_strnicmp( value, "10MS", 4 ) == 0 )
                    psi->trans_mode = ID_TRANS_10MS;
                else if( my_strnicmp( value, "20MS", 4 ) == 0 )
                    psi->trans_mode = ID_TRANS_20MS;
                else {
                    showError((char*)"Unknown transition mode attribute");
                    return 0;
                }
            }
//			/* lock admin window options panel */
//			if( my_strnicmp( attribute, "lock_admin_options_panel", 14 ) == 0 ) {
//				if( my_strnicmp( value, "true", 4 ) == 0 ) {
//					config->lock_options = 1;
//				}
//				if( my_strnicmp( value, "false", 5 ) == 0 ) {
//					config->lock_options = 0;
//				}
//			}
			/* configuration options */
			/* randomization */
			if( my_strnicmp( attribute, "randomize_trials", 13 ) == 0 ) {
				if( my_strnicmp( value, "true", 4 ) == 0 ) {
					config->random_trials = 1;
				}
				if( my_strnicmp( value, "false", 5 ) == 0 ) {
					config->random_trials = 0;
				}
			}
			if( my_strnicmp( attribute, "randomize_conditions", 13 ) == 0 ) {
				if( my_strnicmp( value, "true", 4 ) == 0 ) {
					config->random_conditions = 1;
				}
				if( my_strnicmp( value, "false", 5 ) == 0 ) {
					config->random_conditions = 0;
				}
			}
			/* control scoring slider action */
			if( my_strnicmp( attribute, "only_score_item_playing", 20 ) == 0 ) {
				if( my_strnicmp( value, "true", 4 ) == 0 ) {
					config->only_score_item_playing = 1;
				}
				if( my_strnicmp( value, "false", 5 ) == 0 ) {
					config->only_score_item_playing = 0;
				}
			}
			/* play mono as stereo */
			if( my_strnicmp( attribute, "play_mono_as_stereo", 19 ) == 0 ) {
				if( my_strnicmp( value, "true", 4 ) == 0 ) {
					config->play_mono_as_stereo = 1;
				}
				if( my_strnicmp( value, "false", 5 ) == 0 ) {
					config->play_mono_as_stereo = 0;
				}
			}
			/* check_score */
			if (my_strnicmp(attribute, "check_score", 11) == 0) {
				if (my_strnicmp(value, "true", 4) == 0) {
					config->check_score = 1;
				}
				if (my_strnicmp(value, "false", 5) == 0) {
					config->check_score = 0;
				}
			}
            /* approve_score */
            if (my_strnicmp(attribute, "approve_score", 13) == 0) {
                if (my_strnicmp(value, "true", 4) == 0) {
                    config->approve_score = 1;
                }
                if (my_strnicmp(value, "false", 5) == 0) {
                    config->approve_score = 0;
                }
            }
            /* show_system -- this is ONLY for debugging! */
            if (my_strnicmp(attribute, "show_system", 11) == 0) {
                if (my_strnicmp(value, "true", 4) == 0) {
                    config->show_system = 1;
                }
                if (my_strnicmp(value, "false", 5) == 0) {
                    config->show_system = 0;
                }
            }

			/* file format identifier */
			if( my_strnicmp( attribute, "file_format", 11 ) == 0 ) {
				if( my_strnicmp( value, "mono", 4 ) == 0 ) {
					psi->file_format = FF_MONO;
                    showMessage((char*)"File format is multiple mono files\n");
				}
				else if( my_strnicmp( value, "interleaved", 11 ) == 0 ) {
					psi->file_format = FF_INTER;
					nfiles = 1; /* one file per stimuli */
                    showMessage((char*)"File format is single interleaved file\n");
				}
				else {
					showError((char*)"Unknown file_format attribute");
					return 0;
				}
			}

			/* channel map file for interleaved files */
			if( my_strnicmp( attribute, "interleaved_chn_map_file", 24 ) == 0 ) {
				if (psi->file_format == FF_INTER) {
					my_strncpy(psi->map_file, value, FILE_LEN); /* save */
					if ( (psi->map_channels = parse_chn_map_file(value, psi)) == 0) {
						return 0; /* error */
					}
				}
				else {
					showError((char*)"interleaved_chn_map_file is only for interleaved files\n");
					return 0; /* error */
				}
			}
			/* channel labels for mono files */
			if( my_strnicmp( attribute, "mono_chn_label_file", 19 ) == 0 ) {
				if (psi->file_format == FF_MONO) {
					my_strncpy(psi->map_file, value, FILE_LEN); /* save */
					if ( (psi->map_channels = parse_chn_label_file(value, psi)) == 0) {
						return 0; /* error */
					}
				}
				else {
					showError((char*)"mono_chn_label_file is only for mono files\n");
					return 0; /* error */
				}
			}

			/* speaker_configuration */
			if( my_strnicmp( attribute, "speaker_configuration", 21 ) == 0 ) {
				if( my_strnicmp( value, "same", 4) == 0 ) {
					psi->speaker_configuration = SAME;
				}
				else if( my_strnicmp( value, "different", 9) == 0 ) {
					psi->speaker_configuration = DIFFERENT;
					if (psi->file_format == FF_INTER) {
						showError((char*)"Different speaker configurations is only allowed\nfor mono files\n");
						return 0; /* error */
					}
				}
				else {
					showError((char*)"Unknown speaker_configuration attribute");
					return 0;
				}
			}

		}
		my_free( line );
	}/* end session header parsing */

	/* checks */
	if (psi->file_format == FF_MONO &&psi->map_file[0] == '\0') {
		showError((char*)"Mono File Format must have mono_chn_label_file");
		return 0;
	}

    /* print session configuration */
    printSessionConfig();
    
//	/* set config to admin panel */
//	set_admin( );
//	/* optionally lock options panel
//	 * if lock_options = 1 then arg (enable) is 0
//	 */
//	lock_admin_options_panel( config->lock_options == 1 ? 0 : 1 );

	/* begin parsing trials */

	/* add progress bar */
	portMakeProgress( );
	portShowProgress("Admin", "Parsing session file...", psi->num_trials);

	/* next line is first starter line */
    psi->max_channels_in_session = 0;
	for (trial_idx=0; trial_idx<psi->num_trials; trial_idx++) {
		/* next starter line for next signal (and trial) */
		/* copy signal label, but skip STARTER character */
		my_strncpy(psi->trial[trial_idx].sig_lab, skip_whitespace(&line[1]), LAB_LEN);

		sys=0;
        pt = &psi->trial[trial_idx];
        pt->max_frames_in_trial = 0;
        
		/* parse signal lines in a trial 
		 * each trial block begins with a line starting with "#" */
		while( (line = getLine( fp )) != 0 && *line != STARTER ) {
			char *p;
			/* next system */
            if( ( (sys + 1 - psi->has_open_ref > MAX_SYS ) ||
                (this_test->test_id == T_MUSHRA && (sys + 1 - psi->has_open_ref) > MM_SYS)) ) {
                //sys is index, so add 1 to get count
                //sys includes REF if present, so subtract
                //MUSHRA limited to 12 systems under test
				my_free( line );
				fclose(fp);
				portCloseProgress();
				showError((char*)"Too many systems under test\n");
				return 0; /* error */
			}

			/* read base filename */
			p = line;
			for (i=0; ( (*p != 0 && *p != ' ' && *p != '\t') || i==FILE_LEN-1 ); i++) {
				psi->trial[trial_idx].system[sys].basefilename[i] = *p++;
			}
			psi->trial[trial_idx].system[sys].basefilename[i] = '\0'; /* NULL terminate */
			if (*p != '\0') {
				p++;
				/* read system label */
				p = skip_whitespace(p);
				my_strncpy(psi->trial[trial_idx].system[sys].sys_lab, p, LAB_LEN);
			}
			else {
				/* copy in NULL string */
				my_strncpy(psi->trial[trial_idx].system[sys].sys_lab, "", LAB_LEN);
			}

			/* read WAV header(s) for system signal */
			ps = &pt->system[sys];
            ps->max_channels_in_signal = 0;
            ps->max_frames_in_signal = 0;
			if (psi->file_format == FF_INTER) {
                /* read WAV header for this interleaved system signal */
				get_interleaved_file_info(psi, ps);
			} else { /* if (file_format == FF_MONO) { */
                /* read WAV header for all components of the system signal */
				get_mono_file_info(psi, ps);
			}
            /* if needed, update max channels and frames in trial */
            if (ps->max_channels_in_signal > pt->max_channels_in_trial)
                pt->max_channels_in_trial = ps->max_channels_in_signal;
            if (ps->max_frames_in_signal > pt->max_frames_in_trial)
                pt->max_frames_in_trial = ps->max_frames_in_signal;
            
            /* if needed, update max channels in session */
            if (pt->max_channels_in_trial > psi->max_channels_in_session)
                psi->max_channels_in_session = pt->max_channels_in_trial;

            sys++;
		} /* end parsing this trial */
		/* Display incremental progress. */
		portIncProgress( );
        
		/* don't count open reference as a system under test */
		nsys = sys - psi->has_open_ref; 
		if (psi->num_systems == 0) {
			/* save nsys only once */
			psi->num_systems = nsys;
		}
		else if (psi->num_systems != nsys) {
			showError((char*)"Must have same number of systems under test for each trial.");
			return 0;
		}

	} /* end parsing all trials */
	fclose( fp );
	/* Clear the progress bar. */
	portCloseProgress();

	/* check for consistancy of WAV headers */
	check_session( psi, config, CHECK_FILE );
	/* check that session file meets test methodology requirements */
    this_test->checkThisTest( this );

	psi->trial_map = mkmap( psi->num_trials );
	/* number of systems in randomization map 
	 * does't include Open Reference (REF)
	 * but add one more for sys X in ABX */
	psi->sys_map = mkmap( psi->num_systems + ( (psi->test_id == T_ABX) ? 1 : 0 ) );
    
#if (DEBUG_SES)
        printf("Session   Max channels %2d\n", psi->max_channels_in_session);
        printf("          Samplingrate %d\n", psi->samplerate);
    for (int i=0; i<psi->num_trials; i++) {
        pt = &psi->trial[i];
        printf("Trial  %2d Max channels %2d Max frames %2d\n",
               i, pt->max_channels_in_trial, pt->max_frames_in_trial);
        for (int j=0; j<psi->num_systems; j++) {
            ps = &pt->system[j];
        printf("   Sys %2d Max channels %2d Max frames %2d\n",
               j, ps->max_channels_in_signal, ps->max_frames_in_signal);
        }
    }
#endif
    
	return 1; /* OK */
}


/* helper function for printing admin panel options */
char
opts( int status )
{
    return (status == 1) ? 'x' : ' ';
}

int
Session::printSessionConfig( void )
{
    FILE *fp;
    char filename[FILE_LEN];
    unsigned long len = strlen(this->config->session_file);
    if (len <= FILE_LEN-11) {
        strcpy(filename, this->config->session_file);
        filename[len-4] = '\0';
        strcat(filename, "_config.txt");
    }
    else {
        /* filelength error */
        return 0;
    }
    /* open file */
    if( (fp = fopen( filename, "w" )) == 0 )
        return 0; /* error */

    /* print status of administrator panel */
    fprintf(fp, "Session Status\n\n");
    fprintf(fp, "Session File:  %s\n", config->session_file);
    fprintf(fp, "Laboratory ID: %s\n", config->lab_id);
    fprintf(fp, "Listener ID:   %s\n", config->listener_id);
    fprintf(fp, "Audio Device:  %s\n", config->dev_name);
    fprintf(fp, "Transition:    ");
    switch (session_info.trans_mode) {
        case ID_TRANS_CF: fprintf(fp, "%s\n", TRANS_CF); break;
        case ID_TRANS_05MS: fprintf(fp, "%s\n", TRANS_05MS); break;
        case ID_TRANS_10MS: fprintf(fp, "%s\n", TRANS_10MS); break;
        case ID_TRANS_20MS: fprintf(fp, "%s\n", TRANS_20MS); break;
    }
    fprintf(fp, "Options:\n");
    fprintf(fp, "%c Randomize Trials         %c Show Condition Name\n", opts(config->random_trials), opts(config->show_system) );
    fprintf(fp, "%c Randomize Conditions     %c Allow Previous\n", opts(config->random_conditions), opts(config->show_prev) );
    fprintf(fp, "%c Only Score Item Playing  %c Play Mono as Stereo\n", opts(config->only_score_item_playing), opts(config->play_mono_as_stereo) );
    fprintf(fp, "%c Approve Scores\n", opts(config->approve_score) );
    fprintf(fp, "\n");
    
    fclose(fp);
    
    return 1;
}

void 
Session::endSession( void )
{
	my_free(session_info.trial_map);
	my_free(session_info.sys_map);
	my_free(session_info.trial);
}

int *
Session::mkmap( int n )
{
	int i, *map;
	if( (map = (int *)malloc( n*sizeof(int) )) != 0 ) {
		for( i=0; i<n; i++ )
			map[i] = i;
	}
	return map;
}

void
Session::reset_map( int *map, int n )
{
	int i;
	for( i=0; i<n; i++ ) {
		map[i] = i;
	}
}

void
Session::permute( int *map, int n )
{
	int i, k, t;
	for( i=0; i<n; i++ ) {
		k = (int)(Random()*n);
		t = map[k];
		map[k] = map[i];
		map[i] = t;
	}
}

int
Session::coin_toss(void)
{
	return( Random() > 0.5 ? 1 : 0);
}

/* end of member functions */

/* beginning of internal static functions */

/* channel map file has as
 * first column: interleaved file channel number, counting from 1
 * second column: output channel number, counting from 1
 *
 * returns: number of lines in file, which is number of channels is signal
 */
static int
parse_chn_map_file(char *file, SessionInfo *psi)
{
	FILE *fp;
	char msg[MSG_LEN];
	char *p;
	int i, j, k, nchn;
	if ( (fp = fopen(file, "r")) == NULL ) {
		sprintf(msg, "Cannot open channel map file %s", file);
		showError(msg);
		return 0;
	}
	nchn = 0;
	for (i=1; i<=MAX_CHN; i++) {
		if ( (p = getLine(fp)) == NULL )
			break;
		sscanf(p, "%d %d", &j, &k);
		if (i != j) {
			showError((char*)"Channel map files must have consecutive interleaved\nchannel numbers counting from 1");
			return 0;
		}
		psi->cmp_chn[i-1] = k-1;
		nchn++;
	}
	return nchn;
}

/* channel label file has as
 * first column: Output Buffer channel number, counting from 1
 * second column: filename suffix string to associate with ASIO channel
 *
 * returns: number of lines in file
 */
static int
parse_chn_label_file(char *file, SessionInfo *psi)
{
	FILE *fp;
	char msg[MSG_LEN];
	char *p;
	int i, j, nlab;
	if ( (fp = fopen(file, "r")) == NULL ) {
		sprintf(msg, "Cannot open channel label file %s", file);
		showError(msg);
		return 0;
	}
	nlab = 0;
	for (i=1; i<=MAX_CHN; i++) {
		if ( (p = getLine(fp)) == NULL )
			break;
		sscanf(p, "%d %s", &j, psi->cmp_lab[i-1]);
		if (i != j) {
			showError((char*)"Channel label file must have consecutive\nASIO channel numbers counting from 1");
			return 0;
		}
		nlab++;
	}
	return nlab;
}

static int
get_interleaved_file_info(SessionInfo *psi, System *ps)
{
	/* only one file per signal */
	WAVHDR *wp = &ps->wavHdr[0]; 
	char *status = &ps->wavStatus[0][0];

	/* copy basename to first component name */
	my_strncpy(ps->componentname[0], ps->basefilename, FILE_LEN);

	/* open file */
	ps->wavStatus[0] = openWavFile( ps->componentname[0], "r", wp );
    closeWavFile(wp);
    
    ps->max_channels_in_signal = wp->channels;
    ps->max_frames_in_signal = (int)wp->frames;

    psi->samplerate = wp->samplerate; //all samplerate should match
    
	return (status == NULL) ? 1 : 0;
}

static int
get_mono_file_info(SessionInfo *psi, System *ps)
{
    /* get info on the set of mono files that comprise the signal for a sys in this trail */
    int active_channels = 0, max_channels = 0;
    unsigned long max_frames = 0;

	for (int i=0; i<MAX_CHN; i++) {
		if (psi->cmp_lab[i][0] == '\0')
			continue; //NULL string
		/* construct component file pathname */
		my_strncpy(ps->componentname[i], ps->basefilename, FILE_LEN);
		my_strncat(ps->componentname[i], "_", FILE_LEN);
		my_strncat(ps->componentname[i], psi->cmp_lab[i], FILE_LEN);
		my_strncat(ps->componentname[i], WAV_EXT, FILE_LEN);
		/* open file */
		ps->wavStatus[i] = openWavFile( ps->componentname[i], "r", &ps->wavHdr[i]);
		closeWavFile(&ps->wavHdr[i]);
		/* count if open was successful */
		if (ps->wavStatus[i] == NULL) {
			active_channels++;  //found WAV file, so increment active channel count
            max_channels = i+1;   //maximum channel count
            if (ps->wavHdr[i].frames > max_frames)
                max_frames = (int)ps->wavHdr[i].frames;
		}
	}
    ps->active_channels = active_channels;
    ps->max_channels_in_signal = max_channels;
    ps->max_frames_in_signal = max_frames;
    
    psi->samplerate = ps->wavHdr[0].samplerate; //all samplerate should match

	return max_channels;
}
#define DIFF_ERR	48	/* samples, but could be 1 ms WRT Fs */

static int
check_wav_header(WAVHDR *ref_wp, WAVHDR *wp, int *max_diff, char *msg_str)
{
	int err, diff;

	if (ref_wp->channels != wp->channels) my_strncat(msg_str, "NC ", MSG_LEN);
	if (ref_wp->samplerate != wp->samplerate) my_strncat(msg_str, "SR ", MSG_LEN);
	if (ref_wp->ssize != wp->ssize) my_strncat(msg_str, "WS ", MSG_LEN);
	if (ref_wp->channelMask != wp->channelMask) my_strncat(msg_str, "CM ", MSG_LEN);
	diff = (int)(ref_wp->frames - wp->frames);
	if (diff < 0) diff = -diff; 
	if (diff > *max_diff) *max_diff = diff;
	if (diff > DIFF_ERR) my_strncat(msg_str, "NF ", MSG_LEN);
	if (strlen(msg_str) == 0) {
		my_strncat(msg_str, "OK", MSG_LEN);
		err = 0;
	}
	else {
		err = 1;
	}
	return err;
}

static int
check_interleaved_file(SessionInfo *psi, System *psf, int ref_sig, int *max_diff, char *msg_str)
{
    /* There is only one file per sys in interleaved format,
     * so all channels in that file must have same number of samples
     * This checks that WAV files for all other systems in a trial match the parameters
     * of the REF file (within a specified +/- difference in number of frames
     */
	int k, err;
	WAVHDR *ref_wp, *wp;
	static System *ref_psf ;

	k = 0;
	err = 0;
	if (ref_sig == 1) {
		/* save reference psf */
		ref_psf = psf;
		/* reference signal file */
		if (psf->wavStatus[k] != NULL) {
			/* file open error */
			my_strncat(msg_str, psf->wavStatus[k], MSG_LEN);
			err++;
		}
		else {
			sprintf(msg_str, "%d-channel OK", psf->wavHdr->channels);
		}
	}
	else {
		/* other signal files */
		if (psf->wavStatus[k] != NULL) {
			/* file open error */
			my_strncat(msg_str, psf->wavStatus[k], MSG_LEN);
			err++;
		}
		else {
			/* check against reference */
			ref_wp = &ref_psf->wavHdr[k];
			wp = &psf->wavHdr[k];
			err += check_wav_header(ref_wp, wp, max_diff, msg_str);
		}
	}

	return err;
}

static int
check_components(SessionInfo *psi, System *psf, char *msg_str)
{
	int k, n, sig_ok;

	n = 0;
	msg_str[0] = 0;
	/* for each component in signal */
	for (k=0; k<MAX_CHN; k++) {
		/* if component has valid file label and file was opened succussfully */
		if (psi->cmp_lab[k][0] != '\0' && psf->wavStatus[k] == NULL) {
			/* Print mono_chn_label_file line number */
			my_strncat(msg_str, "x", MSG_LEN);
			n++; /* increment channel count */
		}
		else {
			/* no component file or file open error
			 * so make component filename an empty string */
			psf->componentname[k][0] = 0;
			/* and put spaces in channel string */
			my_strncat(msg_str, " ", MSG_LEN);
		}
	}
	if (n > 0) {
		my_strncat(msg_str, " OK", MSG_LEN);
		sig_ok = 1;
	}
	else {
		sprintf(msg_str, "No component signals");
		sig_ok = 0;
	}
	return sig_ok;
}

static int
check_mono_files(SessionInfo *psi, System *psf, int ref_sig, int *max_diff, char *msg_str)
{
    /* There is are multiple files per sys in mono-file format.
     * For a given sys, all WAV files must agree in all parameters, including number of frames
     * This checks that files for all other systems match the parameters
     * of the REF sys files (within a specified +/- difference in number of frames
     */
	int k, err, sig_ok;
	WAVHDR *ref_wp, *wp;
	static System *ref_psf ;
	static char ref_msg_str[MSG_LEN];

	/* check component files for this sys */
	sig_ok = check_components(psi, psf, msg_str);
	err = sig_ok == 1 ? 0 : 1;

	if (ref_sig == 1) {
		/* save reference psf and reference msg string */
		ref_psf = psf;
		my_strncpy(ref_msg_str, msg_str, MSG_LEN);
	}
	else {
		if (sig_ok == 1) {
			int comp_err = 0;
			char comp_msg_str[MSG_LEN];
			comp_msg_str[0] = '\0';
			for (k=0; k<MAX_CHN; k++) {
				/* if component has valid file label and file was opened succussfully */
				if (psi->cmp_lab[k][0] != '\0' && psf->wavStatus[k] == NULL) {
					/* check against reference */
					ref_wp = &ref_psf->wavHdr[k];
					wp = &psf->wavHdr[k];
                    /* component file parameters must match those of REF */
					comp_err += check_wav_header(ref_wp, wp, max_diff, comp_msg_str);
				}
			}
			if (strcmp(comp_msg_str, "OK") != 0) {
				my_strncat(msg_str, comp_msg_str, MSG_LEN);
			}
		}
		if (sig_ok == 1 && psi->speaker_configuration == SAME) {
			/* check against referenece */
			if (my_strnicmp(ref_msg_str, msg_str, MAX_CHN) != 0) {
				/* match fails */
				my_strncat(msg_str, "USC: ", MSG_LEN);
			}
		}
	}

	return err;
}

/* WAV headers for all files have been read
 * now check that session is consistent 
 */
int 
check_session( SessionInfo *psi, Config *pc, const char *check_file )
{
	char msg_str[MSG_LEN], ref_msg_str[MSG_LEN];
	int i, j, ref_sig, err, max_diff;
	Trial *pt;
	System *psf;
	WAVHDR *ref_wp;
	FILE *fp;

	if( (fp = fopen( check_file, "w" )) == 0 ) {
		sprintf( msg_str, "Cant open check file %s", check_file);
		showError( msg_str );
	}

	fprintf( fp, "Session Check\n");
	fprintf( fp, "\n" );
	fprintf( fp, "Session configuration\n" );
	fprintf( fp, "Session: %s\n", psi->methodology );
	fprintf( fp, "File format: %s\n", psi->file_format == FF_INTER ? "interleaved" : "mono" );
	fprintf( fp, "Speaker configuration: %s\n", psi->speaker_configuration == SAME ? "same" : "different" );
	fprintf( fp, "Open reference: %s\n", psi->has_open_ref == 1 ? "yes" : "no" );
	if (psi->file_format == FF_INTER) {
		fprintf( fp, "Channel Map File (interleaved files): %s\n", 
			psi->map_file[0] == '\0' ? "Default (none specified)" : psi->map_file );
	}
	if (psi->file_format == FF_MONO) {
		fprintf( fp, "Channel Label File (mono files): %s\n", psi->map_file );
	}
	/* Admin Panel options that can be specified in sesssion file */
	fprintf( fp, "Randomize Trials: %s\n", pc->random_trials == 1 ? "yes" : "no" );
	fprintf( fp, "Randomize Conditions: %s\n", pc->random_conditions == 1 ? "yes" : "no" );
	fprintf( fp, "Only score item playing: %s\n", pc->only_score_item_playing == 1 ? "yes" : "no" );
	fprintf( fp, "Play mono as stereo: %s\n", pc->play_mono_as_stereo == 1 ? "yes" : "no" );

	fprintf( fp, "%d Trials\n", psi->num_trials );
	fprintf( fp, "%d Conditions per Trial\n", psi->num_systems );
	fprintf( fp, "\n" );

	fprintf( fp, "Key to diagnostics\n");
	fprintf( fp, "N-channel - Number of channels (0 indicates a problem)\n" );
	fprintf( fp, "OK  - File found as expected\n" );
	fprintf( fp, "USC - Unexpected signal components\n" );
	fprintf( fp, "NC  - Inconsistent number of channels\n" );
	fprintf( fp, "SR  - Inconsistent sampling rate\n" );
	fprintf( fp, "WS  - Inconsistent audio word size\n" );
	fprintf( fp, "NF  - Inconsistent number of frames\n" );
	fprintf( fp, "CM  - Inconsistent Channel Mask\n" );
	fprintf( fp, "\n" );

	if (psi->file_format == FF_MONO) {
#if (MAX_CHN == 32)
		fprintf( fp, "%48s   %s\n", "Channels      Diagnostic", "File" );
		fprintf( fp, "%-48s\n",      "         11111111112222222222333" );
		fprintf( fp, "%-48s\n",      "12345678901234567890123456789012" );
#else
		fprintf( fp, "%80s   %s\n", "Channels      Diagnostic", "File" );
		fprintf( fp, "%-80s\n",      "         1111111111222222222233333333334444444444555555555566666" );
		fprintf( fp, "%-80s\n",      "1234567890123456789012345678901234567890123456789012345678901234" );
#endif
	}
	else {
		fprintf( fp, "%24s   %s\n", "Channels/Diagnostic", "File" );
	}

	/* check each WAV header */
	err = 0;
	max_diff = 0;
	/* for each trial (or sig) */
	for (i=0; i<psi->num_trials; i++) {
		pt = &psi->trial[i];

		/* for each sys (or condition) in a trial */
		for( j=0; j<(psi->has_open_ref + psi->num_systems); j++ ) {
			psf = &pt->system[j];
			if (j == 0) {
				/* reference is first signal */
				ref_sig = 1;
				ref_wp = psf->wavHdr;
			}
			else {
				/* check against reference signal */
				ref_sig = 0;
				my_strncpy(ref_msg_str, msg_str, MSG_LEN);
			}

			msg_str[0] = 0;
			if (psi->file_format == FF_INTER) {
				/* interleaved file format */
				err += check_interleaved_file(psi, psf, ref_sig, &max_diff, msg_str);
			}
			else { /*  if (psi->file_format == FF_MONO) { */
				/* mono file format */
				err += check_mono_files(psi, psf, ref_sig, &max_diff, msg_str);
			}

			if (psi->file_format == FF_MONO) {
#if (MAX_CHN == 32)
				fprintf(fp, "%-48s : %s\n", msg_str, psf->basefilename);
#else
				fprintf(fp, "%-80s : %s\n", msg_str, psf->basefilename);
#endif
			}
			else {
				fprintf(fp, "%24s : %s\n", msg_str, psf->basefilename);
			}
		}
		fprintf( fp, "\n" );
	}
	fclose( fp );

	if( err > 0 ) {
		sprintf( msg_str, 
			"Check Session:\n"
			"One or more files for the current session file either\n"
			"could not be opened or had an inconsistant format.\n"
			"See the file '%s' for full details.", check_file );
		showError( msg_str );
	}
	else {
		sprintf( msg_str, 
            "STEP Check Session Summary\n\n"
			"No errors.\n"
			"Maximum file length difference = %d samples\n\n"
			"See the file '%s' for full details.",
			max_diff, check_file );
        showMessage( msg_str );
	}
	return err; /* 0 is OK */
}

/* "Lehmer random number generator"
 * returns a pseudo-random number uniformly distributed between
 * 0.0 and 1.0.  The period is (m - 1) where m = 2,147,483,647 and the
 * smallest and largest possible values are (1 / m) and 1 - (1 / m)
 * respectively.
 */
static void
initRandom(void) {
	int i;
	PlantSeeds(-1);
	for (i=0; i<250; i++) Random(); /* step into random section */
}

static char *
getLine( FILE *fp )
{
	char buf[LINE_LEN], *p;
	do	{
		if( fgets( buf, sizeof(buf), fp ) == 0 )
			return 0;
		if( (p = strchr(buf, '\r')) != 0 || (p = strchr(buf, '\n')) != 0 )
			*p = 0;
	} while( *buf == 0 );
	return my_strdup( buf );
}

static int
getTrials( FILE *fp )
{
	char *p;
	int ntrials = 0;
	while (1) {
		if ( (p = getLine(fp)) == NULL ) {
			rewind( fp );
			return ntrials;
		}
		if (*p == '#') {
			ntrials++;
		}
	}
	return 0;
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
