#ifndef QWK_WEBPAGE_H
#define QWK_WEBPAGE_H

#include <QtWebKit>
#include "qwk_settings.h"

#ifdef QT5
#include <QtWebKitWidgets/QWebView>
#endif

class QwkWebPage : public QWebPage
{
    Q_OBJECT

public:
    explicit QwkWebPage(QWidget* parent = 0);

    QwkSettings* getSettings();

private:
    qint64  javascriptHangStarted;

public Q_SLOTS:
    bool shouldInterruptJavaScript();

};

#endif // QWK_WEBPAGE_H
