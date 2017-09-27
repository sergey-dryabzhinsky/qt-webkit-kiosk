#include "config.h"

#include <QtDebug>
#include <QtGui>
#include <QtWebKit>
#include "webview.h"
#include <signal.h>
#include "unixsignals.h"

#ifdef QT5
#include <QNetworkReply>
#include <QSslError>
#endif

//#define SHOW_ALSO_OKAY_DEBUG

WebView::WebView(QWidget* parent): QWebView(parent)
{
    player = NULL;
    loader = NULL;
    loadTimer = NULL;
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

    /*handle network reply*/
    connect(page()->networkAccessManager(),
            SIGNAL(finished(QNetworkReply*)),
            this,
            SLOT(handleNetworkReply(QNetworkReply*)));

    /*handle auth request to be in stable state*/
    connect(page()->networkAccessManager(),
           SIGNAL(authenticationRequired(QNetworkReply*,QAuthenticator*)),
            this,
            SLOT(handleAuthReply(QNetworkReply*,QAuthenticator*)));

    /*handle proxy auth request to be in stable state*/
    connect(page()->networkAccessManager(),
           SIGNAL(proxyAuthenticationRequired(QNetworkProxy,QAuthenticator*)),
            this,
            SLOT(handleProxyAuthReply(QNetworkProxy,QAuthenticator*)));

}

void WebView::setPage(QWebPage *page)
{
    QWebView::setPage(page);
    initSignals();
}


void WebView::setSettings(QwkSettings *settings)
{
    qwkSettings = settings;

    if (qwkSettings->getBool("printing/enable")) {
        if (!qwkSettings->getBool("printing/show-printer-dialog")) {
            if (!printer) {
                printer = new QPrinter();
            }
            printer->setPrinterName(qwkSettings->getQString("printing/printer"));
            printer->setPageMargins(
                qwkSettings->getReal("printing/page_margin_left"),
                qwkSettings->getReal("printing/page_margin_top"),
                qwkSettings->getReal("printing/page_margin_right"),
                qwkSettings->getReal("printing/page_margin_bottom"),
                QPrinter::Millimeter
            );
        }
    }

}

void WebView::loadHomepage()
{
    loadCustomPage(qwkSettings->getQString("browser/homepage"));
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
    if (this->getLoadTimer()) {
        this->getLoadTimer()->start(qwkSettings->getUInt("browser/page_load_timeout"));
    }
}


void WebView::handleSslErrors(QNetworkReply* reply, const QList<QSslError> &errors)
{
    qDebug() << QDateTime::currentDateTime().toString() << "handleSslErrors: ";
    QString errStr = "";
    foreach (QSslError e, errors) {
        qDebug() << "ssl error: " << e.errorString();
        errStr += " " + e.errorString();
    }

    if (qwkSettings->getBool("browser/ignore_ssl_errors")) {
        reply->ignoreSslErrors();
    } else {
        reply->abort();
        emit qwkNetworkError(reply->error(), QString("Network SSL errors: ") + errStr);
    }
}

void WebView::handleNetworkReply(QNetworkReply* reply)
{
    if( reply ) {
        if( reply->error()) {
            qDebug() << QDateTime::currentDateTime().toString() << "handleNetworkReply URL:" << reply->request().url().toString();
            QString errStr = reply->errorString();
            qWarning() << QDateTime::currentDateTime().toString() << "handleNetworkReply ERROR:" << reply->error() << "=" << errStr;
            emit qwkNetworkError(reply->error(), reply->errorString());
        } else {
            #ifdef SHOW_ALSO_OKAY_DEBUG
            qDebug() << QDateTime::currentDateTime().toString() << "handleNetworkReply URL:" << reply->request().url().toString();
            qDebug() << QDateTime::currentDateTime().toString() << "handleNetworkReply OK";
            // emit qwkNetworkReplyUrl(reply->request().url());
            #endif
        }
    }
}

void WebView::handleAuthReply(QNetworkReply* aReply, QAuthenticator* aAuth)
{
    if( aReply && aAuth ) {
        qDebug() << QDateTime::currentDateTime().toString() << "handleAuthReply, need authorization, do nothing for now";
        emit qwkNetworkError(QNetworkReply::AuthenticationRequiredError, QString("Web-site need authorization! Nothing to do for now :("));
    }
}

void WebView::handleProxyAuthReply(const QNetworkProxy &proxy, QAuthenticator* aAuth)
{
    Q_UNUSED(proxy);
    if( aAuth ) {
        qDebug() << QDateTime::currentDateTime().toString() << "handleProxyAuthReply, need proxy authorization, do nothing for now";
        emit qwkNetworkError(QNetworkReply::AuthenticationRequiredError, QString("Proxy need authorization! Check your proxy auth settings!"));
    }
}

