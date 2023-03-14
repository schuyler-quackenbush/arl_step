#ifndef __TestBS1116_H
#define __TestBS1116_H

#include "testBase.h"
#include "defineValues.h"

class TestBS1116 : public TestBase {
public:
	TestBS1116( );
	~TestBS1116( );
	// virtual functions
	int initThisTest( SessionInfo *session_info, Config *config );
	int checkThisTest( Session *session );
	int initGUI( void );
	int getScores( void );
	int writeThisTestScores( FILE *fp );
};

#endif /* __TestBS1116_H */
