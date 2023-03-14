#ifndef PANELAB7_H
#define PANELAB7_H

#include <QWidget>
#include <QSlider>
#include <QLabel>
#include <QPushButton>
#include <QDialog>
#include <ui_panelab7.h>

#include "defineValues.h"

QT_BEGIN_NAMESPACE
namespace Ui { class PanelAB7; }
QT_END_NAMESPACE

class PanelAB7 : public QDialog
{
    Q_OBJECT

public:
    PanelAB7(QWidget *parent = nullptr);
    ~PanelAB7();

    int min_zoom;
    double at, dur;
    int button_playing;
    //The following include REF as index 0, even if element is not present for REF
    QSlider *p_verticalSlider[MAX_SYS+1];
    QLabel *p_sliderValue[MAX_SYS+1];
    QPushButton *p_pushButton[MAX_SYS+1];

    QLabel *abLabel[7];

    int approve_question_box( char *msg );
    void closeEvent (QCloseEvent *event);

    void initPP();

    void initPSS();

    void initTrial();

    void startTrial();

    int nextTrial();

    void positionTimer();

    void set_positionBar(int v);

    //  virtual int initThisTest( SessionInfo *session_info, Config *config ) { return 0; }
    //	virtual int checkThisTest( Session *session ) { return 0; }
    //	virtual int initGUI( void ) { return 0; }
    //	virtual int getScores( void ) { return 0; }
    //	virtual int writeThisTestScores( FILE *fp ) { return 0; }

private slots:

    void on_verticalSlider_valueChanged(int value);

    void on_pushButton_A_clicked();

    void on_pushButton_B_clicked();

    void on_pushButton_Play_clicked();

    void on_pushButton_Loop_clicked();

    void on_pushButton_Next_clicked();

    void on_horizontalScrollBar_Start_valueChanged(int value);

    void on_horizontalScrollBar_Stop_valueChanged(int value);

private:
    void radio_button(QPushButton *pb);
    void show_pl_status(void);

    Ui::PanelAB7 *ui;

};
#endif // PANELAB7_H
