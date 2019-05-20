#include "socketpair.h"
#include <QIODevice>
#include <QDebug>

SocketPair::SocketPair(QObject *parent)
        : QObject(parent)
{
    dataCheck = new QTimer();
    dataCheck->setInterval(100);
}

bool SocketPair::create()
{
    connect(dataCheck, SIGNAL(timeout()), this, SLOT(readServerData()));
    connect(&server, SIGNAL(newConnection()), this, SLOT(newConnection()));

    int tries = 5;
    while (tries) {
        if (!server.isListening()) {
            if (server.listen(QHostAddress::LocalHost)) {
                break;
            }
        } else {
            break;
        }
        tries--;
    }
    if (!server.isListening()) {
        qDebug() << "Can't start tcpServer:" << server.errorString();
        return false;
    }

	clientConnection.setSocketOption( QAbstractSocket::LowDelayOption, 1 );
	clientConnection.setSocketOption( QAbstractSocket::KeepAliveOption, 1 );

    clientConnection.connectToHost(QHostAddress::LocalHost, server.serverPort(), QIODevice::WriteOnly);
    return true;
}

void SocketPair::newConnection()
{
    serverConnection = server.nextPendingConnection();

	serverConnection->setSocketOption( QAbstractSocket::LowDelayOption, 1 );
	serverConnection->setSocketOption( QAbstractSocket::KeepAliveOption, 1 );
	serverConnection->setReadBufferSize( 1 );

    dataCheck->start();

//    connect(serverConnection, SIGNAL(readyRead()), this, SLOT(readServerData()));
    server.close();
}

void SocketPair::readServerData()
{
    QByteArray data = serverConnection->readAll();
    if (data.length()) {
        emit sigData(data);
    }
}

void SocketPair::close()
{
    dataCheck->stop();
    clientConnection.close();
    if (serverConnection) {
        serverConnection->close();
        delete serverConnection;
        serverConnection = 0;
    }
    server.close();
}

QTcpSocket* SocketPair::input()
{
    return &clientConnection;
}

QTcpSocket* SocketPair::output()
{
    return serverConnection;
}
