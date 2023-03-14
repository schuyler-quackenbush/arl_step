#ifndef PANELMUSHRA_H
#define PANELMUSHRA_H

#include <QWidget>
#include <QSlider>
#include <QLabel>
#include <QPushButton>
#include <QDialog>
#include <ui_panelmushra.h>

#include "defineValues.h"

QT_BEGIN_NAMESPACE
namespace Ui { class PanelMushra; }
QT_END_NAMESPACE

class PanelMushra : public QDialog
{
    Q_OBJECT

public:
    PanelMushra(QWidget *parent = nullptr);
    ~PanelMushra();

    int min_zoom;
    double at, dur;
    int button_playing;
    //The following include REF as index 0, even if element is not present for REF
    QSlider *p_verticalSlider[MAX_SYS+1];
    QLabel *p_sliderValue[MAX_SYS+1];
    QPushButton *p_pushButton[MAX_SYS+1];

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

    void on_verticalSlider_C_valueChanged(int value);

    void on_verticalSlider_D_valueChanged(int value);

    void on_verticalSlider_E_valueChanged(int value);

    void on_verticalSlider_F_valueChanged(int value);

    void on_verticalSlider_G_valueChanged(int value);

    void on_verticalSlider_H_valueChanged(int value);

    void on_verticalSlider_I_valueChanged(int value);

    void on_verticalSlider_J_valueChanged(int value);

    void on_verticalSlider_K_valueChanged(int value);

    void on_verticalSlider_L_valueChanged(int value);

    void on_pushButton_REF_clicked();

    void on_pushButton_A_clicked();

    void on_pushButton_B_clicked();

    void on_pushButton_C_clicked();

    void on_pushButton_D_clicked();

    void on_pushButton_E_clicked();

    void on_pushButton_F_clicked();

    void on_pushButton_G_clicked();

    void on_pushButton_H_clicked();

    void on_pushButton_I_clicked();

    void on_pushButton_J_clicked();

    void on_pushButton_K_clicked();

    void on_pushButton_L_clicked();

    void on_pushButton_Play_clicked();

    void on_pushButton_Loop_clicked();

    void on_pushButton_Next_clicked();

    void on_horizontalScrollBar_Start_valueChanged(int value);

    void on_horizontalScrollBar_Stop_valueChanged(int value);

private:
    void radio_button(QPushButton *pb);
    void show_pl_status(void);

    Ui::PanelMushra *ui;
};
#endif // PANELMUSHRA_H
