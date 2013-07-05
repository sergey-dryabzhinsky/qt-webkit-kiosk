#include <cachingnm.h>

CachingNetworkManager::CachingNetworkManager(QObject *parent) : QNetworkAccessManager(parent)
{
}

QNetworkReply *CachingNetworkManager::createRequest(QNetworkAccessManager::Operation op, const QNetworkRequest &req, QIODevice *outgoingData)
{
    QNetworkRequest request(req);
    request.setAttribute(QNetworkRequest::CacheLoadControlAttribute, QNetworkRequest::PreferCache);
    return QNetworkAccessManager::createRequest(op, request, outgoingData);
}
