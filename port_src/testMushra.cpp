#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "defineValues.h"
#include "license.h"
#include "session.h"
#include "config.h"
#include "testBase.h"
#include "testMushra.h"
#include "portControls.h"
#include "utils.h"

/* class constructor */
TestMushra::TestMushra( ) : TestBase( )
{
	;
}

/* class desctructor */
TestMushra::~TestMushra()
{
	;
}

/* member functions */

int TestMushra::initThisTest( SessionInfo *psi, Config *p ) 
{
	/* set test ID */
	test_id = T_MUSHRA;
	psi->test_id = test_id;
	my_strncpy(test_methodology, "MUSHRA Test", LAB_LEN);

	/* this test has scores */
	write_scores = 1;

	/* default values */
	psi->has_open_ref = 1;
	p->random_trials = 1;
	p->random_conditions = 1;
    p->only_score_item_playing = 1;
    p->check_score = 1;
	p->approve_score = 1;
	p->show_system = 0;
	p->show_prev = 0;
	p->play_mono_as_stereo = 1;

	return 1;
}

int TestMushra::checkThisTest( Session *p )
{
	if ( p->session_info.num_systems < 1 || 
		p->session_info.num_systems > MUSHRA_MAX_COND) {
		char msg[MSG_LEN];
		sprintf(msg, 
			"Invalid session file, MUSHRA must have "
			"at least 1 but not more than %d test conditions.",
			MUSHRA_MAX_COND);
		showError( msg );
		return 0;
	}
	return 1;  
}

/* initialize score sliders */
int TestMushra::initGUI( void )
{
    SessionInfo *psi = &this->session->session_info;
    init_grades(this->test_id, psi->num_systems, 100.0, 0.0, 100.0, 100.0, "%.0f");
    return 1;
}

/* read scores from GUI */
int TestMushra::getScores()
{
    int i, hr_100;
    double score;
    SessionInfo *psi = &session->session_info;
    TrialScores *pts = &this->trial_scores[psi->trial_map[psi->tnum]];

    hr_100 = 0;
    for( i=0; i<psi->num_systems; i++ ) {
        score = get_scrollbar_score(i);
        if (score == 100.0)
            hr_100++;
        pts->score[psi->sys_map[i]] = score;
			
    }
    if (config->check_score && hr_100 == 0) {
        showError((char*)"One system must be scored at 100");
        return 0;
    }
    return 1;
}

/* Write scores to score file */
int TestMushra::writeThisTestScores(FILE *fp) 
{
	int i, j, k;
	SessionInfo *psi = &this->session->session_info;

	fprintf( fp, "Lab\tListener\tSession\tFile\tSig\tSys\tScore\n" );
	k = psi->has_open_ref;
	for( i=0; i<psi->num_trials; i++ ) {
		for( j=0; j<psi->num_systems; j++ ) {
			fprintf( fp, "%s\t%s\t%s\t%s\t%s\t%s\t%.1f\n",
				config->lab_id, 
				config->listener_id, 
				config->session_id,
				psi->trial[i].system[j+k].basefilename, //includes open reference
				psi->trial[i].sig_lab,
				psi->trial[i].system[j+k].sys_lab, //includes open reference
				this->valid_license  ? this->trial_scores[i].score[j] : 0 ); //does NOT include open reference 
		}
	}
	return 1; 
}
