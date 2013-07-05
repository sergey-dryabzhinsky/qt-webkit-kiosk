#ifndef CACHINGNM_H
#define CACHINGNM_H

#include <QtNetwork>

class CachingNetworkManager : public QNetworkAccessManager
{
    Q_OBJECT

public:
    explicit CachingNetworkManager(QObject* parent = 0);

    QNetworkReply *createRequest( Operation op, const QNetworkRequest & req, QIODevice * outgoingData);
};


#endif // CACHINGNM_H
