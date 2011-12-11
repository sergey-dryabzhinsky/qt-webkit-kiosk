#-------------------------------------------------
#
# Project created by QtCreator 2010-07-22T01:54:24
#
#-------------------------------------------------


QT       += core gui network webkit phonon

# CONFIG += debug
TARGET = qt-webkit-kiosk
TEMPLATE = app

message(Qt version: $$[QT_VERSION])
message(Qt is installed in $$[QT_INSTALL_PREFIX])
message(Settings:)
message( PREFIX: $${PREFIX})
message( TARGET: $${TARGET})

ICON = $${PREFIX}/share/icons/$${TARGET}.png

DEFINES += RESOURCES=\\\"$${PREFIX}/share/$${TARGET}\\\"
DEFINES += ICON=\\\"$${ICON}\\\"

SOURCES += main.cpp\
        mainwindow.cpp \
    webview.cpp \
    anyoption.cpp

HEADERS  += mainwindow.h \
    webview.h \
    anyoption.h \
    config.h

OTHER_FILES += \
    qt-webkit-kiosk.ini


# INSTALL

target.path = $${PREFIX}/bin

icon.files = ../resources/$${TARGET}.png
icon.path = $${PREFIX}/share/icons

INSTALLS += target icon
