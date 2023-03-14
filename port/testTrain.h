#ifndef __TRAIN_H
#define __TRAIN_H

#include "testBase.h"
#include "defineValues.h"

class TestTrain : public TestBase {
public:
	TestTrain( int id );
	~TestTrain( );
	// virtual functions
	int initThisTest( SessionInfo *session_info, Config *config );
	int checkThisTest( Session *session );
	int initGUI( void );
	int getScores( void );
	int writeThisTestScores( FILE *fp );
};

#endif /* __TRAIN_H */
