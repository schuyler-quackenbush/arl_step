//#include "widget.h"
//#include "ui_widget.h"
#include <QCloseEvent>
#include <QMessageBox>

//////////////////////////////////////////////////
///
/// Emulate Radio Buttons
///
//////////////////////////////////////////////////
//Implement signal pushbotton "radio" function
void TESTPANEL::radio_button(QPushButton *pb)
{
    int start_idx;
    switch (session->session_info.test_id) {
        case T_MUSHRA:
        case T_BS1116:
        //REF is 0
        start_idx = session->session_info.has_open_ref ? 0 : 1;
        for (int i=start_idx; i<=session->session_info.num_systems; i++) {
            if (p_pushButton[i] == pb) {
                //this button is clicked
                //check button
                p_pushButton[i]->setChecked(true);
                if (i > 0) {
                    //REF is 0 and has no slider
                    p_verticalSlider[i]->setEnabled(true);
                    p_verticalSlider[i]->setFocus();
                }
                button_playing = i;
                play->paf->playing = 1;
                ui->pushButton_Play->setChecked(true);
                //show status
                show_pl_status();
            }
            else {
                //uncheck other pushbuttons
                p_pushButton[i]->setChecked(false);
                //disable other sliders
                //no slider to disable for REF
                if (i>0 && config->only_score_item_playing) {
                    p_verticalSlider[i]->setEnabled(false);
                }
            }
        }
        break;
        case T_AB5:
        case T_AB7:
            for (int i=1; i<=2; i++) {
                if (p_pushButton[i] == pb) {
                    //check this pushbutton
                    p_pushButton[i]->setChecked(true);
                    button_playing = i - 1; //There is no REF, so A is index 0
                    play->paf->playing = 1;
                    ui->pushButton_Play->setChecked(true);
                    //show status
                    show_pl_status();
                }
                else {
                    //uncheck other pushbutton
                    p_pushButton[i]->setChecked(false);
                }
            }
            break;
        case T_ABX:
            ;
            break;
        case T_MOS:
            ;
            break;
        case T_TRAIN:
            ;
            break;
        default:
            showError((char*)"Test type not supported");
    }
}

//////////////////////////////////////////////////
///
/// Next button
///
//////////////////////////////////////////////////
//Finish this trial and go to next trial
void TESTPANEL::on_pushButton_Next_clicked()
{
    int trial_status;
    extern PanelAdmin *w_ap;

    qDebug() << "enter on_pushButton_Next_clicked()";

    //approve scores
    if (config->approve_score) {
        qDebug() << "ask to approve scores";
        if ( approve_question_box( (char*)"STEP Question\n\nApprove scores and continue?" ) != 1 ) {
            qDebug() << "exit NoApprove on_pushButton_Next_clicked()";
            return;
        }

//        if ( showQuestion( msg ) != 1 ){
//            qDebug() << "exit NoApprove on_pushButton_Next_clicked()";
//            return;
//        }
    }
    qDebug() << "scores approved";

    trial_status = nextTrial();
    switch (trial_status) {
    case 0: //Continue with current trial
        qDebug() << "Continue with current trial";
        return;
    case 1: //Move to next trial
        qDebug() << "Move to next trial";
        break;
    case 2: //Test complete, so close test panel and return to admin panel
        //show admin panel
        qDebug() << "finished test";
        w_ap->show();
        //close test panel
        this->close();
        return;
    default:
        showError((char *)"Unknown trial_status");
        exit(1);
    }

    startTrial();

    qDebug() << "exit on_pushButton_Next_clicked()";
}

//////////////////////////////////////////////////
///
/// Play Loop buttons, Status line
///
//////////////////////////////////////////////////
void TESTPANEL::initPP()
{
    play->paf->playing=0;
    play->paf->loop=0;
    ui->pushButton_Play->setChecked(false);
    ui->pushButton_Loop->setChecked(false);
    ui->horizontalScrollBar_Start->setValue(0);
    ui->horizontalScrollBar_Stop->setValue(100);
}

void TESTPANEL::show_pl_status()
{
    SessionInfo *psi = &session->session_info;
    int trial_idx = psi->trial_map[psi->tnum];
    //map is from 0, but sys_lab is from 1 for systems under test (0 is REF)
    int sys_idx;
    if (session->session_info.has_open_ref) {
        //map is from 0, but button A is index 1
        sys_idx = button_playing == 0 ? 0 : 1 + psi->sys_map[button_playing-1];
    }
    else {
        //no REF so A is index 0
        sys_idx = psi->sys_map[button_playing];
    }
    char *p_sys_lab = psi->trial[trial_idx].system[sys_idx].sys_lab;
    QString status;
    status.append("Status: ");
    status.append(play->paf->playing ? "Playing" : "Paused");
    status.append(" - ");
    status.append(play->paf->loop ? "Looping" : "Once");
    if (config->show_system) {
        status.append(" - ");
        status.append(p_sys_lab);
    }
    ui->label_Status->setText(status);
}

