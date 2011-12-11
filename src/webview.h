#ifndef WEBVIEW_H
#define WEBVIEW_H

#include <QtGui>
#include <QtWebKit>

class WebView : public QWebView
{
    Q_OBJECT

public:
    WebView(QWidget* parent = 0);

    void setSound(QSound *player);
    void setSettings(QSettings *settings);

protected:

    void mousePressEvent(QMouseEvent *event);

private:
    QSound *player;
    QSettings *mainSettings;
};

#endif // WEBVIEW_H
