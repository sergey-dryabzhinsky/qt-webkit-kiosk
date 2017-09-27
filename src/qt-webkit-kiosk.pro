#-------------------------------------------------
#
# Project created by QtCreator 2010-07-22T01:54:24
#
#-------------------------------------------------

QT       = core gui network webkit

contains(QT_VERSION, ^5\\.[0-9]+\\..*) {
    QT       += widgets webkitwidgets printsupport
    DEFINES  += QT5
}

CONFIG += console link_pkgconfig
TARGET = qt-webkit-kiosk
TEMPLATE = app
VERSION = 1.99.7

CONFIG(debug, debug|release) {
# here comes debug specific statements
    message(Debug build)
} else {
# here comes release specific statements
    CONFIG -= debug
    DEFINES += QT_NO_DEBUG_OUTPUT
}

DEFINES += VERSION=\\\"$${VERSION}\\\"

PLAYER = NONE

!win32 {
    isEmpty( PREFIX ) {
        PREFIX = /usr/local
    }

    ICON = $${PREFIX}/share/icons/$${TARGET}.png
    DEFINES += RESOURCES=\\\"$${PREFIX}/share/$${TARGET}/\\\"
    DEFINES += ICON=\\\"$${ICON}\\\"

    PKG_TEST=QtTest

contains(QT_VERSION, ^5\\.[0-9]+\\..*) {

    PKG_TEST=Qt5Test

    packagesExist(Qt5Multimedia) {
        message('Multimedia framework found. Using Multimedia-player.')
        DEFINES += PLAYER_MULTIMEDIA
        QT += multimedia
        PLAYER = MULTIMEDIA
        SOURCES += player/multimedia.cpp
        HEADERS += player/multimedia.h
    }

    contains(PLAYER, NONE) {
        # Ubuntu / Debian version?
        packagesExist(phonon4qt5) {
            message('Phonon framework found. Using Phonon-player.')
            DEFINES += PLAYER_PHONON
            QT += phonon4qt5
            PLAYER = PHONON
            SOURCES += player/phonon.cpp
            HEADERS += player/phonon.h
        }
    }
}

    contains(PLAYER, NONE) {
        packagesExist(phonon) {
            message('Phonon framework found. Using Phonon-player.')
            DEFINES += PLAYER_PHONON
            QT += phonon
            PLAYER = PHONON
            SOURCES += player/phonon.cpp
            HEADERS += player/phonon.h
        }
    }

    packagesExist($${PKG_TEST}) {
        message('Test framework found.')
        DEFINES += USE_TESTLIB
        QT += testlib
    } else {
        warning('No QtTest framework found! Will not do hacks with window...')
    }
}
win32 {
    ICON = ./$${TARGET}.png
    DEFINES += RESOURCES=\\\"./\\\"
    DEFINES += ICON=\\\"$${ICON}\\\"

# Windows don't have pkg-config
# So we check generic paths...

contains(QT_VERSION, ^5\\.[0-9]+\\..*) {
    exists($$[QT_INSTALL_PREFIX]\\bin\\Qt*Multimedia*) {
        message('Multimedia framework found. Using Multimedia-player.')
        DEFINES += PLAYER_MULTIMEDIA
        QT += multimedia
        PLAYER = MULTIMEDIA
        SOURCES += player/multimedia.cpp
        HEADERS += player/multimedia.h
    }
}
    contains(PLAYER, NONE) {
        exists($$[QT_INSTALL_PREFIX]\\bin\\phonon*) {
            message('Phonon framework found. Using Phonon-player.')
            DEFINES += PLAYER_PHONON
            QT += phonon
            PLAYER = PHONON
            SOURCES += player/phonon.cpp
            HEADERS += player/phonon.h
        }
    }
    exists($$[QT_INSTALL_PREFIX]\\bin\\Qt*Test*) {
        message('Test framework found.')
        DEFINES += USE_TESTLIB
        QT += testlib
    } else {
        warning('No QtTest framework found! Will not do hacks with window...')
    }
}

message(Qt player: $${PLAYER})

contains(PLAYER, NONE) {
    warning('No multimedia framework found! Using NULL-player.')
    DEFINES += PLAYER_NULL
    SOURCES += player/null.cpp
    HEADERS += player/null.h
}

message(Qt version: $$[QT_VERSION])
message(Qt is installed in $$[QT_INSTALL_PREFIX])
message(Settings:)
message(- QT     : $${QT})
message(- PREFIX : $${PREFIX})
message(- TARGET : $${TARGET})
message(- VERSION: $${VERSION})

SOURCES += main.cpp\
    mainwindow.cpp \
    webview.cpp \
    anyoption.cpp \
    fakewebview.cpp \
    cachingnm.cpp \
    unixsignals.cpp \
    socketpair.cpp \
    persistentcookiejar.cpp \
    qwk_settings.cpp

HEADERS  += mainwindow.h \
    webview.h \
    anyoption.h \
    config.h \
    qplayer.h \
    fakewebview.h \
    cachingnm.h \
    unixsignals.h \
    socketpair.h \
    persistentcookiejar.h \
    qwk_settings.h

# DEBUG
#message(- SOURCES: $${SOURCES})
#message(- HEADERS: $${HEADERS})

# INSTALL

target.path = $${PREFIX}/bin

icon.files = ../resources/$${TARGET}.png
icon.path = $${PREFIX}/share/icons

INSTALLS += target icon
