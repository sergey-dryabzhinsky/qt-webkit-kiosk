#ifndef UNIXSIGNALS_H
#define UNIXSIGNALS_H

#include <QObject>
#include <QSocketNotifier>
#include "socketpair.h"

class UnixSignals : public QObject
{
    Q_OBJECT
public:
    UnixSignals( QObject *parent = 0);
    void start();

    static void signalHandler(int unused);
    static SocketPair sp;

Q_SIGNALS:
    void sigBREAK();
    void sigTERM();
    void sigINT();
    void sigUSR1();
    void sigUSR2();

private slots:
    void handleSig();

private:

    QSocketNotifier *mNotifier;
};

#endif // UNIXSIGNALS_H
