#include "config.h"

#include <QtGui>
#include <QtWebKit>
#include "webview.h"
#include <QNetworkReply>
#include <QtDebug>
#include <QSslError>
#include "mainwindow.h"


WebView::WebView(QWidget* parent): QWebView(parent)
{
    player = NULL;
    loader = NULL;
    connect(page()->networkAccessManager(),
            SIGNAL(sslErrors(QNetworkReply*, const QList<QSslError> & )),
            this,
            SLOT(handleSslErrors(QNetworkReply*, const QList<QSslError> & )));

    connect(page(), SIGNAL(linkClicked(QUrl)), SLOT(linkClicked(const QUrl &)));
}

void WebView::handleSslErrors(QNetworkReply* reply, const QList<QSslError> &errors)
{
    qDebug() << "handleSslErrors: ";
    foreach (QSslError e, errors)
    {
        qDebug() << "ssl error: " << e;
    }

    if (mainSettings->value("browser/ignore_ssl_errors").toBool()) {
        reply->ignoreSslErrors();
    } else {
        reply->abort();
    }
}

void WebView::setSettings(QSettings *settings)
{
    mainSettings = settings;
}

void WebView::mousePressEvent(QMouseEvent *event)
{
    QWebView::mousePressEvent(event);
    if (event->button() == Qt::LeftButton) {
        qDebug() << "Window Clicked!";
        playSound("event-sounds/window-clicked");
    }
}

void WebView::linkClicked(const QUrl &url)
{
    qDebug() << "Link Clicked!" << url.toString();
    playSound("event-sounds/link-clicked");
}

void WebView::urlChanged(const QUrl &url)
{
    qDebug() << "url Changed!" << url.toString();

    this->load(url);
    qDebug() << "-- load url";

    loader->close();
    loader = NULL;
    qDebug() << "-- close";
}

QPlayer *WebView::getPlayer()
{
    if (mainSettings->value("event-sounds/enable").toBool()) {
        if (player == NULL) {
            player = new QPlayer();
        }
    }
    return player;
}

void WebView::playSound(QString soundSetting)
{
    if (getPlayer() != NULL) {
        QString sound = mainSettings->value(soundSetting).toString();
        qDebug() << "Play sound: " << sound;
        getPlayer()->play(sound);
    }
}

QWebView *WebView::createWindow(QWebPage::WebWindowType type)
{
    Q_UNUSED(type);

    if (loader == NULL) {
        loader = new FakeWebView(this);
        QWebPage *newWeb = new QWebPage(loader);
        loader->setAttribute(Qt::WA_DeleteOnClose, true);
        loader->setPage(newWeb);

        connect(loader, SIGNAL(urlChanged(QUrl)), SLOT(urlChanged(QUrl)));
    }

    return loader;
}
