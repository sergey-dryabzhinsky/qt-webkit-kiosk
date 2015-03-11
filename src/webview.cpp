#include "config.h"

#include <QtDebug>
#include <QtGui>
#include <QtWebKit>
#include <QtWebEngine>
#include "webview.h"
#include <signal.h>
#include "unixsignals.h"

#include <QNetworkReply>
#include <QSslError>

#include "mainwindow.h"

WebView::WebView(QWidget* parent): QWebEngineView(parent)
{
    player = NULL;
    loader = NULL;
    pageIcon = QIcon();
}

/**
 * Call after setPage
 * @brief WebView::initSignals
 */
void WebView::initSignals()
{

    connect(page(),
            SIGNAL(windowCloseRequested()),
            this,
            SLOT(handleWindowCloseRequested()));

    connect(page(),
            SIGNAL(printRequested(QWebEnginePage*)),
            this,
            SLOT(handlePrintRequested(QWebEnginePage*)));

    connect(page(),
            SIGNAL(iconUrlChanged(QUrl)),
            this,
            SLOT(onIconUrlChanged(QUrl)));
}

void WebView::setPage(QWebEnginePage *page)
{
    QWebEngineView::setPage(page);
    initSignals();
}


void WebView::setSettings(QSettings *settings)
{
    mainSettings = settings;

    if (mainSettings->value("printing/enable").toBool()) {
        if (!mainSettings->value("printing/show-printer-dialog").toBool()) {
            if (!printer) {
                printer = new QPrinter();
            }
            printer->setPrinterName(mainSettings->value("printing/printer").toString());
            printer->setPageMargins(
                mainSettings->value("printing/page_margin_left").toReal(),
                mainSettings->value("printing/page_margin_top").toReal(),
                mainSettings->value("printing/page_margin_right").toReal(),
                mainSettings->value("printing/page_margin_bottom").toReal(),
                QPrinter::Millimeter
            );
        }
    }

}

void WebView::loadHomepage()
{
    loadCustomPage(mainSettings->value("browser/homepage").toString());
}

void WebView::loadCustomPage(QString uri)
{
    QFileInfo finfo = QFileInfo();
    finfo.setFile(uri);

    qDebug() << "Page: check local file = " <<
                uri <<
                ", absolute path = " <<
                finfo.absoluteFilePath() <<
                ", local uri = " <<
                QUrl::fromLocalFile(
                    uri
                ).toString();

    if (finfo.isFile()) {
        qDebug() << "Page: Local FILE";
        this->stop();
        this->load(QUrl::fromLocalFile(
            finfo.absoluteFilePath()
        ));
    } else {
        qDebug() << "Page: Remote URI";
        this->stop();
        this->load(QUrl(uri));
    }
}

QIcon WebView::icon() {
    if (!pageIcon.isNull()) {
        return pageIcon;
    }
    return static_cast<MainWindow*>(this->parent())->windowIcon();
}

void WebView::onIconUrlChanged(const QUrl &url) {
    qDebug() << "icon changed";
    QNetworkRequest iconRequest(url);
    iconRequest.setSslConfiguration(QSslConfiguration::defaultConfiguration());
    QNetworkAccessManager* nam = static_cast<MainWindow*>(this->parent())->nam;
    pageIconReply = nam->get(iconRequest);
    pageIconReply->setParent(this);

    connect(pageIconReply, SIGNAL(finished()), this, SLOT(onIconLoaded()));

}

void WebView::onIconLoaded() {
    pageIcon = QIcon();
    if (pageIconReply) {
        QByteArray data = pageIconReply->readAll();
        QPixmap pixmap;
        pixmap.loadFromData(data);
        pageIcon.addPixmap(pixmap);
        pageIconReply->deleteLater();
        pageIconReply = 0;
    }
    emit iconChanged();
}


void WebView::handleWindowCloseRequested()
{
    qDebug() << "Handle windowCloseRequested:" << mainSettings->value("browser/show_homepage_on_window_close").toString();
    if (mainSettings->value("browser/show_homepage_on_window_close").toBool()) {
        qDebug() << "-- load homepage";
        loadHomepage();
    } else {
        qDebug() << "-- exit application";
        UnixSignals::signalHandler(SIGINT);
    }
}

void WebView::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        qDebug() << "Window Clicked!";
        playSound("event-sounds/window-clicked");
    }
    QWebEngineView::mousePressEvent(event);
}

void WebView::handleUrlChanged(const QUrl &url)
{
    if (this->url().toString() != url.toString()) {
        qDebug() << "url Changed!" << url.toString();

        this->stop();
        this->load(url);
        qDebug() << "-- load url";
    }

    if (loader) {
        loader->close();
        qDebug() << "-- close loader";
        loader = NULL;
    }
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
        QFileInfo finfo = QFileInfo();
        finfo.setFile(sound);
        if (finfo.exists()) {
            qDebug() << "Play sound: " << sound;
            getPlayer()->play(sound);
        } else {
            qDebug() << "Sound file" << sound << "not found!";
        }
    }
}

QWebEngineView *WebView::createWindow(QWebEnginePage::WebWindowType type)
{
    if (type != QWebEnginePage::WebBrowserWindow) {
        return NULL;
    }

    if (loader == NULL) {
        qDebug() << "New fake webview loader";
        loader = new FakeWebView(this);
        QWebEnginePage *newWeb = new QWebEnginePage(loader);
        loader->setAttribute(Qt::WA_DeleteOnClose, true);
        loader->setPage(newWeb);

        connect(loader, SIGNAL(urlChanged(const QUrl&)), SLOT(handleUrlChanged(const QUrl&)));
    }

    return loader;
}

void WebView::handlePrintRequested(QWebEnginePage *wf)
{
    //TODO: currently probably impossible;
    #if 0
    qDebug() << "Handle printRequested...";
    if (mainSettings->value("printing/enable").toBool()) {
        if (!mainSettings->value("printing/show-printer-dialog").toBool()) {
            if (printer->printerState() != QPrinter::Error) {
                qDebug() << "... got printer, try use it";
                wf->print(printer);
            }
        }
    }
    #endif
}
