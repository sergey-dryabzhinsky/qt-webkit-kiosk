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
    void clientConnected();
    void sigData(QByteArray);

public slots:
    void newConnection();
    void readServerData();

private:
    QTcpSocket *serverConnection;
    QTcpSocket clientConnection;
    QTcpServer server;

private slots:
    void logServerError(QAbstractSocket::SocketError);
    void logServerConnectionError(QAbstractSocket::SocketError);
    void logClientConnectionError(QAbstractSocket::SocketError);

};

#endif // SOCKETPAIR_H
