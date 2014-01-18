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
}

/**
 * Call after setPage
 * @brief WebView::initSignals
 */
void WebView::initSignals()
{
    connect(page()->networkAccessManager(),
            SIGNAL(sslErrors(QNetworkReply*, const QList<QSslError> & )),
            this,
            SLOT(handleSslErrors(QNetworkReply*, const QList<QSslError> & )));

    connect(page(),
            SIGNAL(windowCloseRequested()),
            this,
            SLOT(handleWindowCloseRequested()));

    connect(page(),
            SIGNAL(printRequested(QWebFrame*)),
            this,
            SLOT(handlePrintRequested(QWebFrame*)));
}

void WebView::setPage(QWebPage *page)
{
    QWebView::setPage(page);
    initSignals();
}


void WebView::setSettings(QSettings *settings)
{
    mainSettings = settings;

    if (mainSettings->value("printing/enable").toBool()) {
        if (!mainSettings->value("printing/show-printer-dialog").toBool()) {
            printer = new QPrinter();
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
    QFileInfo finfo = QFileInfo();
    finfo.setFile(mainSettings->value("browser/homepage").toString());

    qDebug() << "Homepage: like file = " <<
                mainSettings->value("browser/homepage").toString() <<
                ", absolute path = " <<
                finfo.absoluteFilePath() <<
                ", local uri = " <<
                QUrl::fromLocalFile(
                    mainSettings->value("browser/homepage").toString()
                ).toString();

    if (finfo.isFile()) {
        qDebug() << "Homepage: Local FILE!";
        this->load(QUrl::fromLocalFile(
            finfo.absoluteFilePath()
        ));
    } else {
        qDebug() << "Homepage: Remote URI!";
        this->load(QUrl(
            mainSettings->value("browser/homepage").toString()
        ));
    }
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

void WebView::handleWindowCloseRequested()
{
    qDebug() << "Handle windowCloseRequested:" << mainSettings->value("browser/show_homepage_on_window_close").toString();
    if (mainSettings->value("browser/show_homepage_on_window_close").toBool()) {
        qDebug() << "-- load homepage";
        loadHomepage();
    } else {
        qDebug() << "-- exit application";
        QKeyEvent * eventExit = new QKeyEvent( QEvent::KeyPress, Qt::Key_Q, Qt::ControlModifier, "Exit", 0 );
        QCoreApplication::postEvent( this, eventExit );
    }
}

void WebView::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        qDebug() << "Window Clicked!";
        playSound("event-sounds/window-clicked");
    }
    QWebView::mousePressEvent(event);
}

void WebView::handleUrlChanged(const QUrl &url)
{
    qDebug() << "url Changed!" << url.toString();

    this->load(url);
    qDebug() << "-- load url";

    loader->close();
    qDebug() << "-- close";
    loader = NULL;
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

QWebView *WebView::createWindow(QWebPage::WebWindowType type)
{
    Q_UNUSED(type);

    if (loader == NULL) {
        qDebug() << "New fake webview loader";
        loader = new FakeWebView(this);
        QWebPage *newWeb = new QWebPage(loader);
        loader->setAttribute(Qt::WA_DeleteOnClose, true);
        loader->setPage(newWeb);

        connect(loader, SIGNAL(urlChanged(const QUrl&)), SLOT(handleUrlChanged(const QUrl&)));
    }

    return loader;
}

void WebView::handlePrintRequested(QWebFrame *wf)
{
    qDebug() << "Handle printRequested...";
    if (mainSettings->value("printing/enable").toBool()) {
        if (!mainSettings->value("printing/show-printer-dialog").toBool()) {
            if (printer->printerState() != QPrinter::Error) {
                qDebug() << "... got printer, try use it";
                wf->print(printer);
            }
        }
    }
}

void WebView::scrollDown()
{
    QWebFrame* frame = this->page()->mainFrame();
    QPoint point = frame->scrollPosition();
    frame->setScrollPosition(point + QPoint(0, 100));
}

void WebView::scrollPageDown()
{
    QWebFrame* frame = this->page()->mainFrame();
    QPoint point = frame->scrollPosition();
    frame->setScrollPosition(point + QPoint(0, this->page()->mainFrame()->geometry().height()));
}

void WebView::scrollEnd()
{
    QWebFrame* frame = this->page()->mainFrame();
    frame->setScrollPosition(QPoint(0, frame->contentsSize().height()));
}

void WebView::scrollUp()
{
    QWebFrame* frame = this->page()->mainFrame();
    QPoint point = frame->scrollPosition();
    frame->setScrollPosition(point - QPoint(0, 100));
}

void WebView::scrollPageUp()
{
    QWebFrame* frame = this->page()->mainFrame();
    QPoint point = frame->scrollPosition();
    frame->setScrollPosition(point - QPoint(0, this->page()->mainFrame()->geometry().height()));
}

void WebView::scrollHome()
{
    QWebFrame* frame = this->page()->mainFrame();
    frame->setScrollPosition(QPoint(0, 0));
}

