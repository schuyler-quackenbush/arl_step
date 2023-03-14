#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "defineValues.h"
#include "license.h"
#include "session.h"
#include "config.h"
#include "testBase.h"
#include "testMOS.h"
#include "portControls.h"
#include "utils.h"

/* class constructor */
TestMOS::TestMOS( ) : TestBase( )
{
	;
}

/* class desctructor */
TestMOS::~TestMOS()
{
	;
}

/* member functions */

int TestMOS::initThisTest( SessionInfo *psi, Config *p ) 
{
	/* set test ID */
	test_id = T_MOS;
	psi->test_id = test_id;
	my_strncpy(test_methodology, "MOS ACR Test", LAB_LEN);

	/* this test has scores */
	write_scores = 1;

	/* default values */
	psi->has_open_ref = 0;
	p->random_trials = 1;
	p->random_conditions = 1;
	p->only_score_item_playing = 0; 
	p->approve_score = 1;
	p->show_system = 0;
	p->show_prev = 0;
	p->play_mono_as_stereo = 1;

	return 1;
}

int TestMOS::checkThisTest( Session *p )
{
	if ( p->session_info.num_systems !=  1) {
		showError( (char*)"Invalid session file, MOS must have 1 test condition." );
		return 0;
	}
	return 1;  
}

/* initialize score sliders */
int TestMOS::initGUI( void ) 
{
	SessionInfo *psi = &this->session->session_info;
	init_grades(this->test_id, psi->num_systems, 5.0, 1.0, 4.0, 5.0, "%.0f");
	/* if open REF, then show REF and A
	 * otherwise don't show any buttons */
	init_buttons(this->test_id, psi->has_open_ref);
	return 1;
}

/* read scores from GUI */
int TestMOS::getScores() 
{
	SessionInfo *psi = &session->session_info;
	TrialScores *pts = &this->trial_scores[psi->trial_map[psi->tnum]];

	pts->score[0] = get_scrollbar_score(0);
	return 1;
}

/* Write scores to score file */
int TestMOS::writeThisTestScores(FILE *fp) 
{
	int i, k;
	SessionInfo *psi = &this->session->session_info;

	fprintf( fp, "Lab\tListener\tSession\tFile\tSys\tScore\n" );
	k = psi->has_open_ref;
	for( i=0; i<psi->num_trials; i++ ) {
		fprintf( fp, "%s\t%s\t%s\t%s\t%s\t%.1f\n",
			config->lab_id, 
			config->listener_id, 
			config->session_id,
			psi->trial[i].system[k].basefilename, //includes open reference
			psi->trial[i].sig_lab,
			this->valid_license  ? this->trial_scores[i].score[0] : 0 ); //does NOT include open reference 
	}
	return 1; 
}
