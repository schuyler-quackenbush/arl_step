#include "testBase.h"

/* scroll bars */
void init_grades(int test_id, int nsys,
        double top, double bottom, double range,
        double score, const char *fmt ) {;}
// {
//     int i;
//     this_test->score_top = top;
//     this_test->score_bottom = bottom;
//     strncpy(this_test->score_fmt, fmt, 8);
//     this_test->score_inc = (top-bottom)/10;

//     for (i=0; i<nsys; i++) {
//         this_test->scores[i] = score;
//     }
// }

void init_buttons(int test_id, int has_open_ref) {;}

/* timer for position scroll bar */
void setScrollTimer( void ) {;}

double get_scrollbar_score( int i)
{
    extern TestBase *this_test;
    //make sure these are read off of sliders first!
    return this_test->scores[1+i]; //REF is index 0
}

/* radio buttons */
int get_radio_value( char button ) { return 0;}
//{
// 	int c;
// 	printf("========================\n");
// 	printf("Is X signal %c? (Y/N)\n", button);
// 	c = my_getchar();
// 	printf("========================\n");
// 	if (toupper(c) == 'Y') {
// 		return 1;
// 	}
// 	return 0;
// }

/* progress bar */
void portMakeProgress( void ) {;}
void portShowProgress( const char *mode, const char *label, int num_steps ) {;}
void portIncProgress( void ) {;}
void portCloseProgress( void ) {;}

/* administrator panel */
void set_admin( ) {;}
void lock_admin_options_panel( int status ) {;}
