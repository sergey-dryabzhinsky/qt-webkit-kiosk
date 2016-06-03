/**
 * Qt Webkit Kiosk version of QWebPage
 * With custom JS interrupt slot
 */


#include "config.h"

#include <QtDebug>
#include <QtGui>
#include <QtWebKit>
#include "webview.h"
#include "qwk-webpage.h"


QwkWebPage::QwkWebPage(QWidget* parent): QWebPage(parent)
{
    javascriptHangStarted = 0;
}

QSettings* QwkWebPage::getSettings()
{
    if (this->view() != NULL) {
        WebView* v = (WebView *)(this->view());
        return v->getSettings();
    }
    return NULL;
}

bool QwkWebPage::shouldInterruptJavaScript()
{
    qDebug() << "Handle JavaScript Interrupt...";
    QSettings *s = this->getSettings();

    if (s != NULL) {
        if (s->value("browser/interrupt_javascript").toBool()) {

            qint64 now = QDateTime::currentMSecsSinceEpoch();
            if (!javascriptHangStarted) {
                // hang started 30s back
                qDebug() << "-- first interrupt attempt?";
                javascriptHangStarted = now - 30*1000;
                qDebug() << "-- hang start time in msec:" << javascriptHangStarted;
            }

            qDebug() << "-- current time in msec:" << now;

            qint64 interval = s->value("browser/interrupt_javascript_interval").toInt() * 1000;
            qDebug() << "-- max hang interval in msec:" << interval;

            if (now - javascriptHangStarted >= interval) {
                qDebug() << "-- stop javascript! hanged too long!";
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
