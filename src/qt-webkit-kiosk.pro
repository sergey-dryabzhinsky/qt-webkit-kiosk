#-------------------------------------------------
#
# Project created by QtCreator 2010-07-22T01:54:24
#
#-------------------------------------------------


QT       += core gui network webkit

contains(QT_VERSION, ^5\\.[0-9]\\..*) {
QT       += multimedia widgets webkitwidgets printsupport
DEFINES  += QT5
}


CONFIG += console
TARGET = qt-webkit-kiosk
TEMPLATE = app
VERSION = 1.99.0

CONFIG(debug, debug|release) {
# here comes debug specific statements
    message(Debug build)
} else {
# here comes release specific statements
    CONFIG -= debug
    DEFINES += QT_NO_DEBUG_OUTPUT
}

message(Qt version: $$[QT_VERSION])
message(Qt is installed in $$[QT_INSTALL_PREFIX])
message(Settings:)
message( PREFIX: $${PREFIX})
message( TARGET: $${TARGET})
message( VERSION: $${VERSION})

DEFINES += VERSION=\\\"$${VERSION}\\\"

!win32 {
    ICON = $${PREFIX}/share/icons/$${TARGET}.png
    DEFINES += RESOURCES=\\\"$${PREFIX}/share/$${TARGET}/\\\"
    DEFINES += ICON=\\\"$${ICON}\\\"
}
win32 {
    ICON = ./$${TARGET}.png
    DEFINES += RESOURCES=\\\"./\\\"
    DEFINES += ICON=\\\"$${ICON}\\\"
}

packagesExist(phonon) {
    message('Phonon framework found. Using Phonon-player.')
    DEFINES += PLAYER_PHONON
    QT += phonon
    SOURCES += player/phonon.cpp
    HEADERS += player/phonon.h
} else {
    warning('No phonon framework found! Using NULL-player.')
    DEFINES += PLAYER_NULL
    SOURCES += player/null.cpp
    HEADERS += player/null.h
}

packagesExist(QtTest) {
    message('Test framework found.')
    DEFINES += USE_TESTLIB
    QT += testlib
} else {
    warning('No QtTest framework found! Will not do hacks with window...')
}


SOURCES += main.cpp\
    mainwindow.cpp \
    webview.cpp \
    anyoption.cpp \
    fakewebview.cpp \
    cachingnm.cpp \
    unixsignals.cpp \
    socketpair.cpp

HEADERS  += mainwindow.h \
    webview.h \
    anyoption.h \
    config.h \
    qplayer.h \
    fakewebview.h \
    cachingnm.h \
    unixsignals.h \
    socketpair.h

# INSTALL

target.path = $${PREFIX}/bin

icon.files = ../resources/$${TARGET}.png
icon.path = $${PREFIX}/share/icons

INSTALLS += target icon