void WebView::handleWindowCloseRequested()
{
    qDebug() << QDateTime::currentDateTime().toString() << "Handle windowCloseRequested: " << qwkSettings->getQString("browser/show_homepage_on_window_close");
    if (qwkSettings->getBool("browser/show_homepage_on_window_close")) {
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
    QWebView::mousePressEvent(event);
}


/**
 * @brief WebView::getFakeLoader
 * Fake window handler for 'window.open()' and '_blank' target
 * Just need to catch url setup
 * @return FakeWebView
 */
QWebView *WebView::getFakeLoader()
{
    if (!loader) {
        qDebug() << QDateTime::currentDateTime().toString() << "New fake webview loader";
        loader = new FakeWebView(this);
        loader->hide();
        QWebPage *newWeb = new QWebPage(loader);
        loader->setPage(newWeb);

        connect(loader, SIGNAL(urlChanged(const QUrl&)), SLOT(handleFakeviewUrlChanged(const QUrl&)));
        connect(loader, SIGNAL(loadFinished(bool)), SLOT(handleFakeviewLoadFinished(bool)));
    }
    return loader;
}

void WebView::handleFakeviewUrlChanged(const QUrl &url)
{
    qDebug() << QDateTime::currentDateTime().toString() << "WebView::handleFakeviewUrlChanged";
    Q_UNUSED(url);
    if (loader) {
        loader->stop();
    }
}

void WebView::handleFakeviewLoadFinished(bool ok)
{
    qDebug() << QDateTime::currentDateTime().toString() << "WebView::handleFakeviewLoadFinished: ok=" << (int)ok;
    if (loader) {
        QUrl url = loader->url();
        if (this->url().toString() != url.toString()) {
            qDebug() << "url Changed!" << url.toString();
            this->loadCustomPage(url.toString());
            qDebug() << "-- load url";
        }
    }
}


QPlayer *WebView::getPlayer()
{
    if (qwkSettings->getBool("event-sounds/enable")) {
        if (player == NULL) {
            player = new QPlayer();
        }
    }
    return player;
}


QTimer *WebView::getLoadTimer()
{
    if (qwkSettings->getUInt("browser/page_load_timeout")) {
        if (loadTimer == NULL) {
            loadTimer = new QTimer();
            connect(loadTimer, SIGNAL(timeout()), SLOT(handleLoadTimerTimeout()));
        }
    }
    return loadTimer;
}

void WebView::handleLoadTimerTimeout()
{
    loadTimer->stop();
    this->stop();
    if (loader) {
        loader->stop();
    }
    emit qwkNetworkError(QNetworkReply::TimeoutError, QString("Page load timed out! Connection problems?"));
}


/**
 * @brief WebView::resetLoadTimer
 * If page loading, progress event emited
 * And we reset timer
 * If page not loading - timer will be triggered
 */
void WebView::resetLoadTimer()
{
    if (getLoadTimer()) {
        getLoadTimer()->stop();
        getLoadTimer()->start(qwkSettings->getUInt("browser/page_load_timeout"));
    }
}


/**
 * @brief WebView::stopLoadTimer
 * If page loaded, we stop timer
 */
void WebView::stopLoadTimer()
{
    if (getLoadTimer()) {
        getLoadTimer()->stop();
    }
}


void WebView::playSound(QString soundSetting)
{
    if (getPlayer()) {
        QString sound = qwkSettings->getQString(soundSetting);
        QFileInfo finfo = QFileInfo();
        finfo.setFile(sound);
        if (finfo.exists()) {
            qDebug() << "Play sound: " << sound;
            getPlayer()->play(sound);
        } else {
            qDebug() << "Sound file '" << sound << "' not found!";
        }
    }
}

QWebView *WebView::createWindow(QWebPage::WebWindowType type)
{
    if (type != QWebPage::WebBrowserWindow) {
        return NULL;
    }
    qDebug() << QDateTime::currentDateTime().toString() << "Handle createWindow...";

    return getFakeLoader();
}

void WebView::handlePrintRequested(QWebFrame *wf)
{
    qDebug() << QDateTime::currentDateTime().toString() << "Handle printRequested...";
    if (qwkSettings->getBool("printing/enable")) {
        if (!qwkSettings->getBool("printing/show-printer-dialog")) {
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

bool WebView::shouldInterruptJavaScript()
{
    qDebug() << QDateTime::currentDateTime().toString() << "WebView::shouldInterruptJavaScript" ;
    return false;
}
