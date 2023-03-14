#include <QLayout>
#include <QTimer>
#include "paneladmin.h"
#include "panelmushra.h"
#include "ui_panelmushra.h"
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

PanelMushra::PanelMushra(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::PanelMushra)
{
    ui->setupUi(this);

    //Initialize pointers to signal sliders
    //No slider for REF
    p_verticalSlider[0] = 0;
    p_verticalSlider[1] = ui->verticalSlider_A;
    p_verticalSlider[2] = ui->verticalSlider_B;
    p_verticalSlider[3] = ui->verticalSlider_C;
    p_verticalSlider[4] = ui->verticalSlider_D;
    p_verticalSlider[5] = ui->verticalSlider_E;
    p_verticalSlider[6] = ui->verticalSlider_F;
    p_verticalSlider[7] = ui->verticalSlider_G;
    p_verticalSlider[8] = ui->verticalSlider_H;
    p_verticalSlider[9] = ui->verticalSlider_I;
    p_verticalSlider[10] = ui->verticalSlider_J;
    p_verticalSlider[11] = ui->verticalSlider_K;
    p_verticalSlider[12] = ui->verticalSlider_L;

    //Initialize pointers to slider value labels
    p_sliderValue[0] = 0;
    p_sliderValue[1] = ui->label_SliderValue_A;
    p_sliderValue[2] = ui->label_SliderValue_B;
    p_sliderValue[3] = ui->label_SliderValue_C;
    p_sliderValue[4] = ui->label_SliderValue_D;
    p_sliderValue[5] = ui->label_SliderValue_E;
    p_sliderValue[6] = ui->label_SliderValue_F;
    p_sliderValue[7] = ui->label_SliderValue_G;
    p_sliderValue[8] = ui->label_SliderValue_H;
    p_sliderValue[9] = ui->label_SliderValue_I;
    p_sliderValue[10] = ui->label_SliderValue_J;
    p_sliderValue[11] = ui->label_SliderValue_K;
    p_sliderValue[12] = ui->label_SliderValue_L;
    //Initialize pointers to signal pushbuttons
    p_pushButton[0] = ui->pushButton_REF;
    p_pushButton[1] = ui->pushButton_A;
    p_pushButton[2] = ui->pushButton_B;
    p_pushButton[3] = ui->pushButton_C;
    p_pushButton[4] = ui->pushButton_D;
    p_pushButton[5] = ui->pushButton_E;
    p_pushButton[6] = ui->pushButton_F;
    p_pushButton[7] = ui->pushButton_G;
    p_pushButton[8] = ui->pushButton_H;
    p_pushButton[9] = ui->pushButton_I;
    p_pushButton[10] = ui->pushButton_J;
    p_pushButton[11] = ui->pushButton_K;
    p_pushButton[12] = ui->pushButton_L;

    //Disable sliders if only_score_while_playing = true
    if (config->only_score_item_playing) {
        for (int i=1; i<=MM_SYS; i++) {
            p_verticalSlider[i]->setEnabled(false);
        }
    }

    //Hide unused sliders
    for (int i=session->session_info.num_systems+1; i<=MM_SYS; i++) {
        p_verticalSlider[i]->hide();
        p_sliderValue[i]->hide();
        p_pushButton[i]->hide();
    }

    //Hide REF button if no open ref
    if (session->session_info.has_open_ref == 0) {
        p_pushButton[0]->hide();
    }

    //Set vertical size for all sliders
    for (int i=1; i<=MM_SYS; i++) {
        p_verticalSlider[i]->setMinimumHeight(248);
    }

    //Fix slider size
    ui->gridGroupBox_Sliders->adjustSize();
    int new_x = 780 - 53*(MM_SYS-session->session_info.num_systems);
    new_x = (new_x < 500) ? 500 : new_x;
    this->setFixedSize(new_x,600);

    //All buttons are de-selected
    for (int i=0; i<=MM_SYS; i++) {
        p_pushButton[i]->isCheckable();
        p_pushButton[i]->setChecked(false);
    }

    //Remove auto default focus from all buttons
    for (int i=0; i<=MM_SYS; i++) {
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

PanelMushra::~PanelMushra()
{
    delete ui;
}

//////////////////////////////////////////////////
///
/// Score sliders, values and buttons
///
//////////////////////////////////////////////////
//Slider change is written to value label
void PanelMushra::on_verticalSlider_A_valueChanged(int value)
{
    ui->label_SliderValue_A->setText(QString::number(value));
}
void PanelMushra::on_verticalSlider_B_valueChanged(int value)
{
    ui->label_SliderValue_B->setText(QString::number(value));
}
void PanelMushra::on_verticalSlider_C_valueChanged(int value)
{
    ui->label_SliderValue_C->setText(QString::number(value));
}
void PanelMushra::on_verticalSlider_D_valueChanged(int value)
{
    ui->label_SliderValue_D->setText(QString::number(value));
}
void PanelMushra::on_verticalSlider_E_valueChanged(int value)
{
    ui->label_SliderValue_E->setText(QString::number(value));
}
void PanelMushra::on_verticalSlider_F_valueChanged(int value)
{
    ui->label_SliderValue_F->setText(QString::number(value));
}
void PanelMushra::on_verticalSlider_G_valueChanged(int value)
{
    ui->label_SliderValue_G->setText(QString::number(value));
}
void PanelMushra::on_verticalSlider_H_valueChanged(int value)
{
    ui->label_SliderValue_H->setText(QString::number(value));
}
void PanelMushra::on_verticalSlider_I_valueChanged(int value)
{
    ui->label_SliderValue_I->setText(QString::number(value));
}
void PanelMushra::on_verticalSlider_J_valueChanged(int value)
{
    ui->label_SliderValue_J->setText(QString::number(value));
}
void PanelMushra::on_verticalSlider_K_valueChanged(int value)
{
    ui->label_SliderValue_K->setText(QString::number(value));
}
void PanelMushra::on_verticalSlider_L_valueChanged(int value)
{
    ui->label_SliderValue_L->setText(QString::number(value));
}

//Signal pushbuttons
void PanelMushra::on_pushButton_REF_clicked()
{
    QPushButton *pb = (QPushButton *)sender();
    radio_button(pb);
    //send to main program
    //next_sys starts at 0 for REF, 1 for A, etc.
    play->paf->next_sys = 0; //REF is always 0
}
void PanelMushra::on_pushButton_A_clicked()
{
    QPushButton *pb = (QPushButton *)sender();
    radio_button(pb);
    //send to main program
    //in next statement session->session_info.has_open_ref has value 1 (REF) or 0 (No REF)
    play->paf->next_sys = session->session_info.sys_map[0] + session->session_info.has_open_ref;
}
void PanelMushra::on_pushButton_B_clicked()
{
    QPushButton *pb = (QPushButton *)sender();
    radio_button(pb);
    //send to main program
    play->paf->next_sys = session->session_info.sys_map[1] + session->session_info.has_open_ref;
}
void PanelMushra::on_pushButton_C_clicked()
{
    QPushButton *pb = (QPushButton *)sender();
    radio_button(pb);
    //send to main program
    play->paf->next_sys = session->session_info.sys_map[2] + session->session_info.has_open_ref;
}
void PanelMushra::on_pushButton_D_clicked()
{
    QPushButton *pb = (QPushButton *)sender();
    radio_button(pb);
    //send to main program
    play->paf->next_sys = session->session_info.sys_map[3] + session->session_info.has_open_ref;
}
void PanelMushra::on_pushButton_E_clicked()
{
    QPushButton *pb = (QPushButton *)sender();
    radio_button(pb);
    //send to main program
    play->paf->next_sys = session->session_info.sys_map[4] + session->session_info.has_open_ref;
}
void PanelMushra::on_pushButton_F_clicked()
{
    QPushButton *pb = (QPushButton *)sender();
    radio_button(pb);
    //send to main program
    play->paf->next_sys = session->session_info.sys_map[5] + session->session_info.has_open_ref;
}
void PanelMushra::on_pushButton_G_clicked()
{
    QPushButton *pb = (QPushButton *)sender();
    radio_button(pb);
    //send to main program
    play->paf->next_sys = session->session_info.sys_map[6] + session->session_info.has_open_ref;
}
void PanelMushra::on_pushButton_H_clicked()
{
    QPushButton *pb = (QPushButton *)sender();
    radio_button(pb);
    //send to main program
    play->paf->next_sys = session->session_info.sys_map[7] + session->session_info.has_open_ref;
}
void PanelMushra::on_pushButton_I_clicked()
{
    QPushButton *pb = (QPushButton *)sender();
    radio_button(pb);
    //send to main program
    play->paf->next_sys = session->session_info.sys_map[8] + session->session_info.has_open_ref;
}
void PanelMushra::on_pushButton_J_clicked()
{
    QPushButton *pb = (QPushButton *)sender();
    radio_button(pb);
    //send to main program
    play->paf->next_sys = session->session_info.sys_map[9] + session->session_info.has_open_ref;
}
void PanelMushra::on_pushButton_K_clicked()
{
    QPushButton *pb = (QPushButton *)sender();
    radio_button(pb);
    //send to main program
    play->paf->next_sys = session->session_info.sys_map[10] + session->session_info.has_open_ref;
}
void PanelMushra::on_pushButton_L_clicked()
{
    QPushButton *pb = (QPushButton *)sender();
    radio_button(pb);
    //send to main program
    play->paf->next_sys = session->session_info.sys_map[11] + session->session_info.has_open_ref;
}

#define TESTPANEL PanelMushra
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



