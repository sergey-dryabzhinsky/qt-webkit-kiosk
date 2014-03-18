#ifndef SOCKETPAIR_H
#define SOCKETPAIR_H

/**
 * Unix socketpair function emulation
 */

#include <QObject>
#include <QTcpSocket>
#include <QTcpServer>

class SocketPair: public QObject
{
    Q_OBJECT
public:
    SocketPair(QObject *parent = 0);

    bool create();
    void close();
    QTcpSocket* input();
    QTcpSocket* output();

private:
    QTcpSocket *endPoints[2];
    QTcpServer *server;
};

#endif // SOCKETPAIR_H
