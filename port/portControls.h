#ifndef __PORTCONTROLS_H
#define __PORTCONTROLS_H

/* message boxes */
void showError( char *msg );
void showStatus( char *msg );
void showMessage( char *msg );
int  showQuestion( char *msg );

/* scroll bars */
void init_grades(int test_id, int nsys,
		double top, double bottom, double range,
		double score, const char *fmt );
void init_buttons(int test_id, int has_open_ref);
double get_scrollbar_score( int i);

/* timer for position scroll bar */
void setScrollTimer( void );

/* radio buttons */
int get_radio_value( char button );

/* progress bar */
void portMakeProgress( void );
void portShowProgress( const char *mode, const char *label, int num_steps );
void portIncProgress( void );
void portCloseProgress( void );

/* administrator panel */
void set_admin( void );
void lock_admin_options_panel( int status );
//void lock_other_score_sliders( SessionInfo *psi, int id_playing );
//void unlock_all_score_sliders( SessionInfo *psi );

#endif /* __PORTCONTROLS_H */
