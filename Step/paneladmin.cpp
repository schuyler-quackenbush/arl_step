#include <QMessageBox>
#include <QInputDialog>
#include <QFileDialog>
#include "main.h"
#include "paneladmin.h"
#include "ui_paneladmin.h"
#include "panelmushra.h"
#include "panelbs1116.h"
#include "panelab5.h"
#include "panelab7.h"
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

PanelAdmin::PanelAdmin(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::PanelAdmin)
{
    ui->setupUi(this);
}

PanelAdmin::~PanelAdmin()
{
    delete ui;
}

//////////////////////////////////////////////////
///
/// Browse button
///
//////////////////////////////////////////////////
void PanelAdmin::on_pushButton_Browse_clicked()
{
    QByteArray ba;
    char tmp[LINE_LEN];

    //set defaultSessionDir to previous session directory
    defaultSessionDir = config->session_dir;

    sessionFilePathname =  QFileDialog::getOpenFileName(
          this,
          "Open Document",
          defaultSessionDir,
          "STEP Session Files (*.asi)");
    ui->lineEdit_SessionFile->setText(sessionFilePathname);

    if ( sessionFilePathname == "" ) {
        showError((char *)"Please browse for session file");
        return;
    }
    ba = sessionFilePathname.toLocal8Bit();
    char *session_file_pathname = ba.data();
    process_session( session_file_pathname );

    get_session_options();
    ba = testMethodology.toLocal8Bit();
    snprintf(tmp, LINE_LEN, "Test Methodology: %s", ba.data());
    ui->label_TestMethodology->setText(tmp);
    ui->label_SessionInfo->setText(sessionOptions);
}

int PanelAdmin::process_session(char *session_file)
{
    qDebug() << session_file;
    PaDeviceInfo deviceInfo[MAX_DEV];
    int numDevices, defaultOutputDevice;

    /* initialize session */
    session->initSession(license->validLicense(), session_file, config);
    /* initialize config */
    config->initConfig(license->validLicense(), CONFIG_FILE);
    /* change to session file directory */
    if ( !QDir::setCurrent( config->session_dir ) ) {
        showError( (char*)"Cannot change to session directory\n" );
        return 0;
    }
    /* read config file at session file location */
    config->readConfig();
    /* get and set default output audio device */
    pa_get_audio_dev(deviceInfo, &numDevices, &defaultOutputDevice);
    config->setConfig(defaultOutputDevice, (char *)deviceInfo[defaultOutputDevice].name);
    /* open session file */
    session->openSession( );
    return 1;
}

void PanelAdmin::get_session_options()
{
    //set test methodology
    testMethodology = session->session_info.methodology;
    //clear string
    sessionOptions = QString();;
    sessionOptions.append("Audio Device:  ");
    sessionOptions.append(config->dev_name);
    sessionOptions.append("\n\n");
    sessionOptions.append("Transition: ");
    switch (session->session_info.trans_mode) {
        case ID_TRANS_CF:   sessionOptions.append(TRANS_CF); break;
        case ID_TRANS_05MS: sessionOptions.append(TRANS_05MS); break;
        case ID_TRANS_10MS: sessionOptions.append(TRANS_10MS); break;
        case ID_TRANS_20MS: sessionOptions.append(TRANS_20MS); break;
    }
    sessionOptions.append("\n");
    sessionOptions.append(session->session_info.has_open_ref ? "Open Reference is present\n" : "");
    sessionOptions.append(config->random_trials ? "Randomize Trials\n" : "");
    sessionOptions.append(config->random_conditions ? "Randomize Conditions\n" : "");
    sessionOptions.append(config->only_score_item_playing ? "Only Score Item Playing\n" : "");
    sessionOptions.append(config->check_score ? "Check Scores\n" : "");
    sessionOptions.append(config->approve_score ? "Approve Scores\n" : "");
    sessionOptions.append(config->play_mono_as_stereo ? "Play Mono as Stereo\n" : "");
    sessionOptions.append(config->show_system ? "Show System Name\n" : "");
}

//////////////////////////////////////////////////
///
/// Start button
///
//////////////////////////////////////////////////
void PanelAdmin::on_pushButton_Start_clicked()
{
    QByteArray ba;

    if ( session->session_info.test_id == 0 ) {
        showError((char *)"Please browse for a session file");
        return;
    }
    if ( (labID = ui->lineEdit_LabID->text()) == "" ) {
        showError((char *)"Please enter Lab ID");
        return;
    }
    if ( (listenerID = ui->lineEdit_LisID->text()) == "" ) {
        showError((char *)"Please enter User ID");
        return;
    }

    //save labID and listenerID
    ba = labID.toLocal8Bit();
    strlcpy( config->lab_id, ba.data(), LAB_LEN );
    ba = listenerID.toLocal8Bit();
    strlcpy( config->listener_id, ba.data(), LAB_LEN );

    //start session
    if ( !startSession() ) {
        showError((char *)"Cannot start session");
        exit(1);
    }

    //hide admin panel
    this->hide();

    //select and show test panel
    switch (session->session_info.test_id) {
        case T_MUSHRA:
            PanelMushra *pt1;
            pt1 = new PanelMushra(this);
            pt1->show();
            break;
        case T_BS1116:
            PanelBS1116 *pt2;
            pt2 = new PanelBS1116(this);
            pt2->show();
            break;
        case T_AB5:
            PanelAB5 *pt3;
            pt3 = new PanelAB5(this);
            pt3->show();
            break;
        case T_AB7:
            PanelAB7 *pt4;
            pt4 = new PanelAB7(this);
            pt4->show();
            break;
        case T_ABX:
        case T_MOS:
            char msg[LINE_LEN];
            snprintf(msg, LINE_LEN, "Test type %s not yet supported\nPlease browse for another test type\n",
                session->session_info.methodology);
            showError((char*)"Test type not supported");
            break;
        default:
            showError((char*)"Test type not supported");
    }
}

int PanelAdmin::startSession(void)
{
    SessionInfo *psi = &session->session_info;

    //initialize test
    this_test->initTest( psi->valid_license, session, config);
    //randomize trials
    if (config->random_trials) {
        session->permute(psi->trial_map, psi->num_trials);
    }
    else {
        session->reset_map(psi->trial_map, psi->num_trials);
    }
    //reset trial number
    psi->tnum = 0;
    //check if score file exists
    if ( !this_test->checkScoreFile() ) {
        return 0;
    }
    return 1;
}

//////////////////////////////////////////////////
///
/// Quit button
///
//////////////////////////////////////////////////
void PanelAdmin::on_pushButton_Quit_clicked()
{
    SessionInfo *psi = &session->session_info;
    qDebug() << "Quit Button Pressed";

    if (play->paf != NULL)
        play->stopPlay();
    else
        exit(0); //never intialized play, so just quit

    if (psi->tnum < psi->num_trials) {
        if ( showQuestion((char *)"The session is not complete. Exit anyway?")  == 1) {
            this_test->complete_test = 0;
            this_test->writePartialScores();
        }
        else {
            return;
        }
    }
    else {
        this_test->complete_test = 1;
        this_test->writeScores();
    }

    //write config files
    config->writeConfig();
    config->writeGlobalConfig();
    //end session
    session->endSession();

    exit(0);
}

