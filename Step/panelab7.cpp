#include <QLayout>
#include <QTimer>
#include "paneladmin.h"
#include "panelab7.h"
#include "ui_panelab7.h"
#include "qt_message.h"

#include "defineValues.h"
#include "allTestIncludes.h"
#include "config.h"
#include "session.h"
#include "transition.h"
#include "transBuff.h"
#include "play.h"
#include "trialAudioFiles.h"
#include "paUtils.h"

extern License *license;
extern Config *config;
extern Session *session;
extern TestBase *this_test;
extern TransBuff *trans_buff;
extern Play *play;
extern TrialAudioFiles *trial_audio_files;

PanelAB7::PanelAB7(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::PanelAB7)
{
    ui->setupUi(this);

    //Initialize pointers to signal sliders
    //No slider for REF
    p_verticalSlider[0] = 0;
    p_verticalSlider[1] = ui->verticalSlider;
    //Initialize pointers to slider value labels
    p_sliderValue[0] = 0;
    p_sliderValue[1] = ui->label_SliderValue;
    //Initialize pointers to signal pushbuttons
    p_pushButton[0] = 0;
    p_pushButton[1] = ui->pushButton_A;
    p_pushButton[2] = ui->pushButton_B;

    //Set initial slider limits, value and label
    ui->label_SliderValue->setText(QString::number(0/10.0, 'f', 1));

    //All buttons are de-selected
    for (int i=1; i<=2; i++) {
        p_pushButton[i]->isCheckable();
        p_pushButton[i]->setChecked(false);
    }

    //Remove auto default focus from all buttons
    for (int i=1; i<=2; i++) {
        p_pushButton[i]->setAutoDefault(false);
    }
    ui->pushButton_Play->setAutoDefault(false);
    ui->pushButton_Loop->setAutoDefault(false);
    ui->pushButton_Next->setAutoDefault(false);

    //Set the default signal for play
    button_playing = 0; //Select A (there is no REF, so A is index 0)
    p_pushButton[1]->setChecked(true); //But A is index 1 here
    p_verticalSlider[1]->setEnabled(true);

    //Set Play, Loop as chekable
    ui->pushButton_Play->isCheckable();
    ui->pushButton_Loop->isCheckable();

    //Set min_zoom to 10% of audio duration
    min_zoom = 10;

    initPSS();

    initTrial();

    startTrial();
}

PanelAB7::~PanelAB7()
{
    delete ui;
}

//////////////////////////////////////////////////
///
/// Score sliders, values and buttons
///
//////////////////////////////////////////////////
//Slider change is written to value label
void PanelAB7::on_verticalSlider_valueChanged(int value)
{
    ui->label_SliderValue->setText(QString::number(value/10.0, 'f', 1));
}

//REF is not part of sys_map
void PanelAB7::on_pushButton_A_clicked()
{
    QPushButton *pb = (QPushButton *)sender();
    radio_button(pb);
    //send to main program
    play->paf->next_sys = session->session_info.sys_map[0] + 0; //There is never a REF
}
void PanelAB7::on_pushButton_B_clicked()
{
    QPushButton *pb = (QPushButton *)sender();
    radio_button(pb);
    //send to main program
    play->paf->next_sys = session->session_info.sys_map[1] + 0; //There is never a REF
}

#define TESTPANEL PanelAB7
//////////////////////////////////////////////////
///
/// Init, Run, Next Trial functions
///
//////////////////////////////////////////////////
#include "initRunNextTrial.cpp"

//////////////////////////////////////////////////
///
/// Emulate Radio Buttons
///
/// Next button
///
/// Play Loop buttons, Status line
///
/// Position Indicator, Start Stop sliders
///
//////////////////////////////////////////////////
#include "common_panel_controls.cpp"


