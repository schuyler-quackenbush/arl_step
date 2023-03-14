#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "defineValues.h"
#include <stdio.h>
#include "portControls.h"
#include "license.h"
#include "session.h"
#include "config.h"
#include "defineValues.h"
#include "testBase.h"
#include "utils.h"

/* class constructor */
TestBase::TestBase( )
{
	;
}

/* class desctructor */
TestBase::~TestBase()
{
	my_free(this->trial_scores);
}

/* non-virtual member functions */

/* initialize */
int 
TestBase::initTest( int valid_license, 
   Session *p1, Config *p2)
{
	int i, j;
	SessionInfo *psi = &p1->session_info;

	this->valid_license = valid_license;
	this->session = p1;
	this->config = p2;

	/* construct score file */
	my_strncpy(this->score_file, config->session_dir, FILE_LEN);
	my_strncat(this->score_file, PATH_SEP, FILE_LEN);
	my_strncat(this->score_file, "Scores_", FILE_LEN);
//NOT USING SESSION_ID
//	my_strncat(this->score_file, config->session_id, FILE_LEN);
//	my_strncat(this->score_file, "_", FILE_LEN);
	my_strncat(this->score_file, config->lab_id, FILE_LEN);
	my_strncat(this->score_file, "_", FILE_LEN);
	my_strncat(this->score_file, config->listener_id, FILE_LEN);
	my_strncat(this->score_file, ".txt", FILE_LEN);

	/* allocate score file memory */
	if ( (this->trial_scores = (TrialScores *)malloc(psi->num_trials * sizeof(TrialScores))) == NULL) {
		showError( (char*)"Malloc of score memory failed\n");
		return 0;
	}
	/* initalize scores to zero */
	for( i=0; i<psi->num_trials; i++ ) {
		for( j=0; j<psi->num_systems; j++ ) {
			this->trial_scores[i].score[j] = 0;
		}
	}
	this->test_methodology[0] = 0; /* initialize string */
	this->complete_test = 0;
	return 1;
}

/* Check for an existing score file */
int TestBase::checkScoreFile()
{
	char msg[MSG_LEN];
	FILE *fp;

	if( write_scores && ((fp = fopen( score_file, "r" )) != 0) ) {
		fclose(fp);
		/* file exits */
		if( !this->readPartialScores( ) ) {
            sprintf( msg, "The score file %s already exists\n\nOverwrite?",
                score_file);
			if ( showQuestion( msg ) == 1 ) {
				return 1; // score file exists and user wishes to overwrite
			}
			else {
				showError( (char*)"Enter a different Session ID or Listener ID." );
				return 0; // score file exists and don't overwrite, so abandon
			}
		}
	}
	return 1; // score file does not exist 
}

/* Write scores */
int TestBase::writeScores()
{
	FILE *fp;

	if( (fp = fopen( this->score_file, "w" )) == 0 ) {
		char msg[MSG_LEN];
		sprintf(msg, "Can't open score file %s for writing", this->score_file );
		showError( msg );
		return 0;
	}

	/* virtual function */
	writeThisTestScores( fp );

	fclose(fp);
	return 1;
}


/* Write scores to temporary save file */
int TestBase::writePartialScores()
{ 
	int i, j;
	char msg[256];
	SessionInfo *psi = &this->session->session_info;
	FILE *fp;

	/* there are no scores for training or playlist, so don't save anything */
	if (this->test_id == T_TRAIN || this->test_id == T_PLAYLIST) {
		return 1;
	}

	if( (fp = fopen( this->score_file, "w" )) == NULL ) {
		sprintf( msg, "Can't open %s for writing", this->score_file );
		showError( msg );
		return 0;
	}
	fprintf( fp, "* %d %d %d\n",
		psi->num_trials, psi->num_systems, psi->tnum );
	for( i=0; i<psi->num_trials; i++ ) {
		fprintf( fp, "%d ", psi->trial_map[i] );
	}
	fprintf( fp, "\n" );
	for( i=0; i<psi->num_trials; i++ ) {
		for( j=0; j<psi->num_systems; j++ ) {
			fprintf( fp, "%.1f ", 
				this->valid_license ? this->trial_scores[i].score[j] : 0 );
		}
		fprintf( fp, "\n" );
	}
	fclose(fp);

	return 1;
}

/* Read scores from temporary save file */
int TestBase::readPartialScores( )
{ 
	int i, j, ntrial, nsys, tnum;
	char msg[MSG_LEN], c;
	SessionInfo *psi = &this->session->session_info;
	FILE *fp;

	if( (fp = fopen( this->score_file, "r" )) == NULL ) {
		sprintf( msg, "Can't open %s for reading", this->score_file );
		showError( msg );
		return 0; //no score file
	}
	if( fscanf( fp, "%c %d %d %d",
		&c, &ntrial, &nsys, &tnum ) != 4 ||
			c != '*' || ntrial<=0 || tnum<0
			)
		return 0; //invalid score file contents
	if( ntrial != psi->num_trials ) {
		sprintf( msg, "Session for %s was incomplete but "
			"saved file does not match current session", config->listener_id );
		showError( msg );
		return 0; // wrong score file
	}
    sprintf( msg, "Previous session for %s was incomplete\n\n"
            "Keep previous scores and continue with trial %d of %d?",
			config->listener_id, tnum+1, psi->num_trials );
	/* ask user what to do */
	if( showQuestion( msg ) != 1 ) {
		/* do not continue */
		return 0;
	}

	psi->tnum = tnum;
	for( i=0; i<psi->num_trials; i++ ) {
		fscanf( fp, "%d", &psi->trial_map[i] );
	}
	for( i=0; i<psi->num_trials; i++ ) {
		for( j=0; j<psi->num_systems; j++ ) {
			fscanf( fp, "%lf", &this->trial_scores[i].score[j] );
		}
	}
	fclose(fp);

	return 1; // continue with scores in this test
}
