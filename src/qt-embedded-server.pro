#-------------------------------------------------
#
# Project created by Sergey 2014-03-22T07:29:00
#
#-------------------------------------------------


QT       += core gui

CONFIG += console
TARGET = qt-embedded-server
TEMPLATE = app
VERSION = 1.99.0

isEmpty( PREFIX ) {
    PREFIX = /usr/local
}


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

SOURCES += embedded_server.cpp qt-embedded-server.cpp

HEADERS  += config.h embedded_server.h

# INSTALL

target.path = $${PREFIX}/bin

INSTALLS += target
