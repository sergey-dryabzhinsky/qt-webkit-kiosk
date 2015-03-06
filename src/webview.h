#ifndef WEBVIEW_H
#define WEBVIEW_H

#include <QtWebKit>

#include <QtWebKitWidgets/QWebView>
#include <QtWebKitWidgets/QWebFrame>

#include <QtWebEngineWidgets/QWebEnginePage>
#include <QtWebEngineWidgets/QWebEngineView>

#include <QPrinter>
#include <qplayer.h>
#include <fakewebview.h>

class WebView : public QWebEngineView
{
    Q_OBJECT

public:
    explicit WebView(QWidget* parent = 0);

    void setSettings(QSettings *settings);
    void loadCustomPage(QString uri);
    void loadHomepage();
    void initSignals();

    void setPage(QWebEnginePage* page);

    QWebEngineView *createWindow(QWebEnginePage::WebWindowType type);

    void playSound(QString soundSetting);



public slots:
    void handlePrintRequested(QWebEnginePage *);
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
