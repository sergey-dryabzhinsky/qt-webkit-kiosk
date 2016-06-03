#ifndef QWK_WEBPAGE_H
#define QWK_WEBPAGE_H

#include <QSettings>
#include <QtWebKit>

#ifdef QT5
#include <QtWebKitWidgets/QWebView>
#endif

class QwkWebPage : public QWebPage
{
    Q_OBJECT

public:
    explicit QwkWebPage(QWidget* parent = 0);

    QSettings* getSettings();

private:
    qint64  javascriptHangStarted;

public Q_SLOTS:
    bool shouldInterruptJavaScript();

};

#endif // QWK_WEBPAGE_H
