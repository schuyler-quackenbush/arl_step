#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "defineValues.h"
#include "license.h"
#include "session.h"
#include "config.h"
#include "testBase.h"
#include "testTrain.h"
#include "portControls.h"
#include "utils.h"

/* class constructor */
TestTrain::TestTrain( int id ) : TestBase( )
{
		test_id = id;
}

/* class desctructor */
TestTrain::~TestTrain()
{
	;
}

/* member functions */

int TestTrain::initThisTest( SessionInfo *psi, Config *p ) 
{
	/* set test ID */
	psi->test_id = test_id;
	if (test_id == T_TRAIN) {
		my_strncpy(test_methodology, "Training", LAB_LEN);
	}
	else { /* T_PLAYLIST */
		my_strncpy(test_methodology, "Playlist", LAB_LEN);
	}

	/* this test does not have scores */
	write_scores = 0;

	/* default values */
	psi->has_open_ref = 1;
	p->random_trials = 0;
	p->random_conditions = 0;
	p->only_score_item_playing = 0; 
	p->approve_score = 0;
	p->show_system = 0;
	p->show_prev = 1;
	p->play_mono_as_stereo = 1;

	return 1;
}

int TestTrain::checkThisTest( Session *p )
{
	if ( p->session_info.num_systems < 1 || 
		p->session_info.num_systems > MUSHRA_MAX_COND) {
		char msg[MSG_LEN];
		sprintf(msg, 
			"Invalid session file, Training must have at least 1 but not more than %d test conditions.",
			MUSHRA_MAX_COND);
		showError( msg );
		return 0;
	}
	return 1;  
}

/* initialize score sliders */
int TestTrain::initGUI( void ) 
{
	SessionInfo *psi = &this->session->session_info;
	if (test_id == T_TRAIN) {
		init_grades(this->test_id, psi->num_systems, 100.0, 0.0, 100.0, 100.0, "%.0f");
	}
	else { /* T_PLAYLIST */
		/* don't show any buttons */
		init_grades(this->test_id, 0, 100.0, 0.0, 100.0, 100.0, "%.0f");
	}
	return 1;
}

/* read scores from GUI */
int TestTrain::getScores() 
{
	return 1;
}

/* Write scores to score file */
int TestTrain::writeThisTestScores( FILE *fp ) 
{
	return 1; 
}
