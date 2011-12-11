#include "config.h"

#include <QtGui>
#include <QtWebKit>
#include "webview.h"

WebView::WebView(QWidget* parent)
{
    this->setParent(parent);
}

void WebView::setSettings(QSettings *settings)
{
    this->mainSettings = settings;
}

void WebView::mousePressEvent(QMouseEvent *event)
{
    QWebView::mousePressEvent(event);
    if (event->button() == Qt::LeftButton) {
        this->setFocus();
        if (mainSettings->value("event-sounds/enable").toBool()) {
            QString sound = mainSettings->value("event-sounds/window-clicked").toString();
            qDebug() << "Clicked! Play sound: " << sound;
            if (sound.length()) {
                qDebug() << "Sound available? " << QSound::isAvailable();
                QSound::play(sound);
            }
        }
    }
}
