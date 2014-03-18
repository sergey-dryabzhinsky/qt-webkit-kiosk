#include <signal.h>
#include <sys/types.h>
#include <unistd.h>
#include <QApplication>
#include <QDebug>
#include "unixsignals.h"

SocketPair UnixSignals::spBREAK;
SocketPair UnixSignals::spHUP;
SocketPair UnixSignals::spINT;
SocketPair UnixSignals::spTERM;
SocketPair UnixSignals::spUSR1;
SocketPair UnixSignals::spUSR2;

/**
 * It should be created only ones
 *
 * @brief UnixSignals::UnixSignals
 * @param parent
 */
UnixSignals::UnixSignals( QObject *parent )
    : QObject(parent)
{
    if (!UnixSignals::spBREAK.create())
        qFatal("Unable to create signal BREAK socket pair");

    if (!UnixSignals::spHUP.create())
        qFatal("Unable to create signal HUP socket pair");

    if (!UnixSignals::spINT.create())
        qFatal("Unable to create signal INT socket pair");

    if (!UnixSignals::spTERM.create())
        qFatal("Unable to create signal TERM socket pair");

    if (!UnixSignals::spUSR1.create())
        qFatal("Unable to create signal USR1 socket pair");

    if (!UnixSignals::spUSR2.create())
        qFatal("Unable to create signal USR2 socket pair");

    mNotifierBREAK = new QSocketNotifier(UnixSignals::spBREAK.output()->socketDescriptor(), QSocketNotifier::Read, this);
    connect(mNotifierBREAK, SIGNAL(activated(int)), this, SLOT(handleSigBREAK()), Qt::QueuedConnection);

    mNotifierHUP = new QSocketNotifier(UnixSignals::spHUP.output()->socketDescriptor(), QSocketNotifier::Read, this);
    connect(mNotifierHUP, SIGNAL(activated(int)), this, SLOT(handleSigHUP()), Qt::QueuedConnection);

    mNotifierINT = new QSocketNotifier(UnixSignals::spINT.output()->socketDescriptor(), QSocketNotifier::Read, this);
    connect(mNotifierINT, SIGNAL(activated(int)), this, SLOT(handleSigINT()), Qt::QueuedConnection);

    mNotifierTERM = new QSocketNotifier(UnixSignals::spTERM.output()->socketDescriptor(), QSocketNotifier::Read, this);
    connect(mNotifierTERM, SIGNAL(activated(int)), this, SLOT(handleSigTERM()), Qt::QueuedConnection);

    mNotifierUSR1 = new QSocketNotifier(UnixSignals::spUSR1.output()->socketDescriptor(), QSocketNotifier::Read, this);
    connect(mNotifierUSR1, SIGNAL(activated(int)), this, SLOT(handleSigUSR1()), Qt::QueuedConnection);

    mNotifierUSR2 = new QSocketNotifier(UnixSignals::spUSR2.output()->socketDescriptor(), QSocketNotifier::Read, this);
    connect(mNotifierUSR2, SIGNAL(activated(int)), this, SLOT(handleSigUSR2()), Qt::QueuedConnection);
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
#endif

    if (receivers(SIGNAL(sigTERM())) > 0) {
        if (signal(SIGTERM, UnixSignals::signalHandler) == SIG_ERR) {
            qFatal("Unable to set signal handler on TERM");
        }
    } else {
        qDebug("No listeners for signal TERM");
    }

#ifdef SIGINT
    if (receivers(SIGNAL(sigINT())) > 0) {
        if (signal(SIGINT, UnixSignals::signalHandler) == SIG_ERR) {
            qFatal("Unable to set signal handler on INT");
        }
    } else {
        qDebug("No listeners for signal INT");
    }
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
#endif
}

void UnixSignals::stop()
{
    UnixSignals::spBREAK.close();
    UnixSignals::spHUP.close();
    UnixSignals::spINT.close();
    UnixSignals::spTERM.close();
    UnixSignals::spUSR1.close();
    UnixSignals::spUSR2.close();
}

void UnixSignals::signalHandler(int number)
{
    char tmp = number;
    qDebug() << "UnixSignals::signalHandler -- pass signal" << number;

    switch (number) {
#ifdef SIGBREAK
    // Available on Windows
    case SIGBREAK:
        UnixSignals::spBREAK.input()->write(&tmp, sizeof(tmp));
        break;
#endif
    case SIGTERM:
        UnixSignals::spTERM.input()->write(&tmp, sizeof(tmp));
        break;
#ifdef SIGHUP
    // Unavailable on Windows
    case SIGHUP:
        UnixSignals::spHUP.input()->write(&tmp, sizeof(tmp));
        break;
#endif
#ifdef SIGINT
    case SIGINT:
        UnixSignals::spINT.input()->write(&tmp, sizeof(tmp));
        break;
#endif
#ifdef SIGUSR1
    // Unavailable on Windows
    case SIGUSR1:
        UnixSignals::spUSR1.input()->write(&tmp, sizeof(tmp));
        break;
#endif
#ifdef SIGUSR2
    // Unavailable on Windows
    case SIGUSR2:
        UnixSignals::spUSR2.input()->write(&tmp, sizeof(tmp));
        break;
#endif
    default:
        qDebug() << "UnixSignals::signalHandler -- Unknown signal:" << number;
        break;
    }
}

void UnixSignals::handleSigBREAK()
{
    mNotifierBREAK->setEnabled(false);

    char number = 0;
    UnixSignals::spBREAK.output()->read(&number, sizeof(number));

    qDebug() << "Got SIGBREAK! emit event!";
    emit sigBREAK();

    mNotifierBREAK->setEnabled(true);
}

void UnixSignals::handleSigHUP()
{
    mNotifierHUP->setEnabled(false);

    char number = 0;
    UnixSignals::spHUP.output()->read(&number, sizeof(number));

    qDebug() << "Got SIGHUP! emit event!";
    emit sigHUP();

    mNotifierHUP->setEnabled(true);
}

void UnixSignals::handleSigINT()
{
    mNotifierINT->setEnabled(false);

    char number = 0;
    UnixSignals::spINT.output()->read(&number, sizeof(number));

    qDebug() << "Got SIGINT! emit event!";
    emit sigINT();

    mNotifierINT->setEnabled(true);
}

void UnixSignals::handleSigTERM()
{
    mNotifierTERM->setEnabled(false);

    char number = 0;
    UnixSignals::spTERM.output()->read(&number, sizeof(number));

    qDebug() << "Got SIGTERM! emit event!";
    emit sigTERM();

    mNotifierTERM->setEnabled(true);
}

void UnixSignals::handleSigUSR1()
{
    mNotifierUSR1->setEnabled(false);

    char number = 0;
    UnixSignals::spUSR1.output()->read(&number, sizeof(number));

    qDebug() << "Got SIGUSR1! emit event!";
    emit sigUSR1();

    mNotifierUSR1->setEnabled(true);
}

void UnixSignals::handleSigUSR2()
{
    mNotifierUSR2->setEnabled(false);

    char number = 0;
    UnixSignals::spUSR2.output()->read(&number, sizeof(number));

    qDebug() << "Got SIGUSR2! emit event!";
    emit sigUSR2();

    mNotifierUSR2->setEnabled(true);
}
