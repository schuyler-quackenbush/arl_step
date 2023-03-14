#ifndef __QT_MSGBOX_H
#define __QT_MSGBOX_H

/* message boxes */
void showError( char *msg );
void showInformation( char *title, char *msg );
void showMessage( char *msg );
int  showQuestion( char *msg );
void showStatus( char *msg );

#endif /* __QT_MSGBOX_H */
