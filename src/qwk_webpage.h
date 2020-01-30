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

    static QString userAgent;

private:
    qint64  javascriptHangStarted;

public Q_SLOTS:
    bool shouldInterruptJavaScript();

protected:
    void javaScriptConsoleMessage( const QString & message, int lineNumber, const QString & sourceID );

    virtual QString userAgentForUrl(const QUrl & url) const;

};

#endif // QWK_WEBPAGE_H
