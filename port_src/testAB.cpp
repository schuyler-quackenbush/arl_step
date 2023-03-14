#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "defineValues.h"
#include "license.h"
#include "session.h"
#include "config.h"
#include "testBase.h"
#include "testAB.h"
#include "portControls.h"
#include "utils.h"

/* class constructor */
TestAB::TestAB( int levels ) : TestBase( )
{
	num_levels = levels;;
}

/* class desctructor */
TestAB::~TestAB()
{
	;
}

/* member functions */

int TestAB::initThisTest( SessionInfo *psi, Config *p ) 
{
	/* set test ID */
	if (num_levels == 5) {
		test_id = T_AB5;
		my_strncpy(test_methodology, "AB5 Comparative Test", LAB_LEN);
	}
	else {
		test_id = T_AB7;
		my_strncpy(test_methodology, "AB7 Comparative Test", LAB_LEN);
	}
	psi->test_id = test_id;

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

int TestAB::checkThisTest( Session *p )
{
	if ( p->session_info.num_systems != 2  ) {
		showError( (char*)"Invalid session file, AB Comparison must have 2 test conditions." );
		return 0;
	}
    if (p->session_info.has_open_ref) {
        showError( (char*)"Invalid session file, AB Comparison cannot have open reference." );
        return 0;
    }
	return 1;  
}

///* initialize score sliders */
int TestAB::initGUI( void )
{
    SessionInfo *psi = &this->session->session_info;
    switch (num_levels) {
        case 5:
            init_grades(this->test_id, psi->num_systems, 2.0, -2.0, 4.0, 0.0, "%.0f" );
            break;
        case 7:
            init_grades(this->test_id, psi->num_systems, 3.0, -3.0, 6.0, 0.0, "%.0f" );
            break;
    }
    return 1;
}

///* read scores from GUI */
int TestAB::getScores()
{
    SessionInfo *psi = &session->session_info;
    TrialScores *pts = &this->trial_scores[psi->trial_map[psi->tnum]];

    if (psi->sys_map[0] == 0) {
        pts->score[0] = get_scrollbar_score(0);
    }
    else {
        /* A and B are reversed wrt session file */
        pts->score[0] = -get_scrollbar_score(0);
    }
    return 1;
}

/* Write scores to score file */
int TestAB::writeThisTestScores(FILE *fp) 
{
	int i, k;
	SessionInfo *psi = &this->session->session_info;

	/* one scores per trial */
	fprintf( fp, "Lab\tListener\tSession\tFileA\tFileB\tSig\tSysA\tSysB\tScore\n" );
	k = psi->has_open_ref;
	for( i=0; i<psi->num_trials; i++ ) {
		fprintf( fp, "%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%.0f\n",
			config->lab_id, 
			config->listener_id, 
			config->session_id,
			psi->trial[i].system[k].basefilename, //includes open reference
			psi->trial[i].system[k+1].basefilename, //includes open reference
			psi->trial[i].sig_lab,
			psi->trial[i].system[k].sys_lab, //includes open reference
			psi->trial[i].system[k+1].sys_lab, //includes open reference
			this->valid_license  ? this->trial_scores[i].score[0] : 0 );//does NOT include open reference 
	}
	return 1; 
}
