#-------------------------------------------------
#
# Project created by QtCreator 2010-07-22T01:54:24
#
#-------------------------------------------------

TEMPLATE = subdirs
CONFIG += ordered

SUBDIRS += src/qt-webkit-kiosk.pro

# INSTALL

target.path = $$[install_prefix]/bin

icon.files = qt-webkit-kiosk.png
icon.path = $$[install_prefix]/share/icons

desktop.files = resources/qt-webkit-kiosk.desktop
desktop.path = $$[install_prefix]/share/applications

config.files = resources/qt-webkit-kiosk.ini
config.path = $$[install_prefix]/share/qt-webkit-kiosk

sound.files = resources/window-clicked.wav
sound.path = $$[install_prefix]/share/qt-webkit-kiosk

INSTALLS += target icon desktop config sound
