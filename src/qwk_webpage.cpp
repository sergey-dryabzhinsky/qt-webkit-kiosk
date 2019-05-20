/**
 * Qt Webkit Kiosk version of QWebPage
 * With custom JS interrupt slot
 */


#include "config.h"

#include <QtDebug>
#include <QtGui>
#include <QtWebKit>
#include "webview.h"
#include "qwk_webpage.h"


QwkWebPage::QwkWebPage(QWidget* parent): QWebPage(parent)
{
    javascriptHangStarted = 0;
}

QwkSettings* QwkWebPage::getSettings()
{
    if (this->view() != NULL) {
        WebView* v = (WebView *)(this->view());
        return v->getSettings();
    }
    return NULL;
}

bool QwkWebPage::shouldInterruptJavaScript()
{
    qWarning() << QDateTime::currentDateTime().toString() << "shouldInterruptJavaScript: Handle JavaScript Interrupt...";
    QwkSettings *s = this->getSettings();

    if (s != NULL) {
        if (s->getBool("browser/interrupt_javascript")) {

            qint64 now = QDateTime::currentMSecsSinceEpoch();
            if (!javascriptHangStarted) {
                // hang started 30s back
                qDebug() << "-- first interrupt attempt?";
                // In Qt4 - hang interval check - 10 sec
                javascriptHangStarted = now - 10*1000;
                qDebug() << "-- hang start time in msec:" << javascriptHangStarted;
            }

            qDebug() << "-- current time in msec:" << now;

            qint64 interval = s->getInt("browser/interrupt_javascript_interval") * 1000;
            qDebug() << "-- max hang interval in msec:" << interval;
            qDebug() << "-- current hang interval in msec:" << now - javascriptHangStarted;

            if (now - javascriptHangStarted >= interval) {
                qWarning() << QDateTime::currentDateTime().toString() << "shouldInterruptJavaScript: Stop javascript! Executed too long!";
                return true;
            }
        }

        qApp->processEvents(QEventLoop::AllEvents, 50);
        // No interrupt, ever
        return false;
    }

    // Interrupt, by default
    return QWebPage::shouldInterruptJavaScript();
}
