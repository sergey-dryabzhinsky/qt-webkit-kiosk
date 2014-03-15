#-------------------------------------------------
#
# Project created by QtCreator 2010-07-22T01:54:24
#
#-------------------------------------------------


QT       += core gui network webkit multimedia widgets webkitwidgets printsupport

CONFIG += console
TARGET = qt-webkit-kiosk
TEMPLATE = app
VERSION = 1.05.14

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

SOURCES += main.cpp\
    mainwindow.cpp \
    webview.cpp \
    anyoption.cpp \
    qplayer.cpp \
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
