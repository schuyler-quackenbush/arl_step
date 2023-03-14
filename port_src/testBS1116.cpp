#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "defineValues.h"
#include "license.h"
#include "session.h"
#include "config.h"
#include "testBase.h"
#include "testBS1116.h"
#include "portControls.h"
#include "utils.h"

/* class constructor */
TestBS1116::TestBS1116( ) : TestBase( )
{
	;
}

/* class desctructor */
TestBS1116::~TestBS1116()
{
	;
}

/* member functions */

int TestBS1116::initThisTest( SessionInfo *psi, Config *p ) 
{
	/* set test ID */
	test_id = T_BS1116;
	psi->test_id = test_id;
	my_strncpy(test_methodology, "BS.1116 Test", LAB_LEN);


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

int TestBS1116::checkThisTest( Session *p )
{
	if ( p->session_info.has_open_ref != 1 ||
		p->session_info.num_systems != 2 ) {
		char msg[MSG_LEN];
		sprintf(msg, 
			"Invalid session file, BS.1116 must have Open Reference (REF) "
			"plus 2 test conditions.");
		showError( msg );
		return 0;
	}
	return 1;  
}

int TestBS1116::initGUI( void ) 
{
	SessionInfo *psi = &this->session->session_info;
	init_grades(this->test_id, psi->num_systems, 5.0, 1.0, 40.0, 5.0, "%.1f");
	return 1;
}

/* Read scores from GUI */
int TestBS1116::getScores() 
{
	int i, hr_5;
	double score;
	SessionInfo *psi = &session->session_info;
	TrialScores *pts = &this->trial_scores[psi->trial_map[psi->tnum]];

	hr_5 = 0;
	for( i=0; i<psi->num_systems; i++ ) {
		score = get_scrollbar_score(i);
		if (score == 5.0)
			hr_5++;
		pts->score[psi->sys_map[i]] = score;
			
	}
	if (config->check_score && hr_5 == 0) {
		showError((char*)"One of A or B must be scored to 5");
		return 0;
	}
	return 1;
}

/* Write scores to score file */
int TestBS1116::writeThisTestScores(FILE *fp) 
{
	int i, k;
	SessionInfo *psi = &this->session->session_info;

	/* two scores per trial */
	fprintf( fp, "Lab\tListener\tSession\tFileA\tFileB\tSig\tSysA\tSysB\tScoreA\tScoreB\n" );
	k = psi->has_open_ref;
	for( i=0; i<psi->num_trials; i++ ) {
		fprintf( fp, "%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%.1f\t%.1f\n",
			config->lab_id, 
			config->listener_id, 
			config->session_id,
			psi->trial[i].system[k].basefilename, //includes open reference
			psi->trial[i].system[k+1].basefilename, //includes open reference
			psi->trial[i].sig_lab,
			psi->trial[i].system[k].sys_lab, //includes open reference
			psi->trial[i].system[k+1].sys_lab, //includes open reference
			this->valid_license  ? this->trial_scores[i].score[0] : 0, //does NOT include open reference
			this->valid_license  ? this->trial_scores[i].score[1] : 0 ); //does NOT include open reference
	}
	return 1; 
}