//Play/Pause and Loop/Once buttons
void TESTPANEL::on_pushButton_Play_clicked()
{
    if (play->paf->playing == 0) {
        play->paf->playing = 1;
        ui->pushButton_Play->setChecked(true);
    }
    else {
        play->paf->playing = 0;
        ui->pushButton_Play->setChecked(false);
    }
    show_pl_status();
}

void TESTPANEL::on_pushButton_Loop_clicked()
{
    if (play->paf->loop == 0) {
        play->paf->loop = 1;
        ui->pushButton_Loop->setChecked(true);
    }
    else {
        play->paf->loop = 0;
        ui->pushButton_Loop->setChecked(false);
    }
    show_pl_status();
}

//////////////////////////////////////////////////
///
/// Position Indicator, Start Stop sliders
///
//////////////////////////////////////////////////
//Progress bar position is set in timer function
void TESTPANEL::initPSS(void)
{
    //Create 100ms timer for position bar
    QTimer *timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &TESTPANEL::positionTimer);
    timer->start(100);

    //Set width of time indication boxes
    ui->label_PositionTime->setFixedWidth(40);
    ui->label_StartTime->setFixedWidth(40);
    ui->label_StopTime->setFixedWidth(40);
}

void TESTPANEL::positionTimer()
{
    //set position sliders
    //position slider has length 100
    char msg[LINE_LEN];
    play->audioStatus( &at, &dur);
    //set position slider
    ui->progressBar_Position->setValue(100*at/dur);
    //set label
    snprintf(msg, LINE_LEN, "%6.2f", at);
    ui->label_PositionTime->setText(msg);
}

//Start and Stop slider change are written to value labels
//and reported to main program
void TESTPANEL::on_horizontalScrollBar_Start_valueChanged(int value)
{
    char msg[LINE_LEN];
    int value_start = value;
    int value_stop = ui->horizontalScrollBar_Stop->value();
    if ( value_start + min_zoom < value_stop ) {
        //no conflict with min_zoom window
        snprintf(msg, LINE_LEN, "%6.2f", value_start*dur/100.0);
        ui->label_StartTime->setText(msg);
    }
    else {
        //limit start to insure min_zoom window
        int value_start = ui->horizontalScrollBar_Stop->value() - min_zoom;
        ui->horizontalScrollBar_Start->setValue(value_start);
        snprintf(msg, LINE_LEN, "%6.2f", value_start*dur/100.0);
        ui->label_StartTime->setText(msg);
    }
    //send new position to main program
    double start = value_start*dur/100.0;
    double stop = value_stop*dur/100.0;
    play->audioWindow( start, stop );
}

void TESTPANEL::on_horizontalScrollBar_Stop_valueChanged(int value)
{
    char msg[LINE_LEN];
    int value_start = ui->horizontalScrollBar_Start->value();;
    int value_stop = value;
    if ( value_start + min_zoom < value_stop ) {
        //no conflict with min_zoom window
        snprintf(msg, LINE_LEN, "%6.2f", value_stop*dur/100.0);
        ui->label_StopTime->setText(msg);
    }
    else {
        //limit start to insure min_zoom window
        int value_stop = ui->horizontalScrollBar_Start->value() + min_zoom;
        ui->horizontalScrollBar_Stop->setValue(value_stop);
        snprintf(msg, LINE_LEN, "%6.2f", value_stop*dur/100.0);
        ui->label_StopTime->setText(msg);
    }
    //send new position to main program
    double start = value_start*dur/100.0;
    double stop = value_stop*dur/100.0;
    play->audioWindow( start, stop );
}


//////////////////////////////////////////////////
///
/// Window close button
///
//////////////////////////////////////////////////
void TESTPANEL::closeEvent (QCloseEvent *event)
{
    //show admin panel
    extern PanelAdmin *w_ap;
    qDebug() << "clicked on window close";
    w_ap->show();
    //close test panel
    this->close();
}

int TESTPANEL::approve_question_box( char *msg )
{
    QMessageBox::StandardButton resBtn = QMessageBox::question( this, "widget", (char *)msg,
//        QMessageBox::Cancel | QMessageBox::No | QMessageBox::Yes,
        QMessageBox::No | QMessageBox::Yes,
        QMessageBox::Yes);
//    if (resBtn != QMessageBox::Yes) {
//        event->ignore();
//    } else {
//        event->accept();
//    }
//    int ret = msgBox.exec();
//    switch (ret) {
//        case QMessageBox::Yes:
//            ret_val = 1;
//            break;
//        case QMessageBox::No:
//            ret_val = 0;
//            break;
//    default:
//            ret_val = -1;
//    }
//    return ret_val;

    if (resBtn == QMessageBox::Yes) {
        return 1;
    } else {
        return 0;
    }
 }
