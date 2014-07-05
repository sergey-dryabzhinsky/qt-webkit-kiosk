#ifndef EMBEDDED_SERVER_H
#define EMBEDDED_SERVER_H

/**
 * Qapplication for Embedded GUI server
 */

#include <QApplication>
#include <QTimer>
#include <QProcess>

class EmbeddedServer: public QApplication
{
    Q_OBJECT
public:
    EmbeddedServer( int & argc, char ** argv );

private slots:
    void startupSlot();
	void stateChangedSlot(QProcess::ProcessState newState);
	void aboutToQuitSlot();

private:
    QTimer startup;
	QProcess process;
};

#endif // EMBEDDED_SERVER_H
