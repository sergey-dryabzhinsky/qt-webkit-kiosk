/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial Usage
** Licensees holding valid Qt Commercial licenses may use this file in
** accordance with the Qt Commercial License Agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Nokia.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights.  These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** If you have questions regarding the use of this file, please contact
** Nokia at qt-info@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/

#include <signal.h>
#include "mainwindow.h"
#include <QApplication>

static void unixSignalHandler(int signum) {
    qDebug("DBG: main.cpp::unixSignalHandler(). signal = %d\n", signum);

    /*
     * Make sure your Qt application gracefully quits.
     * NOTE - purpose for calling qApp->exit(0):
     *      1. Forces the Qt framework's "main event loop `qApp->exec()`" to quit looping.
     *      2. Also emits the QCoreApplication::aboutToQuit() signal. This signal is used for cleanup code.
     */
    qApp->exit(0);
}

int main(int argc, char * argv[])
{
    QApplication app(argc, argv);
    MainWindow *browser = new MainWindow();

    if (signal(SIGINT, unixSignalHandler) == SIG_ERR) {
        qFatal("ERR - %s(%d): An error occurred while setting a signal handler.\n", __FILE__,__LINE__);
    }
    if (signal(SIGTERM, unixSignalHandler) == SIG_ERR) {
        qFatal("ERR - %s(%d): An error occurred while setting a signal handler.\n", __FILE__,__LINE__);
    }
    // executes browser.cleanupSlot() when the Qt framework emits aboutToQuit() signal.
    QObject::connect(qApp,          SIGNAL(aboutToQuit()),
                     browser,   SLOT(cleanupSlot()));

    return app.exec();
}
