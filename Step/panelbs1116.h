#ifndef PANELBS1116_H
#define PANELBS1116_H

#include <QWidget>
#include <QSlider>
#include <QLabel>
#include <QPushButton>
#include <QDialog>
#include <ui_panelbs1116.h>

#include "defineValues.h"

QT_BEGIN_NAMESPACE
namespace Ui { class PanelBS1116; }
QT_END_NAMESPACE

class PanelBS1116 : public QDialog
{
    Q_OBJECT

public:
    PanelBS1116(QWidget *parent = nullptr);
    ~PanelBS1116();

    int min_zoom;
    double at, dur;
    int button_playing;
    //The following include REF as index 0, even if element is not present for REF
    QSlider *p_verticalSlider[2+1];
    QLabel *p_sliderValue[2+1];
    QPushButton *p_pushButton[2+1];

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

    void on_verticalSlider_A_valueChanged(int value);

    void on_verticalSlider_B_valueChanged(int value);

    void on_pushButton_REF_clicked();

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

    Ui::PanelBS1116 *ui;
};

#endif // PANELBS1116_H
