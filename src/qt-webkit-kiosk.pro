#-------------------------------------------------
#
# Project created by QtCreator 2010-07-22T01:54:24
#
#-------------------------------------------------


QT       += core gui network webkit multimedia widgets webkitwidgets printsupport

contains(QT_VERSION, ^4\\.[0-9]\\..*) {
    message("Cannot build Qt Creator with Qt version $${QT_VERSION}.")
    error("Use at least Qt 5.0.")
}

CONFIG += console
TARGET = qt-webkit-kiosk
TEMPLATE = app
VERSION = 1.05.11

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
    cachingnm.cpp

HEADERS  += mainwindow.h \
    webview.h \
    anyoption.h \
    config.h \
    qplayer.h \
    fakewebview.h \
    cachingnm.h

# INSTALL

target.path = $${PREFIX}/bin

icon.files = ../resources/$${TARGET}.png
icon.path = $${PREFIX}/share/icons

INSTALLS += target icon
