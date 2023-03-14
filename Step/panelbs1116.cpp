#include <QLayout>
#include <QTimer>
#include "paneladmin.h"
#include "panelbs1116.h"
#include "ui_panelbs1116.h"
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

PanelBS1116::PanelBS1116(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::PanelBS1116)
{
    ui->setupUi(this);

    //Initialize pointers to signal sliders
    //No slider for REF
    p_verticalSlider[0] = 0;
    p_verticalSlider[1] = ui->verticalSlider_A;
    p_verticalSlider[2] = ui->verticalSlider_B;

    //Initialize pointers to slider value labels
    p_sliderValue[0] = 0;
    p_sliderValue[1] = ui->label_SliderValue_A;
    p_sliderValue[2] = ui->label_SliderValue_B;
    //Initialize pointers to signal pushbuttons
    p_pushButton[0] = ui->pushButton_REF;
    p_pushButton[1] = ui->pushButton_A;
    p_pushButton[2] = ui->pushButton_B;

    //Disable sliders if only_score_while_playing = true
    if (config->only_score_item_playing) {
        for (int i=1; i<=2; i++) {
            p_verticalSlider[i]->setEnabled(false);
        }
    }

    //Set initial slider value
    ui->label_SliderValue_A->setText(QString::number(50/10.0, 'f', 1));
    ui->label_SliderValue_B->setText(QString::number(50/10.0, 'f', 1));
    ui->verticalSlider_A->setMinimum(10);
    ui->verticalSlider_A->setMaximum(50);


    //All buttons are de-selected
    for (int i=0; i<=2; i++) {
        p_pushButton[i]->isCheckable();
        p_pushButton[i]->setChecked(false);
    }

    //Remove auto default focus from all buttons
    for (int i=0; i<=2; i++) {
        p_pushButton[i]->setAutoDefault(false);
    }
    ui->pushButton_Play->setAutoDefault(false);
    ui->pushButton_Loop->setAutoDefault(false);
    ui->pushButton_Next->setAutoDefault(false);

    //Set the default signal for play
    //is there an open reference?
    if (session->session_info.has_open_ref == 1) {
        button_playing = 0; //Select REF
        ui->pushButton_REF->setChecked(true);
    }
    else {
        ui->pushButton_REF->hide(); //hide REF button
        button_playing = 1; //Select A
        ui->pushButton_A->setChecked(true);
    }

    //Set Play, Loop as chekable
    ui->pushButton_Play->isCheckable();
    ui->pushButton_Loop->isCheckable();

    //Set min_zoom to 10% of audio duration
    min_zoom = 10;

    initPSS();

    initTrial();

    startTrial();
}

PanelBS1116::~PanelBS1116()
{
    delete ui;
}

//////////////////////////////////////////////////
///
/// Score sliders, values and buttons
///
//////////////////////////////////////////////////
//Slider change is written to value label
void PanelBS1116::on_verticalSlider_A_valueChanged(int value)
{
    ui->label_SliderValue_A->setText(QString::number(value/10.0, 'f', 1));
}
void PanelBS1116::on_verticalSlider_B_valueChanged(int value)
{
    ui->label_SliderValue_B->setText(QString::number(value/10.0, 'f', 1));
}

//Signal pushbuttons
void PanelBS1116::on_pushButton_REF_clicked()
{
    QPushButton *pb = (QPushButton *)sender();
    radio_button(pb);
    //send to main program
    //next_sys starts at 0 for REF, 1 for A, etc.
    play->paf->next_sys = 0; //REF is always 0
}
void PanelBS1116::on_pushButton_A_clicked()
{
    QPushButton *pb = (QPushButton *)sender();
    radio_button(pb);
    //send to main program
    play->paf->next_sys = session->session_info.sys_map[0] + 1; //There is always a REF
}
void PanelBS1116::on_pushButton_B_clicked()
{
    QPushButton *pb = (QPushButton *)sender();
    radio_button(pb);
    //send to main program
    play->paf->next_sys = session->session_info.sys_map[1] + 1; //There is always a REF
}

#define TESTPANEL PanelBS1116
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


