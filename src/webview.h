#ifndef WEBVIEW_H
#define WEBVIEW_H

#include <QtWebKit>
#include <QtWebKitWidgets/QWebView>
#include <QtWebKitWidgets/QWebFrame>
#include <QPrinter>
#include <qplayer.h>
#include <fakewebview.h>

class WebView : public QWebView
{
    Q_OBJECT

public:
    explicit WebView(QWidget* parent = 0);

    void setSettings(QSettings *settings);
    void loadHomepage();
    void initSignals();

    void setPage(QWebPage* page);

    QWebView *createWindow(QWebPage::WebWindowType type);

    void playSound(QString soundSetting);

public slots:
    void handlePrintRequested(QWebFrame *);
    void handleUrlChanged(const QUrl &);

    // http://slow-tone.blogspot.com/2011/04/qt-hide-scrollbars-qwebview.html?showComment=1318404188431#c5258624438625837585
    void scrollUp();
    void scrollDown();
    void scrollPageUp();
    void scrollPageDown();
    void scrollHome();
    void scrollEnd();

protected:
    void mousePressEvent(QMouseEvent *event);
    QPlayer *getPlayer();

private:
    QPlayer *player;
    QSettings *mainSettings;
    FakeWebView *loader;
    QPrinter *printer;

private slots:
    void handleSslErrors(QNetworkReply* reply, const QList<QSslError> &errors);
    void handleWindowCloseRequested();

};

#endif // WEBVIEW_H
