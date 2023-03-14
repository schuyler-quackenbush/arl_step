#ifndef __AB_H
#define __AB_H

#include "testBase.h"
#include "defineValues.h"

class TestAB : public TestBase {
public:
	TestAB( int levels );
	~TestAB( );
	// virtual functions
	int initThisTest( SessionInfo *session_info, Config *config );
	int checkThisTest( Session *session );
	int initGUI( void );
	int getScores( void );
	int writeThisTestScores( FILE *fp );

	int num_levels;
};

#endif /* __AB_H */
