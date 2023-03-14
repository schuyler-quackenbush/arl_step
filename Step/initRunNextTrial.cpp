//////////////////////////////////////////////////
///
/// Init, Run, Next Trial functions
///
//////////////////////////////////////////////////
void TESTPANEL::initTrial()
{
    qDebug() << "enter initTrial()";

    //free transition object storage
    trans_buff->free_trans_buff();

    //set up driver for playing
    if ( play->initPlay(session, trial_audio_files, config->transition_mode) == 0 ) {
        showError((char *)"Cannot initialize audio driver");
        exit(1);
    }
    qDebug() << "exit initTrial()";
}

void TESTPANEL::startTrial()
{
    qDebug() << "enter startTrial()";
    char msg[LINE_LEN];

    SessionInfo *psi = &session->session_info;

    //randomize conditions on panel buttons
    if (config->random_conditions) {
        session->permute(psi->sys_map, psi->num_systems);
    }
    else {
        session->reset_map(psi->sys_map, psi->num_systems);
    }

    snprintf(msg, LINE_LEN, "Trial %d of %d: %s", psi->tnum+1, psi->num_trials, psi->trial[psi->trial_map[psi->tnum]].sig_lab);
    //set title for trial
    ui->label_TrialNum->setText(msg);

    //free audio file buffers
    trial_audio_files->freeAudioFiles();

    //open audio files and create audio file buffers
    if ( trial_audio_files->openAudioFiles(psi, config, psi->trial_map[psi->tnum]) == 0) {
        showError((char *)"Cannot open audio files");
        exit(1);
    }

    //Get duration of this trial signal
    play->audioStatus( &at, &dur);
    snprintf(msg, LINE_LEN, "%6.2f", dur);
    ui->label_StopTime->setText(msg);

    initPP();

    //MORE

    //VIDEO
    qDebug() << "exit startTrial()";
}

int TESTPANEL::nextTrial()
{
    // 0: return (to current trial)
    // 1: continue to next trial
    // 2: test complete, so close test panel and return to admin panel

    SessionInfo *psi = &session->session_info;

    qDebug() << "enter nextTrial()";

    //stop play
    play->stopPlay();

    //read scores off of slider value boxes to scores[] in testBase.h
    QString value;
    switch (session->session_info.test_id) {
    case T_MUSHRA:
    case T_BS1116:
        for (int i=1; i<=session->session_info.num_systems; i++) {
            //REF is phantom vertical slider index 0
            value = p_sliderValue[i]->text();
            this_test->scores[i] = value.toDouble();
        }
        break;
    case T_AB5:
    case T_AB7:
        value = p_sliderValue[1]->text();
        this_test->scores[1] = value.toDouble();
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
        break;
    }

    //save scores to TrialScores
    this_test->getScores();

    //save scores so far
    this_test->writePartialScores();

    //next trial
    psi->tnum++;

    //set sliders to initial value, deselect sliders and uncheck buttons
    switch (session->session_info.test_id) {
    case T_MUSHRA:
    case T_BS1116:
        for (int i=1; i<=session->session_info.num_systems; i++) {
            p_verticalSlider[i]->setValue(100);
            p_verticalSlider[i]->setEnabled(false);
            p_pushButton[i]->setChecked(false);
        }
        break;
    case T_AB5:
    case T_AB7:
        p_verticalSlider[1]->setValue(0);
        p_verticalSlider[1]->setEnabled(true);
        p_pushButton[1]->setChecked(true);
        p_pushButton[2]->setChecked(false);
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

    //select initial button to play
    if (session->session_info.has_open_ref == 1) {
        button_playing = 0; //Select REF
        p_pushButton[0]->setChecked(true);
    }
    else {
        button_playing = 1; //Select A
        p_pushButton[1]->setChecked(true);
        p_verticalSlider[1]->setEnabled(true);
    }

    if (psi->tnum < psi->num_trials) {
        qDebug() << "exit 1 nextTrial()";
        return 1;
    }
    else {
        qDebug() << "test complete";
        //test complete
        //stop and write scores
        play->stopPlay();
        this_test->complete_test = 1;
        this_test->writeScores();
        //write config files
        config->writeConfig();
        config->writeGlobalConfig();
        qDebug() << "exit 2 nextTrial()";
        return 2;
    }

    return 0; //this is never executed
}
