#include "socketpair.h"
#include <QDebug>
#include <QIODevice>
#include <QtGlobal>

SocketPair::SocketPair(QObject *parent)
        : QObject(parent)
{
}

bool SocketPair::create()
{
    connect(&server, SIGNAL(newConnection()), this, SLOT(newConnection()), Qt::QueuedConnection);

    connect(&server, SIGNAL(acceptError(QAbstractSocket::SocketError)), this, SLOT(logServerError(QAbstractSocket::SocketError)));
    connect(&clientConnection, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(logClientConnectionError(QAbstractSocket::SocketError)));

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
    if(serverConnection != nullptr) {
        connect(serverConnection, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(logServerConnectionError(QAbstractSocket::SocketError)));
    } else {
        qFatal("%s:%d:%s, server.nextPendingConnection() was NULL.", __FILE__,
                __LINE__, __func__);
    }

    serverConnection->setSocketOption( QAbstractSocket::LowDelayOption, 1 );
    serverConnection->setSocketOption( QAbstractSocket::KeepAliveOption, 1 );
    serverConnection->setReadBufferSize( 1 );

    connect(serverConnection, SIGNAL(readyRead()), this, SLOT(readServerData()));
    server.close();
    emit clientConnected();
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

void SocketPair::logServerError(QAbstractSocket::SocketError socketError) {
    qCritical("%s:%d:%s, server.serverError() is: '%d', and server.errorString() "
            "is: '%s', and socketError is: '%d'", __FILE__, __LINE__, __func__,
            server.serverError(), qUtf8Printable(server.errorString()),
            socketError);
}

void SocketPair::logServerConnectionError(QAbstractSocket::SocketError socketError) {
    qCritical("%s:%d:%s, serverConnection->error() is: '%d', and "
           "serverConnection->errorString() is: '%s', and socketError is: '%d'",
           __FILE__, __LINE__, __func__, serverConnection->error(),
            qUtf8Printable(serverConnection->errorString()), socketError);
}

void SocketPair::logClientConnectionError(QAbstractSocket::SocketError socketError) {
    qCritical("%s:%d:%s, clientConnection.error() is: '%d', and "
           "clientConnection.errorString() is: '%s', and socketError is: '%d'",
           __FILE__, __LINE__, __func__, clientConnection.error(),
            qUtf8Printable(clientConnection.errorString()), socketError);
}
