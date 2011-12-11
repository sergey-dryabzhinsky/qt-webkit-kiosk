#-------------------------------------------------
#
# Project created by QtCreator 2010-07-22T01:54:24
#
#-------------------------------------------------


message(Qt version: $$[QT_VERSION])
message(Qt is installed in $$[QT_INSTALL_PREFIX])
message(Settings:)
message( PREFIX: $${PREFIX})

QT       += core gui network webkit

TARGET = qt-webkit-kiosk
TEMPLATE = app

DEFINES += SHARE_DIR=\\\"$${PREFIX}/share\\\"

SOURCES += main.cpp\
        mainwindow.cpp \
    webview.cpp \
    anyoption.cpp

HEADERS  += mainwindow.h \
    webview.h \
    anyoption.h

OTHER_FILES += \
    qt-webkit-kiosk.ini


# INSTALL

target.path = $${PREFIX}/bin

INSTALLS += target
