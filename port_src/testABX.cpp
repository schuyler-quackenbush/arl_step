#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "defineValues.h"
#include "license.h"
#include "session.h"
#include "config.h"
#include "testBase.h"
#include "testABX.h"
#include "portControls.h"
#include "utils.h"

/* class constructor */
TestABX::TestABX( ) : TestBase( )
{
	;
}

/* class desctructor */
TestABX::~TestABX()
{
	;
}

/* member functions */

int TestABX::initThisTest( SessionInfo *psi, Config *p ) 
{
	/* set test ID */
	test_id = T_ABX;
	psi->test_id = test_id;
	my_strncpy(test_methodology, "ABX Test", LAB_LEN);


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

int TestABX::checkThisTest( Session *p )
{
	if ( p->session_info.num_systems != 2 ) {
		showError( (char*)"Invalid session file, ABX must have 2 conditions per trial." );
		return 0;
	}
	return 1;  
}

/* initialize score sliders */
int TestABX::initGUI( void )
{
    /* already randomized A and B
     * now randomize X */
    session->session_info.sys_map[2] = session->coin_toss();

    init_grades(this->test_id, 0, 0, 0, 0, .0, "");
    return 1;
}

///* read scores from GUI */
int TestABX::getScores()
{
    int a, b;
    SessionInfo *psi = &session->session_info;
    TrialScores *pts = &this->trial_scores[psi->trial_map[psi->tnum]];

    a = get_radio_value( 'A' );
    b = get_radio_value( 'B' );
    if( a==0 && b==0 ) {
        showError( (char*)"Either A or B must be selected" );
        return 0;
    }
    pts->score[0] = psi->sys_map[0] == 0 ? 1 : 0;	/* 1 if A is system[0] */
    pts->score[1] = psi->sys_map[2] == psi->sys_map[0] ? 1 : 0;	/* 1 if X is A */
    pts->score[2] = ( ( /* 1 if X is identified correctly */
        ( a && (psi->sys_map[2] == psi->sys_map[0]) ) ||
        ( b && (psi->sys_map[2] == psi->sys_map[1]) ) ) ? 1 : 0 );

    return 1;
}

/* Write scores to score file */
int TestABX::writeThisTestScores(FILE *fp) 
{
	int i;
	char *sysA, *sysB;
	SessionInfo *psi = &this->session->session_info;

	/* one scores per trial */
	fprintf( fp, "Lab\tListener\tSession\tFileA\tFileB\tSig\tSysA\tSysB\tX\tScore\n" );
	for( i=0; i<psi->num_trials; i++ ) {
		sysA = psi->trial[i].system[0].sys_lab;
		sysB = psi->trial[i].system[0+1].sys_lab;
		fprintf( fp, "%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%.0f\n",
			config->lab_id, 
			config->listener_id, 
			config->session_id,
			psi->trial[i].system[0].basefilename, //includes open reference
			psi->trial[i].system[0+1].basefilename, //includes open reference
			psi->trial[i].sig_lab,
			this->trial_scores[i].score[0] == 1 ? sysA : sysB, /* 1 if A is system[0] */
			this->trial_scores[i].score[0] == 1 ? sysB : sysA, /* 1 if A is system[0] */
			this->trial_scores[i].score[1] == 1 ? "A"  : "B" , /* 1 if X is A */
			( this->valid_license  ? this->trial_scores[i].score[2] : 0 ) ); /* 1 if identity of X is correct */
	}
	return 1; 
}
