#ifndef __ABX_H
#define __ABX_H

#include "testBase.h"
#include "defineValues.h"

class TestABX : public TestBase {
public:
	TestABX( );
	~TestABX( );
	// virtual functions
	int initThisTest( SessionInfo *session_info, Config *config );
	int checkThisTest( Session *session );
	int initGUI( void );
	int getScores( void );
	int writeThisTestScores( FILE *fp );
};

#endif /* __ABX_H */
