#ifndef WEBVIEW_H
#define WEBVIEW_H

#include <QtGui>
#include <QtWebKit>
#include <phonon/audiooutput.h>
#include <phonon/mediaobject.h>
#include <phonon/backendcapabilities.h>

class WebView : public QWebView
{
    Q_OBJECT

public:
    WebView(QWidget* parent = 0);

    void setSettings(QSettings *settings);

protected:

    void mousePressEvent(QMouseEvent *event);

private:
    Phonon::MediaObject *mediaObject;
    Phonon::AudioOutput *audioOutput;
    QSettings *mainSettings;
};

#endif // WEBVIEW_H
