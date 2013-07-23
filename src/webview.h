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
