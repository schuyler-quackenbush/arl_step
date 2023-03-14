#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "license.h"
#include "config.h"
#include "session.h"
#include "trialAudioFiles.h"
#include "wav.h"
#include "defineValues.h"
#include "portControls.h"
#include "utils.h"

/* class constructor */
TrialAudioFiles::TrialAudioFiles()
{
	/* initialize */
    this->audio_files.channels = 0;
    this->audio_files.ru_frames = 0;

    this->audio_files.samplerate = 0;
    this->audio_files.prefSize = FRAMES_PER_BUFFER;
    this->audio_files.next_frame = 0;
    this->audio_files.start_frame = 0;
	this->audio_files.stop_frame = 0;
    this->audio_files.loop = 0;
    this->audio_files.playing = 0;
    this->audio_files.prev_sys = 0;
    this->audio_files.next_sys = 0;

    /* set pointers to NULL */
	for (int i=0; i<MAX_SYS; i++) {
		for (int j=0; j<MAX_CHN; j++) {
			/* set sample pointer to NULL */
			this->audio_files.sys_files[i].samples[j] = NULL;
		}
	}
}

/* class desctructor */
TrialAudioFiles::~TrialAudioFiles()
{
	this->freeAudioFiles();
}

/* free audio sample storage */
void
TrialAudioFiles::freeAudioFiles()
{
	int i, j;
	AUDIO_SAMP *p;
	AudioFiles *paf;

	paf = &this->audio_files;
	for (i=0; i<MAX_SYS; i++) {
		for (j=0; j<MAX_CHN; j++) {
			p = paf->sys_files[i].samples[j];
			my_free(p);
			paf->sys_files[i].samples[j] = NULL;
		}
	}
}

static void
set_file_params( SessionInfo *psi, Trial *pt, AudioFiles *paf )
{
    /* number of channels is same for all sys in FF_INTER,
     * but may differ in FF_MONO, so paf->channels is max channel count for all sys in a trial
     */
    paf->channels = pt->max_channels_in_trial;
    paf->ru_frames = pt->max_frames_in_trial;    
    paf->next_frame = 0;
    paf->samplerate = psi->samplerate;
    paf->next_frame = 0;
    paf->start_frame = 0;
    paf->loop = 0;
    /* round ru_frams up to integral number of output buffers */
    int i = (int)( paf->ru_frames/paf->prefSize);
    int j = paf->ru_frames % paf->prefSize;
    if (j>0) {
        i += 1;
    }
    paf->ru_frames = i * paf->prefSize;
    /* but limit if no valid license */
    if ( !psi->valid_license ) {
        /* MAX_TRIAL_SEC, in samples rounded to integral number of buffers */
        unsigned long n = paf->prefSize * (int)(0.5 + (double)(paf->samplerate*MAX_TRIAL_SEC)/paf->prefSize);
        paf->ru_frames = (paf->ru_frames < n) ? paf->ru_frames : n;
	}
    paf->stop_frame = paf->ru_frames;
}

static int
openInterleavedWavFile(SessionInfo *psi, Config *pc, Trial *pt, System *ps, int sys_idx, AudioFiles *paf)
{
	char *err_str, *filename, msg[MSG_LEN];
	AUDIO_SAMP *idp, *dp;
	WAVHDR wav_hdr, *wp = &wav_hdr;

	/* only one file */
	filename = &ps->componentname[0][0];

	/* open interleaved file */
	if ( (err_str = openWavFile(filename, "r", wp)) != NULL ) {
		return 0; /* error */
	}

    unsigned long num_samp = paf->ru_frames * paf->channels;
    if (wp->frames > num_samp) {
        if (DEBUG_TAF) printf("Truncating %s by %lu samples\n", filename, wp->frames - num_samp);
    }
	/* malloc for interleaved file and read file */
	if ( (idp = (AUDIO_SAMP *)malloc(num_samp * sizeof(AUDIO_SAMP))) == NULL) {
		sprintf(msg, "Cannot malloc buffer for file %s\n", filename);
		showError(msg);
		return 0;
	}
    unsigned long n;
	if ( (n = READ_WAV( wp, idp, (int)num_samp)) != num_samp ) {
		if (n < 0) {
			my_free(idp);
			return 0; /* error */
		}
        else if (n < num_samp) {
			/* padd with zeros */
			if (DEBUG_TAF) printf("Zero padding %s by %lu samples\n", filename, num_samp - n);
            for (unsigned long i=n; i<num_samp; i++) {
				idp[i] = 0; /* samples */
			}
		}
	}
    if (DEBUG_TAF) printf("Read %lu samples for %d channels from %s\n", n, wp->channels, filename);
	closeWavFile(wp);

	/* de-interleave buffer */
	for ( int i=0; i < wp->channels; i++ ) {
        if (DEBUG_TAF) printf("malloc %lu samples of channel %d\n", paf->ru_frames, i);
		if ( (dp = (AUDIO_SAMP *)malloc(paf->ru_frames * sizeof(AUDIO_SAMP))) == NULL) {
			sprintf(msg, "Cannot malloc buffer %d component of file %s\n", i, filename);
			showError(msg);
			return 0;
		}
		paf->sys_files[sys_idx].samples[i] = dp;
		/* copy component file samples */
		if (DEBUG_TAF) printf("copying %lu samples of channel %d\n", paf->ru_frames, i);
		int m = 0;
		int n = psi->cmp_chn[i]; /* channel index into channel interleave */
        for (unsigned long j=0; j<paf->ru_frames; j++) {
			dp[m] = idp[n];
			n += wp->channels;
			m++;
		}
        if (DEBUG_TAF) {
            printf("Audio sample pointer sys %d chan %d %p\n",
                   sys_idx, i, paf->sys_files[sys_idx].samples[i] );
        }
	}
    /* free interleaved buffer */
    my_free(idp);

	/* play mono as stereo 
	 * for interleaved files, paf->nch reflects channel count in file
	 * so play_mono_as_stereo works correctly
	 */
	if (wp->channels == 1 && pc->play_mono_as_stereo == 1) {
		/* duplicate mono signal buffer for dual mono */
		if ( (dp = (AUDIO_SAMP *)malloc(paf->ru_frames * sizeof(AUDIO_SAMP))) == NULL) {
			sprintf(msg, "Cannot malloc additional dual mono component of file %s\n", filename);
			showError(msg);
			return 0;
		}
        paf->sys_files[sys_idx].samples[1] = dp;
		memcpy(paf->sys_files[sys_idx].samples[1], paf->sys_files[sys_idx].samples[0],
               paf->ru_frames * sizeof(AUDIO_SAMP));
	}

	return 1; /* OK */
}

