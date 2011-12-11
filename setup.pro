#-------------------------------------------------
#
# Project created by QtCreator 2010-07-22T01:54:24
#
#-------------------------------------------------

TEMPLATE = subdirs
CONFIG += ordered

SUBDIRS += src/qt-webkit-kiosk.pro

# INSTALL

target.path = $${PREFIX}/bin

desktop.files = resources/qt-webkit-kiosk.desktop
desktop.path = $${PREFIX}/share/applications

config.files = resources/qt-webkit-kiosk.ini
config.path = $${PREFIX}/share/qt-webkit-kiosk

sound.files = resources/window-clicked.wav
sound.path = $${PREFIX}/share/qt-webkit-kiosk

html.files = resources/default.html
html.path = $${PREFIX}/share/qt-webkit-kiosk

doc.files = doc/lgpl.html
doc.path = $${PREFIX}/share/doc/qt-webkit-kiosk

INSTALLS += target desktop config sound html doc
