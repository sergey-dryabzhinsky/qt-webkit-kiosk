#include "socketpair.h"
#include <QTcpSocket>

SocketPair::SocketPair(QObject *parent)
        : QObject(parent)
{
    endPoints[0] = endPoints[1] = 0;
    server = new QTcpServer();
}

bool SocketPair::create()
{
    server->listen();

    QTcpSocket *active = new QTcpSocket(this);
    active->connectToHost("127.0.0.1", server->serverPort());

    // need more time as working with embedded
    // device and testing from emualtor
    // things tend to get slower
    if (!active->waitForConnected(1000))
        return false;

    if (!server->waitForNewConnection(1000))
        return false;

    QTcpSocket *passive = server->nextPendingConnection();
    passive->setParent(this);

    endPoints[0] = active;
    endPoints[1] = passive;
    return true;
}

void SocketPair::close()
{
    server->close();
    if (endPoints[0]) {
        endPoints[0]->close();
    }
    if (endPoints[1]) {
        endPoints[1]->close();
    }
}

QTcpSocket* SocketPair::input()
{
    return endPoints[0];
}

QTcpSocket* SocketPair::output()
{
    return endPoints[1];
}
