#include <QMessageBox>
#include "qt_message.h"

#include "defineValues.h"
#include "testBase.h"

QString format_msg(char *title, char *msg) {
    QString Qs_msg = "";
#ifdef Q_OS_DARWIN
    Qs_msg.append(title);
    Qs_msg.append("\n\n");
#endif
    Qs_msg.append(msg);
    return Qs_msg;
}

void
showError( char *msg )
{
    char *title = (char *)"STEP Error";
    QMessageBox msgBox;
    msgBox.setIcon(QMessageBox::Warning);
    msgBox.setWindowTitle(title);
    msgBox.setText(format_msg(title, msg));
    msgBox.exec();
}

void
showInformation( char *title, char *msg )
{
    QMessageBox msgBox;
    msgBox.setWindowTitle(title);
    msgBox.setText(format_msg(title, msg));
    msgBox.exec();
}

void
showMessage( char *msg )
{
    char *title = (char *)"STEP Message";
    QMessageBox msgBox;
    msgBox.setIcon(QMessageBox::Information);
    msgBox.setWindowTitle(title);
    msgBox.setText(format_msg(title, msg));
    msgBox.exec();
}

int
showQuestion( char *msg )
{
    char *title = (char *)"STEP Question";
    int ret_val = 0;
    QMessageBox msgBox;
    msgBox.setIcon(QMessageBox::Question);
    msgBox.setWindowTitle(title);
    msgBox.setText(format_msg(title, msg));
    msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    int ret = msgBox.exec();
    switch (ret) {
        case QMessageBox::Yes:
            ret_val = 1;
            break;
        case QMessageBox::No:
            ret_val = 0;
            break;
    default:
            ret_val = -1;
    }
    return ret_val;
}

void
showStatus( char *msg )
{
    char *title = (char *)"STEP Information";
    QMessageBox msgBox;
    msgBox.setIcon(QMessageBox::Information);
    msgBox.setWindowTitle(title);
    msgBox.setText(format_msg(title, msg));
    msgBox.exec();
}


