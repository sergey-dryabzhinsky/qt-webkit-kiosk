#include <fakewebview.h>

FakeWebView::FakeWebView(QWidget *parent) : QWebView(parent)
{
    this->settings()->setAttribute(QWebSettings::JavascriptEnabled, false);
    this->settings()->setAttribute(QWebSettings::WebGLEnabled, false);
    this->settings()->setAttribute(QWebSettings::JavaEnabled, false);
    this->settings()->setAttribute(QWebSettings::PluginsEnabled, false);
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
