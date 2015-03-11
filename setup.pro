#-------------------------------------------------
#
# Project created by QtCreator 2010-07-22T01:54:24
#
#-------------------------------------------------

TEMPLATE = subdirs
CONFIG += ordered warn_on

CONFIG(debug, debug|release) {
# here comes debug specific statements
    message(Debug build)
} else {
# here comes release specific statements
    message(Release build)
    CONFIG -= debug
    DEFINES += QT_NO_DEBUG_OUTPUT
}
message(testing)
SUBDIRS += src/qt-webengine-kiosk.pro

# INSTALL

target.path = $${PREFIX}/bin

desktop.files = resources/qt-webengine-kiosk.desktop
desktop.path = $${PREFIX}/share/applications

config.files = resources/qt-webengine-kiosk.ini
config.path = $${PREFIX}/share/qt-webengine-kiosk

sound.files = \
    resources/window-clicked.wav resources/window-clicked.ogg resources/window-clicked.mp3 \
    resources/link-clicked.wav resources/link-clicked.ogg resources/link-clicked.mp3
sound.path = $${PREFIX}/share/qt-webengine-kiosk

html.files = resources/default.html
html.path = $${PREFIX}/share/qt-webengine-kiosk

doc.files = doc/lgpl.html doc/README.md
doc.path = $${PREFIX}/share/doc/qt-webengine-kiosk

INSTALLS += target desktop config sound html doc
