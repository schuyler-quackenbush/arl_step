#ifndef QT_INTERFACE_H
#define QT_INTERFACE_H

/* scroll bars and buttons */
void init_grades(int test_id, int nsys,
        double top, double bottom, double range,
        double score, const char *fmt ) {;}
void init_buttons(int test_id, int has_open_ref) {;}

/* timer for position scroll bar */
void setScrollTimer( void ) {;}
double get_scrollbar_score( int i) { return 0;}

/* radio buttons */
int get_radio_value( char button ) { return 0;}

/* progress bar */
void portMakeProgress( void ) {;}
void portShowProgress( const char *mode, const char *label, int num_steps ) {;}
void portIncProgress( void ) {;}
void portCloseProgress( void ) {;}

/* administrator panel */
void set_admin( ) {;}
void lock_admin_options_panel( int status ) {;}

#endif //QT_INTERFACE_H
