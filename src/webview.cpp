#include "config.h"

#include <QtGui>
#include <QtWebKit>
#include "webview.h"

WebView::WebView(QWidget* parent)
{
    audioOutput = new Phonon::AudioOutput(Phonon::MusicCategory, this);
    mediaObject = new Phonon::MediaObject(this);

    mediaObject->setTickInterval(1000);

    Phonon::createPath(mediaObject, audioOutput);

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
            qDebug() << "Window Clicked! Play sound: " << sound;
            if (sound.length()) {

                mediaObject->stop();
                mediaObject->clearQueue();

                Phonon::MediaSource source(sound);

                mediaObject->setCurrentSource(source);
                mediaObject->play();
            }
        }
    }
}

void WebView::linkClicked(const QUrl &url)
{
    QWebView::linkClicked(url);
    if (mainSettings->value("event-sounds/enable").toBool()) {
        QString sound = mainSettings->value("event-sounds/link-clicked").toString();
        qDebug() << "Link Clicked! Play sound: " << sound;
        if (sound.length()) {

            mediaObject->stop();
            mediaObject->clearQueue();

            Phonon::MediaSource source(sound);

            mediaObject->setCurrentSource(source);
            mediaObject->play();
        }
    }
}
