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

    // QSocketNotifier suck...
    static SocketPair spBREAK;
    static SocketPair spTERM;
    static SocketPair spHUP;
    static SocketPair spINT;
    static SocketPair spUSR1;
    static SocketPair spUSR2;

Q_SIGNALS:
    void sigBREAK();
    void sigTERM();
    void sigHUP();
    void sigINT();
    void sigUSR1();
    void sigUSR2();

private slots:
    void handleSigBREAK();
    void handleSigTERM();
    void handleSigHUP();
    void handleSigINT();
    void handleSigUSR1();
    void handleSigUSR2();

private:

    QSocketNotifier *mNotifierBREAK;
    QSocketNotifier *mNotifierTERM;
    QSocketNotifier *mNotifierHUP;
    QSocketNotifier *mNotifierINT;
    QSocketNotifier *mNotifierUSR1;
    QSocketNotifier *mNotifierUSR2;
};

#endif // UNIXSIGNALS_H
