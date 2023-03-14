#ifndef __TestBase_H
#define __TestBase_H

#include "defineValues.h"
#include "session.h"
#include "config.h"

typedef struct {
	double score[MAX_SYS];	/* maximum number of systems under test (i.e. sliders) */
} TrialScores;

class TestBase {
public:
	TestBase( );
	~TestBase();
	int initTest( int valid_license, Session *session, Config *config);
    virtual int initThisTest( SessionInfo *session_info, Config *config ) { return 0; }
    virtual int checkThisTest( Session *session ) { return 0; }
    virtual int initGUI( void ) { return 0; }
    virtual int getScores( void ) { return 0; }
    virtual int writeThisTestScores( FILE *fp ) { return 0; }
	int checkScoreFile( void );
	int writePartialScores( void );
	int readPartialScores( void );
	int writeScores( void );

	Config *config;
	Session *session;

	int valid_license;
	int test_id;
	char test_methodology[LAB_LEN];
	int write_scores;
	char score_file[FILE_LEN];
	TrialScores *trial_scores;
	int complete_test;
	int play_id;

    double scores[MAX_SYS], score_top, score_bottom, score_inc, score_initial;
    char score_fmt[8];
};

#endif /* __TestBase_H */
