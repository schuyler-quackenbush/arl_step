#ifndef __MOS_H
#define __MOS_H

#include "testBase.h"
#include "defineValues.h"

class TestMOS : public TestBase {
public:
	TestMOS( );
	~TestMOS( );
	// virtual functions
	int initThisTest( SessionInfo *session_info, Config *config );
	int checkThisTest( Session *session );
	int initGUI( void );
	int getScores( void );
	int writeThisTestScores( FILE *fp );
};

#endif /* __MOS_H */
