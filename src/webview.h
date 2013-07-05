#ifndef WEBVIEW_H
#define WEBVIEW_H

#include <QtGui>
#include <QtWebKit>
#include <QtWebKitWidgets/QWebView>
#include <qplayer.h>

class WebView : public QWebView
{
    Q_OBJECT

public:
    explicit WebView(QWidget* parent = 0);

    void setSettings(QSettings *settings);

    QWebView *createWindow(QWebPage::WebWindowType type);
protected slots:
    void linkClicked(const QUrl &);
    void urlChanged(const QUrl &);

protected:

    void mousePressEvent(QMouseEvent *event);
    QPlayer *getPlayer();
    void playSound(QString soundSetting);

private slots:
    void handleSslErrors(QNetworkReply* reply, const QList<QSslError> &errors);

private:
    QPlayer *player;
    QSettings *mainSettings;
    QWebView *loader;
};

#endif // WEBVIEW_H
