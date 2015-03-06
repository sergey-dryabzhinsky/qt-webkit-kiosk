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

    QIcon icon();


public slots:
    void handlePrintRequested(QWebEnginePage *);
    void handleUrlChanged(const QUrl &);

signals:
    void iconChanged();

protected:
    void mousePressEvent(QMouseEvent *event);
    QPlayer *getPlayer();

private:
    QPlayer *player;
    QSettings *mainSettings;
    FakeWebView *loader;
    QPrinter *printer;

    QIcon pageIcon;
    QNetworkReply* pageIconReply;

private slots:
    void handleWindowCloseRequested();
    void onIconLoaded();
    void onIconUrlChanged(const QUrl&);

};

#endif // WEBVIEW_H
