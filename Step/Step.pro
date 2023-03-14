QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    anglelabel.cpp \
    common_panel_controls.cpp \
    initRunNextTrial.cpp \
    main.cpp \
    panelab5.cpp \
    panelab7.cpp \
    paneladmin.cpp \
    panelbs1116.cpp \
    panelmushra.cpp \
    qt_interface.cpp \
    qt_message.cpp

HEADERS += \
    ../port/allTestIncludes.h \
    ../port/cmdline.h \
    ../port/config.h \
    ../port/defineValues.h \
    ../port/license.h \
    ../port/message.h \
    ../port/paUtils.h \
    ../port/play.h \
    ../port/portControls.h \
    ../port/rngs.h \
    ../port/session.h \
    ../port/step_cmdline.h \
    ../port/sysFiles.h \
    ../port/testAB.h \
    ../port/testABX.h \
    ../port/testBS1116.h \
    ../port/testBase.h \
    ../port/testMOS.h \
    ../port/testMushra.h \
    ../port/testTrain.h \
    ../port/transBuff.h \
    ../port/transition.h \
    ../port/trialAudioFiles.h \
    ../port/tty_interface.h \
    ../port/utils.h \
    ../port/video.h \
    ../port/wav.h \
    anglelabel.h \
    main.h \
    panelab5.h \
    panelab7.h \
    paneladmin.h \
    panelbs1116.h \
    panelmushra.h \
    qt_interface.h \
    qt_message.h

FORMS += \
    panelab5.ui \
    panelab7.ui \
    paneladmin.ui \
    panelbs1116.ui \
    panelmushra.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

DISTFILES += \
    icons8-music-1024.icns

#
# My additions to this file:
#
# To use
# Openssl
# PortAudio
# step_port
INCLUDEPATH += $$PWD/../Openssl/include
DEPENDPATH += $$PWD/../Openssl/include
INCLUDEPATH += $$PWD/../../../../../../../usr/local/include
DEPENDPATH += $$PWD/../../../../../../../usr/local/include
INCLUDEPATH += $$PWD/../port
DEPENDPATH += $$PWD/../port

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../Openssl/lib/release/ -lcrypto
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../Openssl/lib/debug/ -lcrypto
else:unix: LIBS += -L$$PWD/../Openssl/lib/ -lcrypto

#not sure about the win32 paths
win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../../../../../../usr/local/Cellar/portaudio/19.7.0/lib/release/ -lportaudio
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../../../../../usr/local/Cellar/portaudio/19.7.0/lib/debug/ -lportaudio
else:unix: LIBS += -L$$PWD/../../../../../../../usr/local/lib/ -lportaudio

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../port/ -lstep_port
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../port/ -lstep_port
else:unix: LIBS += -L$$PWD/../port/ -lstep_port
