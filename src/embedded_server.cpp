#include "embedded_server.h"
#include <QDebug>

EmbeddedServer::EmbeddedServer( int & argc, char ** argv )
        : QApplication(argc, argv, QApplication::GuiServer)
{
	connect(&startup, SIGNAL(timeout()), this, SLOT(startupSlot()));
	startup.setInterval(1);
	startup.setSingleShot(true);
	startup.start(1);

	process.setProcessChannelMode(QProcess::ForwardedChannels);
	connect(&process, SIGNAL(stateChanged(QProcess::ProcessState)), this, SLOT(stateChangedSlot(QProcess::ProcessState)));
}

void EmbeddedServer::startupSlot()
{
	QStringList cmd(QCoreApplication::arguments());
	qDebug() << "Command line arguments:" << cmd.join(" ");
	// Remove app cmd
	cmd.removeFirst();
	if (cmd.size() and cmd.first() == "-qws") {
		cmd.removeFirst();
	}
	if (!cmd.size()) {
		qDebug() << "No program in commandline arguments to execute!";
		exit(0);
		return;
	}

	QString program = cmd.first();
	cmd.removeFirst();

	qDebug() << "Start:" << endl << "  program:" << program << endl << "  arguments:" << cmd.join(" ");

	process.start(program, cmd);
}

void EmbeddedServer::stateChangedSlot(QProcess::ProcessState newState)
{
	if (newState == QProcess::Running) {
		qDebug() << "Process started...";
	}
	if (newState == QProcess::NotRunning) {
		qDebug() << "Process not running... Exiting...";
		exit(0);
	}
}

void EmbeddedServer::aboutToQuitSlot()
{
	if (process.state() == QProcess::Running) {
		qDebug() << "Process is running... Terminate it.";
		process.terminate();
	}
}
