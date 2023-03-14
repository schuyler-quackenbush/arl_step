#include <stdio.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "portControls.h"
#include "tty_interface.h"

/* globals */
double scores[MAX_SYS], score_top, score_bottom, score_inc;
char score_fmt[8];

/* message boxes */
void
showError( char *msg_str )
{
	fprintf(stderr, "========================\n");
	fprintf(stderr, "STEP Error\n");
	fprintf(stderr, "%s\n", msg_str);
	fprintf(stderr, "CR to continue\n");
	fprintf(stderr, "========================\n");
	my_getchar();
}

void
showStatus( char *msg_str )
{
	fprintf(stderr, "========================\n");
	fprintf(stderr, "STEP Status\n");
	fprintf(stderr, "%s\n", msg_str);
	fprintf(stderr, "CR to continue\n");
	fprintf(stderr, "========================\n");
	my_getchar();
}

void
showMessage( char *msg_str )
{
	fprintf(stderr, "========================\n");
	fprintf(stderr, "STEP Message\n");
	fprintf(stderr, "%s:\n\n", title);
	fprintf(stderr, "%s\n", msg_str);
	fprintf(stderr, "========================\n");
}

int
showQuestion( char *msg_str )
{
	int c;
	fprintf(stderr, "========================\n");
	fprintf(stderr, "STEP Question\n");
	fprintf(stderr, "%s\n", msg_str);
	fprintf(stderr, "Response (Y/N): ");
	c = my_getchar();
	fprintf(stderr, "========================\n");
	if (toupper(c) == 'Y') {
		return 1;
	}
	return 0;
}

/* scroll bars */
void 
init_grades(int test_id, int nsys,
		double top, double bottom, double range,
		double score, const char *fmt )
{
	int i;
	score_top = top;
	score_bottom = bottom;
	strncpy(score_fmt, fmt, 8);
	score_inc = (top-bottom)/10;

	for (i=0; i<nsys; i++) {
		scores[i] = score;
	}
}

void 
init_buttons(int test_id, int has_open_ref)
{
	return; /* nothing needed */
}

/* timer for position scroll bar */
void setScrollTimer( void ) {;}

double get_scrollbar_score( int i)
{
	return scores[i];
}

/* radio buttons */
int get_radio_value( char button )
{
	int c;
	printf("========================\n");
	printf("Is X signal %c? (Y/N)\n", button);
	c = my_getchar();
	printf("========================\n");
	if (toupper(c) == 'Y') {
		return 1;
	}
	return 0;
}

/* progress bar */
void portMakeProgress( void ) {;}
void portShowProgress( const char *mode, const char *label, int num_steps ) {;}
void portIncProgress( void ) {;}
void portCloseProgress( void ) {;}

/* administrator panel */
void set_admin( ) {;}
void lock_admin_options_panel( int status ) {;}
