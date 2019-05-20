#ifndef SOCKETPAIR_H
#define SOCKETPAIR_H

/**
 * Unix socketpair function emulation
 */

#include <QObject>
#include <QTcpSocket>
#include <QTcpServer>
#include <QTimer>

class SocketPair: public QObject
{
    Q_OBJECT
public:
    SocketPair(QObject *parent = 0);

    bool create();
    void close();
    QTcpSocket* input();
    QTcpSocket* output();

Q_SIGNALS:
    void sigData(QByteArray);

public slots:
    void newConnection();
    void readServerData();

private:
    QTimer *dataCheck;
    QTcpSocket *serverConnection;
    QTcpSocket clientConnection;
    QTcpServer server;
};

#endif // SOCKETPAIR_H
