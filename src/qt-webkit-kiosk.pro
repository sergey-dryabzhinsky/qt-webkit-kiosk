#-------------------------------------------------
#
# Project created by QtCreator 2010-07-22T01:54:24
#
#-------------------------------------------------

QT       += core gui network webkit

TARGET = qt-webkit-kiosk
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    webview.cpp \
    anyoption.cpp

HEADERS  += mainwindow.h \
    webview.h \
    anyoption.h

OTHER_FILES += \
    qt-webkit-kiosk.ini


target.path = $$[install_prefix]/bin

