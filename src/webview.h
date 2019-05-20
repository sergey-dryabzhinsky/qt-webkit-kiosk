#ifndef WEBVIEW_H
#define WEBVIEW_H

#include <QtWebKit>

#ifdef QT5
#include <QtWebKitWidgets/QWebView>
#include <QtWebKitWidgets/QWebFrame>
#endif

#include <QPrinter>
#include "qplayer.h"
#include "qwk_webpage.h"
#include "fakewebview.h"
#include "qwk_settings.h"

class WebView : public QWebView
{
    Q_OBJECT

public:
    explicit WebView(QWidget* parent = 0);

    void setSettings(QwkSettings *settings);
    QwkSettings* getSettings();

    void loadCustomPage(QString uri);
    void loadHomepage();
    void initSignals();

    void setPage(QwkWebPage* page);
    void resetLoadTimer();
    void stopLoadTimer();

    QWebView *createWindow(QWebPage::WebWindowType type);

    void playSound(QString soundSetting);

    // http://slow-tone.blogspot.com/2011/04/qt-hide-scrollbars-qwebview.html?showComment=1318404188431#c5258624438625837585
    void scrollUp();
    void scrollDown();
    void scrollPageUp();
    void scrollPageDown();
    void scrollHome();
    void scrollEnd();

public slots:
    void handlePrintRequested(QWebFrame *);
    void handleFakeviewUrlChanged(const QUrl &);
    void handleFakeviewLoadFinished(bool);

protected:
    void mousePressEvent(QMouseEvent *event);
    QPlayer *getPlayer();
    QWebView *getFakeLoader();
    QTimer *getLoadTimer();

signals:

    void qwkNetworkError(QNetworkReply::NetworkError error, QString message);
    void qwkNetworkReplyUrl(QUrl url);

private:
    QPlayer     *player;
    QwkSettings *qwkSettings;
    FakeWebView *loader;
    QPrinter    *printer;
    QTimer      *loadTimer;

private slots:
    void handleSslErrors(QNetworkReply* reply, const QList<QSslError> &errors);
    void handleWindowCloseRequested();

    void handleNetworkReply(QNetworkReply *reply);
    void handleAuthReply(QNetworkReply *aReply, QAuthenticator *aAuth);
    void handleProxyAuthReply(const QNetworkProxy &proxy, QAuthenticator *aAuth);

    void handleLoadTimerTimeout();
};

#endif // WEBVIEW_H
