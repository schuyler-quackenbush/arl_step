#include <QApplication>
#include <QDir>
#include <QDebug>
#include <string.h>
#include "qt_message.h"
#include "paneladmin.h"

#include "main.h"
#include "defineValues.h"
#include "license.h"
#include "config.h"
#include "session.h"
#include "transBuff.h"
#include "play.h"
#include "allTestIncludes.h"
#include "trialAudioFiles.h"
#include "wav.h"

License *license = new License;
Config *config = new Config;
Session *session = new Session;
TrialAudioFiles *trial_audio_files = new TrialAudioFiles;
TransBuff *trans_buff = new TransBuff;
TestBase *this_test = NULL;
Play *play = new Play;

PanelAdmin *w_ap;

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    PanelAdmin w;

    //pointer to PanelAdmin
    w_ap = &w;

    //Get directory of STEP executable
    QString step_app_dir = QCoreApplication::applicationDirPath();
    QByteArray ba = step_app_dir.toLocal8Bit();
    char *step_dir = ba.data();

 #if __APPLE__
    //MACOS: License three directories above executable
    char *end;
    for (int i=0; i<3; i++) {
        end = strrchr(step_dir, '/');
        *end = 0;
    }
 #else
    //Windows 10: License is in same directory
 #endif
    char license_path[LICENSE_LEN];
    strlcpy( license_path, step_dir, LICENSE_LEN);
    strlcat( license_path, PATH_SEP, LICENSE_LEN);
    strlcat( license_path, "license.txt", LICENSE_LEN);
    qDebug() << license_path;

    //Check STEP license
    license->checkLicense( license_path );
    showInformation( license->getTitle(), license->getMessage() );

    //Read global config
    config->readGlobalConfig( step_dir );

    //Show administrator panel
    w.show();
    return a.exec();
}
