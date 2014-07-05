#ifndef UNIXSIGNALS_H
#define UNIXSIGNALS_H

/**
 * Realy ugly QSocketNotifier behaviour...
 * I can't send signal number through socket
 * So I create socket pair for every signal I listen to...
 * Damn...
 */

#include <QObject>
#include <QSocketNotifier>
#include "socketpair.h"

class UnixSignals : public QObject
{
    Q_OBJECT
public:
    UnixSignals( QObject *parent = 0 );

    void start();
    void stop();

    static void signalHandler(int number);

    static SocketPair sockPair;

Q_SIGNALS:
    void sigBREAK();
    void sigTERM();
    void sigHUP();
    void sigINT();
    void sigUSR1();
    void sigUSR2();

private slots:
    void handleSig(QByteArray);

private:
    void handleSignal(int);
};

#endif // UNIXSIGNALS_H
