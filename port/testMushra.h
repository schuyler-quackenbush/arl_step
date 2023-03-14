#ifndef __MUSHRA_H
#define __MUSHRA_H

#include "testBase.h"
#include "defineValues.h"

class TestMushra : public TestBase {
public:
	TestMushra( );
	~TestMushra( );
	// virtual functions
    int initThisTest( SessionInfo *session_info, Config *config );
	int checkThisTest( Session *session );
    int initGUI( void );
    int getScores( void );
	int writeThisTestScores( FILE *fp );
};

#endif /* __MUSHRA_H */
