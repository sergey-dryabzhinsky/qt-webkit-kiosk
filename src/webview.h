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
    WebView(QWidget* parent = 0);

    void setSettings(QSettings *settings);

protected:

    void mousePressEvent(QMouseEvent *event);
    void linkClicked(const QUrl &);

private:
    QPlayer *player;
    QSettings *mainSettings;
};

#endif // WEBVIEW_H
