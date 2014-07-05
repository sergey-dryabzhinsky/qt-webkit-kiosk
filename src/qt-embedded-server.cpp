/****************************************************************************
**
** Copyright (C) 2014 Sergey Dryabzhinsky.
** All rights reserved.
** Contact: Sergey Dryabzhinsky (sergey.dryabzhinsky@gmail.com)
**
** $QT_BEGIN_LICENSE:LGPL$
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** If you have questions regarding the use of this file, please contact
** Sergey at sergey.dryabzhinsky@gmail.com.
** $QT_END_LICENSE$
**
****************************************************************************/

#include "config.h"
#include <QApplication>
#include <QDebug>
#include "embedded_server.h"


int main(int argc, char * argv[])
{
    EmbeddedServer app(argc, argv);
    app.setApplicationName("Simple Qt GUI embedded server");
    app.setApplicationVersion(VERSION);

    QObject::connect(qApp, SIGNAL(aboutToQuit()),
                     &app, SLOT(aboutToQuitSlot()));

    int ret = app.exec();
    qDebug() << "Application return:" << ret;
}
