#include <signal.h>
#include <sys/types.h>
#include <unistd.h>
#include <QDebug>
#include "unixsignals.h"

SocketPair UnixSignals::sp;

/**
 * It should be created only ones
 *
 * @brief UnixSignals::UnixSignals
 * @param parent
 */
UnixSignals::UnixSignals( QObject *parent )
    : QObject(parent)
{
    if (!UnixSignals::sp.create())
        qFatal("Unable to create signal socket pair");

    mNotifier = new QSocketNotifier(UnixSignals::sp.output()->socketDescriptor(), QSocketNotifier::Read, this);
    connect(mNotifier, SIGNAL(activated(int)), this, SLOT(handleSig()));
}

void UnixSignals::start()
{
#ifdef SIGBREAK
    // Available on Windows
    if (receivers(SIGNAL(sigBREAK())) > 0) {
        if (signal(SIGBREAK, UnixSignals::signalHandler) == SIG_ERR) {
            qFatal("Unable to set signal handler on BREAK");
        }
    }
#endif

    if (receivers(SIGNAL(sigTERM())) > 0) {
        if (signal(SIGTERM, UnixSignals::signalHandler) == SIG_ERR) {
            qFatal("Unable to set signal handler on TERM");
        }
    }

    if (receivers(SIGNAL(sigINT())) > 0) {
        if (signal(SIGINT, UnixSignals::signalHandler) == SIG_ERR) {
            qFatal("Unable to set signal handler on INT");
        }
    }

#ifdef SIGUSR1
    // Unavailable on Windows
    if (receivers(SIGNAL(sigUSR1())) > 0) {
        if (signal(SIGUSR1, UnixSignals::signalHandler) == SIG_ERR) {
            qFatal("Unable to set signal handler on USR1");
        }
    }
#endif

#ifdef SIGUSR2
    // Unavailable on Windows
    if (receivers(SIGNAL(sigUSR2())) > 0) {
        if (signal(SIGUSR2, UnixSignals::signalHandler) == SIG_ERR) {
            qFatal("Unable to set signal handler on USR2");
        }
    }
#endif

}

void UnixSignals::signalHandler(int number)
{
    char tmp = number;
    UnixSignals::sp.input()->write(&tmp, sizeof(tmp));
}

void UnixSignals::handleSig()
{
    mNotifier->setEnabled(false);
    char tmp;
    UnixSignals::sp.output()->read(&tmp, sizeof(tmp));
    switch (tmp) {
#ifdef SIGBREAK
    // Available on Windows
    case SIGBREAK:
        qDebug() << "Got SIGBREAK! emit event!";
        emit sigBREAK();
        break;
#endif
    case SIGTERM:
        qDebug() << "Got SIGTERM! emit event!";
        emit sigTERM();
        break;
    case SIGINT:
        qDebug() << "Got SIGINT! emit event!";
        emit sigINT();
        break;
#ifdef SIGUSR1
    // Unavailable on Windows
    case SIGUSR1:
        qDebug() << "Got SIGUSR1! emit event!";
        emit sigUSR1();
        break;
#endif
#ifdef SIGUSR2
    // Unavailable on Windows
    case SIGUSR2:
        qDebug() << "Got SIGUSR2! emit event!";
        emit sigUSR2();
        break;
#endif
    default:
        break;
    }
    mNotifier->setEnabled(true);
}
