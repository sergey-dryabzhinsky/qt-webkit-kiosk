#include <signal.h>
#include <QApplication>
#include <QDebug>
#include "unixsignals.h"

SocketPair UnixSignals::sockPair;

/**
 * It should be created only ones
 *
 * @brief UnixSignals::UnixSignals
 * @param parent
 */
UnixSignals::UnixSignals( QObject *parent )
    : QObject(parent)
{
    if (!UnixSignals::sockPair.create())
        qFatal("Unable to create signal socket pair");

    connect(&UnixSignals::sockPair, SIGNAL(sigData(QByteArray)), this, SLOT(handleSig(QByteArray)));
}

void UnixSignals::start()
{
#ifdef SIGBREAK
    // Available on Windows
    if (receivers(SIGNAL(sigBREAK())) > 0) {
        if (signal(SIGBREAK, UnixSignals::signalHandler) == SIG_ERR) {
            qFatal("Unable to set signal handler on BREAK");
        }
    } else {
        qDebug("No listeners for signal BREAK");
    }
#else
    qWarning("No signal BREAK defined");
#endif

#ifdef SIGTERM
    if (receivers(SIGNAL(sigTERM())) > 0) {
        if (signal(SIGTERM, UnixSignals::signalHandler) == SIG_ERR) {
            qFatal("Unable to set signal handler on TERM");
        }
    } else {
        qDebug("No listeners for signal TERM");
    }
#else
    qWarning("No signal TERM defined");
#endif

#ifdef SIGINT
    if (receivers(SIGNAL(sigINT())) > 0) {
        if (signal(SIGINT, UnixSignals::signalHandler) == SIG_ERR) {
            qFatal("Unable to set signal handler on INT");
        }
    } else {
        qDebug("No listeners for signal INT");
    }
#else
    qWarning("No signal INT defined");
#endif

#ifdef SIGHUP
    // Unavailable on Windows
    if (receivers(SIGNAL(sigHUP())) > 0) {
        if (signal(SIGHUP, UnixSignals::signalHandler) == SIG_ERR) {
            qFatal("Unable to set signal handler on HUP");
        }
    } else {
        qDebug("No listeners for signal HUP");
    }
#else
    qWarning("No signal HUP defined");
#endif

#ifdef SIGUSR1
    // Unavailable on Windows
    if (receivers(SIGNAL(sigUSR1())) > 0) {
        if (signal(SIGUSR1, UnixSignals::signalHandler) == SIG_ERR) {
            qFatal("Unable to set signal handler on USR1");
        }
    } else {
        qDebug("No listeners for signal USR1");
    }
#else
    qWarning("No signal USR1 defined");
#endif

#ifdef SIGUSR2
    // Unavailable on Windows
    if (receivers(SIGNAL(sigUSR2())) > 0) {
        if (signal(SIGUSR2, UnixSignals::signalHandler) == SIG_ERR) {
            qFatal("Unable to set signal handler on USR2");
        }
    } else {
        qDebug("No listeners for signal USR1");
    }
#else
    qWarning("No signal USR2 defined");
#endif
}

void UnixSignals::stop()
{
    UnixSignals::sockPair.close();
}

void UnixSignals::signalHandler(int number)
{
    char tmp = number;
    qDebug() << "UnixSignals::signalHandler -- pass signal" << number;
    UnixSignals::sockPair.input()->write(&tmp, sizeof(tmp));
}

void UnixSignals::handleSig(QByteArray data)
{
    qDebug() << "Got data:" << data.toHex();
    int number = 0, lastNumber = 0;
    if (data.length()) {
        qDebug() << " signals in data:" << data.length();

        // Socket can be filled with several signals
        // If they come too fast...
        while (data.length()) {
            number = data[0];
            data.remove(0, 1);

            // Skeep repeated signals
            if (number != lastNumber) {
                handleSignal(number);
                lastNumber = number;
            }
        }

    }
}

void UnixSignals::handleSignal(int number)
{
    qDebug() << "Got signal:" << number;
    switch (number) {
#ifdef SIGBREAK
        case SIGBREAK:
            qDebug() << "Got SIGBREAK! emit event!";
            emit sigBREAK();
            break;
#endif
#ifdef SIGHUP
        case SIGHUP:
            qDebug() << "Got SIGHUP! emit event!";
            emit sigHUP();
            break;
#endif
#ifdef SIGINT
        case SIGINT:
            qDebug() << "Got SIGINT! emit event!";
            emit sigINT();
            break;
#endif
#ifdef SIGTERM
        case SIGTERM:
            qDebug() << "Got SIGTERM! emit event!";
            emit sigTERM();
            break;
#endif
#ifdef SIGUSR1
        case SIGUSR1:
            qDebug() << "Got SIGUSR1! emit event!";
            emit sigUSR1();
            break;
#endif
#ifdef SIGUSR2
        case SIGUSR2:
            qDebug() << "Got SIGUSR2! emit event!";
            emit sigUSR2();
            break;
#endif
        default:
            qDebug() << "Got something? Dunno what to do...";
    }

}
