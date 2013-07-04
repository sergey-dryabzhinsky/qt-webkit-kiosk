#include "config.h"

#include <QtGui>
#include <QtWebKit>
#include "webview.h"

WebView::WebView(QWidget* parent)
{
    player = new QPlayer();

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
            player->play(sound);
        }
    }
}

void WebView::linkClicked(const QUrl &url)
{
    QWebView::linkClicked(url);
    if (mainSettings->value("event-sounds/enable").toBool()) {
        QString sound = mainSettings->value("event-sounds/link-clicked").toString();
        qDebug() << "Link Clicked! Play sound: " << sound;
        player->play(sound);
    }
}