static int
openMonoWavFile(SessionInfo *psi, Config *pc, Trial *pt, System *ps, int sys_idx, AudioFiles *paf)
{
	char *err_str, *filename, msg[MSG_LEN];
    unsigned long n;
	AUDIO_SAMP *dp;
	WAVHDR wav_hdr, *wp = &wav_hdr;

    for ( int i=0; i < pt->max_channels_in_trial; i++ ) {
        /* allocate buffer */
        if ( (dp = (AUDIO_SAMP *)malloc(paf->ru_frames * sizeof(AUDIO_SAMP))) == NULL) {
            sprintf(msg, "Cannot malloc buffer for channel %d\n", i);
            showError(msg);
            return 0;
        }
        paf->sys_files[sys_idx].samples[i] = dp;

        filename = &ps->componentname[i][0];
        if (*filename == 0) {
            /* zero channel */
            memset( dp, 0, paf->ru_frames * sizeof(AUDIO_SAMP) );
			if (DEBUG_TAF) printf("Zero channel\n");
		}
        else {
            /* open component file */
            wp = &ps->wavHdr[i];
            if ( (err_str = openWavFile(filename, "r", wp)) != NULL ) {
                return 0; /* error */
            }

            /* read in component file samples */
            if (wp->nsamp > paf->ru_frames) {
                n = wp->nsamp - paf->ru_frames;
                if (DEBUG_TAF) printf("Truncating %s by %ld samples\n", filename, n);
            }
            if ( (n = READ_WAV( wp, dp, (int)paf->ru_frames)) != paf->ru_frames) {
                if (n < 0) {
                    return 0; /* error */
                }
                else { /* n < paf->ru_frames */
                    /* padd with zeros */
                    if (DEBUG_TAF) printf("Zero padding %s by %lu samples\n", filename, paf->ru_frames - n);
                    for (unsigned long j=n; j<paf->ru_frames; j++) {
                        dp[j] = 0;
                    }
                }
            }
            closeWavFile(wp);
            if (DEBUG_TAF) printf("Read %ld samples from %s\n", n, filename);
        }
	}
	/* play mono as stereo 
	 * for mono files, paf->nch is always 1 since it is the channel count in a mono file!	 * hence we use paf->nch_in_sig
	 */
	if (pt->max_channels_in_trial == 1 && pc->play_mono_as_stereo == 1) {
		/* duplicate mono signal buffer for dual mono */
		if ( (dp = (AUDIO_SAMP *)malloc(paf->ru_frames * sizeof(AUDIO_SAMP))) == NULL) {
			sprintf(msg, "Cannot malloc additional dual mono component of file %s\n", 
				ps->basefilename);
			showError(msg);
			return 0;
		}
		paf->sys_files[sys_idx].samples[1] = dp;
		memcpy(paf->sys_files[sys_idx].samples[1], paf->sys_files[sys_idx].samples[0],
               paf->ru_frames * sizeof(*dp));
	}

	return 1; /* OK */
}

/* open audio files, malloc storage for audio samples and read samples */
int
TrialAudioFiles::openAudioFiles( SessionInfo *psi, Config *pc, int trial_idx )
{
    Trial *pt;
	System *ps;
    AudioFiles *paf;

    pt = &psi->trial[trial_idx];
	paf = &this->audio_files;
    
    set_file_params( psi, pt, paf );

	/* add progress bar */
	portMakeProgress( );
	portShowProgress("Test", "Opening files...", psi->has_open_ref + psi->num_systems);

    int num_systems = psi->has_open_ref + psi->num_systems;
	for ( int sys_idx=0; sys_idx < num_systems; sys_idx++ ) {
		/* Display incremental progress. */
		portIncProgress( );

		ps = &pt->system[sys_idx];
                
        if (psi->file_format == FF_INTER ) {
            /* open interleaved system file */
            if ( openInterleavedWavFile( psi, pc, pt, ps, sys_idx, paf ) == 0) {
                return 0;
            }
        }
        else { /* psi->file_format == FF_MONO */
            /* open system component mono files */
			if ( openMonoWavFile( psi, pc, pt, ps, sys_idx, paf ) == 0 ) {
				return 0;
			}
		}
	}
    /* Clear the progress bar. */
	portCloseProgress();

	/* initial values for loop and zoom controls */
	paf->next_frame = 0;
    paf->start_frame = 0;
    paf->stop_frame = paf->ru_frames;
	paf->loop = 0;
	return 1;
}

/* get pointer to AudoFiles structure */
AudioFiles *
TrialAudioFiles::get_pAudioFiles()
{
	return (&this->audio_files);
}
