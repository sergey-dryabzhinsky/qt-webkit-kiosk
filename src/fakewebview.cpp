#include <fakewebview.h>

FakeWebView::FakeWebView(QWidget *parent) : QWebView(parent)
{
}

void FakeWebView::setUrl(const QUrl &url)
{
    emit ( urlChanged( url ) );
}


void FakeWebView::load(const QUrl &url)
{
    emit ( urlChanged( url ) );
}

void FakeWebView::load(const QNetworkRequest &request,
                       QNetworkAccessManager::Operation operation,
                       const QByteArray &body)
{
    Q_UNUSED(operation);
    Q_UNUSED(body);
    emit ( urlChanged( request.url() ) );
}
