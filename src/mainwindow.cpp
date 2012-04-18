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

#include "config.h"

#include <QtGui>
#include <QtNetwork>
#include <QtWebKit>
#include <QDebug>
#include "mainwindow.h"

MainWindow::MainWindow()
{
    progress = 0;

    cmdopts = new AnyOption();
    //cmdopts->setVerbose();

    cmdopts->addUsage("This is a simple web-browser working in fullscreen kiosk-mode.");
    cmdopts->addUsage("");
    cmdopts->addUsage("Usage: ");
    cmdopts->addUsage("");
    cmdopts->addUsage(" --help -h                       Print usage and exit");
    cmdopts->addUsage(" --version -V                    Print version and exit");
    cmdopts->addUsage(" --config options.ini            Configuration INI-file");
    cmdopts->addUsage(" --uri http://www.example.com/   Open this URI");
    cmdopts->addUsage("");

    cmdopts->setFlag("help", 'h');
    cmdopts->setFlag("version", 'V');

    cmdopts->setOption("config");
    cmdopts->setOption("uri");

    cmdopts->setVersion(VERSION);

    cmdopts->processCommandArgs( QCoreApplication::argc(), QCoreApplication::argv() );

    if (cmdopts->getFlag('h') || cmdopts->getFlag("help")) {
        qDebug() << ">> Help option in command prompt...";
        cmdopts->printUsage();
        eventExit = new QKeyEvent( QEvent::KeyPress, Qt::Key_Q, Qt::ControlModifier, "Exit", 0 );
        QCoreApplication::postEvent( this, eventExit );
        return;
    }

    if (cmdopts->getFlag('V') || cmdopts->getFlag("version")) {
        qDebug() << ">> Version option in command prompt...";
        cmdopts->printVersion();
        eventExit = new QKeyEvent( QEvent::KeyPress, Qt::Key_Q, Qt::ControlModifier, "Exit", 0 );
        QCoreApplication::postEvent( this, eventExit );
        return;
    }

    if (cmdopts->getValue("config")) {
        qDebug() << ">> Config option in command prompt...";
        loadSettings(QString(cmdopts->getValue("config")));
    } else {
        loadSettings(QString(""));
    }

    qDebug() << "Application icon: " << mainSettings->value("application/icon").toString();

    setWindowIcon(QIcon(
       mainSettings->value("application/icon").toString()
    ));

    if (cmdopts->getValue("uri")) {
        qDebug() << ">> Uri option in command prompt...";
        mainSettings->setValue("browser/homepage", cmdopts->getValue("uri"));
    }

    QCoreApplication::setOrganizationName(
            mainSettings->value("application/organization").toString()
            );
    QCoreApplication::setOrganizationDomain(
            mainSettings->value("application/organization-domain").toString()
            );
    QCoreApplication::setApplicationName(
            mainSettings->value("application/name").toString()
            );
    QCoreApplication::setApplicationVersion(
            mainSettings->value("application/version").toString()
            );

    // --- Network --- //

    if (mainSettings->value("proxy/enable").toBool()) {
        bool system = mainSettings->value("proxy/system").toBool();
        if (system) {
            QNetworkProxyFactory::setUseSystemConfiguration(system);
        } else {
            QNetworkProxy proxy;
            proxy.setType(QNetworkProxy::HttpProxy);
            proxy.setHostName(
                    mainSettings->value("proxy/host").toString()
            );
            proxy.setPort(mainSettings->value("proxy/port").toUInt());
            if (mainSettings->value("proxy/auth").toBool()) {
                proxy.setUser(mainSettings->value("proxy/username").toString());
                proxy.setPassword(mainSettings->value("proxy/password").toString());
            }
            QNetworkProxy::setApplicationProxy(proxy);
        }
    }

    if (!mainSettings->value("printing/show-printer-dialog").toBool()) {
        printer = new QPrinter(QPrinter::ScreenResolution);
        printer->setPrinterName(mainSettings->value("printing/printer").toString());
        printer->setPageMargins(
            mainSettings->value("printing/page_margin_left").toReal(),
            mainSettings->value("printing/page_margin_top").toReal(),
            mainSettings->value("printing/page_margin_right").toReal(),
            mainSettings->value("printing/page_margin_bottom").toReal(),
            QPrinter::Millimeter
        );
    }

    // --- Web View --- //

    view = new WebView(this);
    view->setSettings(mainSettings);

    view->page()->settings()->setAttribute(QWebSettings::JavascriptEnabled,
        mainSettings->value("browser/javascript").toBool()
    );
    view->page()->settings()->setAttribute(QWebSettings::JavaEnabled,
        mainSettings->value("browser/java").toBool()
    );
    view->page()->settings()->setAttribute(QWebSettings::PluginsEnabled,
        mainSettings->value("browser/plugins").toBool()
    );


    // --- Disk cache --- //
    if (mainSettings->value("cache/enable").toBool()) {
        diskCache = new QNetworkDiskCache(this);
        QString location = mainSettings->value("cache/location").toString();
        if (!location.length()) {
            location = QDesktopServices::storageLocation(QDesktopServices::CacheLocation);
        }
        diskCache->setCacheDirectory(location);
        diskCache->setMaximumCacheSize(mainSettings->value("cache/size").toUInt());

        view->page()->networkAccessManager()->setCache(diskCache);
    }

    connect(view, SIGNAL(titleChanged(QString)), SLOT(adjustTitle()));
    connect(view, SIGNAL(loadProgress(int)), SLOT(setProgress(int)));
    connect(view, SIGNAL(loadFinished(bool)), SLOT(finishLoading(bool)));
    connect(view, SIGNAL(iconChanged()), SLOT(pageIconLoaded()));

    connect(view->page(), SIGNAL(printRequested(QWebFrame*)), SLOT(printRequested(QWebFrame*)));

    connect(QApplication::desktop(), SIGNAL(resized(int)), SLOT(desktopResized(int)));

    show();

    setCentralWidget(view);

    view->page()->view()->setFocusPolicy(Qt::StrongFocus);
    view->setFocusPolicy(Qt::StrongFocus);
    this->setFocusPolicy(Qt::StrongFocus);

    if (mainSettings->value("view/fullscreen").toBool()) {
        showFullScreen();
    } else if (mainSettings->value("view/maximized").toBool()) {
        showMaximized();
    } else if (mainSettings->value("view/fixed-size").toBool()) {
        centerFixedSizeWindow();
    }

    QFileInfo finfo = QFileInfo();
    finfo.setFile(mainSettings->value("browser/homepage").toString());

    qDebug() << "Homepage: like file = " <<
                mainSettings->value("browser/homepage").toString() <<
                ", absolute path = " <<
                finfo.absoluteFilePath() <<
                ", local uri = " <<
                QUrl::fromLocalFile(
                    mainSettings->value("browser/homepage").toString()
                ).toString();

    if (finfo.isFile()) {
        qDebug() << "Homepage: Local FILE!";
        view->load(QUrl::fromLocalFile(
            finfo.absoluteFilePath()
        ));
    } else {
        qDebug() << "Homepage: Remote URI!";
        view->load(QUrl(
            mainSettings->value("browser/homepage").toString()
        ));
    }
}


void MainWindow::centerFixedSizeWindow()
{
    quint16 widowWidth = mainSettings->value("view/fixed-width").toUInt();
    quint16 widowHeight = mainSettings->value("view/fixed-height").toUInt();

    quint16 screenWidth = QApplication::desktop()->screenGeometry().width();
    quint16 screenHeight = QApplication::desktop()->screenGeometry().height();

    qDebug() << "Screen size: " << screenWidth << "x" << screenHeight;

    quint16 x = 0;
    quint16 y = 0;

    if (mainSettings->value("view/fixed-centered").toBool()) {
        x = (screenWidth - widowWidth) / 2;
        y = (screenHeight - widowHeight) / 2;
    } else {
        x = mainSettings->value("view/fixed-x").toUInt();
        y = mainSettings->value("view/fixed-y").toUInt();
    }

    qDebug() << "Move window to: (" << x << ";" << y << ")";

    move ( x, y );
    setFixedSize( widowWidth, widowHeight );

}


void MainWindow::keyPressEvent(QKeyEvent *event)
{
    switch (event->key()) {
    case Qt::Key_Q:
        if (int(event->modifiers()) == Qt::CTRL) {
            QApplication::exit(0);
        }
        break;
    case Qt::Key_F11:
        if (isFullScreen()) {
            showNormal();
        } else {
            showFullScreen();
        }
        break;
    default:
        QMainWindow::keyPressEvent(event);
    }
}

void MainWindow::loadSettings(QString ini_file)
{
    if (!ini_file.length()) {
        ini_file = RESOURCES"qt-webkit-kiosk.ini";
        qDebug() << "Ini file: " << ini_file;
    }
    mainSettings = new QSettings(ini_file, QSettings::IniFormat, this);

    if (!mainSettings->contains("application/organization")) {
        mainSettings->setValue("application/organization", "Organization" );
    }
    if (!mainSettings->contains("application/organization-domain")) {
        mainSettings->setValue("application/organization-domain", "www.example.com" );
    }
    if (!mainSettings->contains("application/name")) {
        mainSettings->setValue("application/name", "QtWebkitKiosk" );
    }
    if (!mainSettings->contains("application/version")) {
        mainSettings->setValue("application/version", VERSION );
    }
    if (!mainSettings->contains("application/icon")) {
        mainSettings->setValue("application/icon", ICON );
    }

    if (!mainSettings->contains("proxy/enable")) {
        mainSettings->setValue("proxy/enable", false);
    }
    if (!mainSettings->contains("proxy/system")) {
        mainSettings->setValue("proxy/system", true);
    }
    if (!mainSettings->contains("proxy/host")) {
        mainSettings->setValue("proxy/host", "proxy.example.com");
    }
    if (!mainSettings->contains("proxy/port")) {
        mainSettings->setValue("proxy/port", 3128);
    }
    if (!mainSettings->contains("proxy/auth")) {
        mainSettings->setValue("proxy/auth", false);
    }
    if (!mainSettings->contains("proxy/username")) {
        mainSettings->setValue("proxy/username", "username");
    }
    if (!mainSettings->contains("proxy/password")) {
        mainSettings->setValue("proxy/password", "password");
    }

    if (!mainSettings->contains("view/fullscreen")) {
        mainSettings->setValue("view/fullscreen", true);
    }
    if (!mainSettings->contains("view/maximized")) {
        mainSettings->setValue("view/maximized", false);
    }
    if (!mainSettings->contains("view/fixed-size")) {
        mainSettings->setValue("view/fixed-size", false);
    }
    if (!mainSettings->contains("view/fixed-width")) {
        mainSettings->setValue("view/fixed-width", 800);
    }
    if (!mainSettings->contains("view/fixed-height")) {
        mainSettings->setValue("view/fixed-height", 600);
    }
    if (!mainSettings->contains("view/fixed-centered")) {
        mainSettings->setValue("view/fixed-centered", true);
    }
    if (!mainSettings->contains("view/fixed-x")) {
        mainSettings->setValue("view/fixed-x", 0);
    }
    if (!mainSettings->contains("view/fixed-y")) {
        mainSettings->setValue("view/fixed-y", 0);
    }


    if (!mainSettings->contains("browser/homepage")) {
        mainSettings->setValue("browser/homepage", RESOURCES"default.html");
    }
    if (!mainSettings->contains("browser/javascript")) {
        mainSettings->setValue("browser/javascript", true);
    }
    if (!mainSettings->contains("browser/java")) {
        mainSettings->setValue("browser/java", false);
    }
    if (!mainSettings->contains("browser/plugins")) {
        mainSettings->setValue("browser/plugins", true);
    }


    if (!mainSettings->contains("event-sounds/enable")) {
        mainSettings->setValue("event-sounds/enable", false);
    }
    if (!mainSettings->contains("event-sounds/window-clicked")) {
        mainSettings->setValue("event-sounds/window-clicked", RESOURCES"window-clicked.ogg");
    }

    if (!mainSettings->contains("cache/enable")) {
        mainSettings->setValue("cache/enable", true);
    }
    if (!mainSettings->contains("cache/location")) {
        QString location = QDesktopServices::storageLocation(QDesktopServices::CacheLocation);
        QDir d = QDir(location);
        location += d.separator();
        location += mainSettings->value("application/name").toString();
        d.setPath(location);
        mainSettings->setValue("cache/location", d.absolutePath());
    }
    if (!mainSettings->contains("cache/size")) {
        mainSettings->setValue("cache/size", 50*1024*1024);
    }

    if (!mainSettings->contains("printing/show-printer-dialog")) {
        mainSettings->setValue("printing/show-printer-dialog", false);
    }
    if (!mainSettings->contains("printing/printer")) {
        mainSettings->setValue("printing/printer", "default");
    }
    if (!mainSettings->contains("printing/page_margin_left")) {
        mainSettings->setValue("printing/page_margin_left", 5);
    }
    if (!mainSettings->contains("printing/page_margin_top")) {
        mainSettings->setValue("printing/page_margin_top", 5);
    }
    if (!mainSettings->contains("printing/page_margin_right")) {
        mainSettings->setValue("printing/page_margin_right", 5);
    }
    if (!mainSettings->contains("printing/page_margin_bottom")) {
        mainSettings->setValue("printing/page_margin_bottom", 5);
    }

    if (!mainSettings->contains("attach/javascripts")) {
        mainSettings->setValue("attach/javascripts", "");
    }
    if (!mainSettings->contains("attach/styles")) {
        mainSettings->setValue("attach/styles", "");
    }

    mainSettings->sync();
}

void MainWindow::adjustTitle()
{
    if (progress <= 0 || progress >= 100) {
        setWindowTitle(view->title());
    } else {
        setWindowTitle(QString("%1 (%2%)").arg(view->title()).arg(progress));
    }
}

void MainWindow::setProgress(int p)
{
    progress = p;
    adjustTitle();
}

void MainWindow::desktopResized(int p)
{
    qDebug() << "Desktop resized event: p=" << p;
    if (mainSettings->value("view/fullscreen").toBool()) {
        showFullScreen();
    } else if (mainSettings->value("view/maximized").toBool()) {
        showMaximized();
    } else if (mainSettings->value("view/fixed-size").toBool()) {
        centerFixedSizeWindow();
    }
}

void MainWindow::finishLoading(bool)
{
    progress = 100;
    adjustTitle();

    attachStyles();
    attachJavascripts();
}

void MainWindow::attachJavascripts()
{
    if (!mainSettings->contains("attach/javascripts")) {
        return;
    }
    QStringList scripts = mainSettings->value("attach/javascripts").toStringList();
    if (!scripts.length()) {
        return;
    }
    qDebug() << "Page loaded, attach" << scripts.length() << " user javascript files...";
    QStringListIterator scriptsIterator(scripts);
    QFileInfo finfo = QFileInfo();
    QString file_name;
    while (scriptsIterator.hasNext()) {
        file_name = scriptsIterator.next();
        qDebug() << "-- attach " << file_name;

        finfo.setFile(file_name);
        if (finfo.isFile()) {
            qDebug() << "-- it's local file";
            QFile f(file_name);
            QString content = QString(f.readAll()) + " null";
            view->page()->mainFrame()->evaluateJavaScript(content);
        } else {
            qDebug() << "-- it's remote file";
            QString content = "<script type=\"text/javascript\" src=\"";
            content += file_name;
            content += "\"></script>\n";
            view->page()->mainFrame()->findFirstElement("head").appendInside(content);
        }
    }
}

void MainWindow::attachStyles()
{
    if (!mainSettings->contains("attach/styles")) {
        return;
    }
    QStringList styles = mainSettings->value("attach/styles").toStringList();
    if (!styles.length()) {
        return;
    }
    qDebug() << "Page loaded, attach" << styles.length() << " user style files...";
    QStringListIterator stylesIterator(styles);
    QString file_name;
    QFileInfo finfo = QFileInfo();
    while (stylesIterator.hasNext()) {
        file_name = stylesIterator.next();
        qDebug() << "-- attach " << file_name;

        finfo.setFile(file_name);

        QString content;
        if (finfo.isFile()) {
            qDebug() << "-- it's local file";
            QFile f(file_name);
            content = "<style type=\"text/css\">\n";
            content += QString(f.readAll());
            content += "</style>\n";
        } else {
            qDebug() << "-- it's remote file";
            content = "<link type=\"text/css\" rel=\"stylesheet\" href=\"";
            content += file_name;
            content += "\"/>\n";
        }
        view->page()->mainFrame()->findFirstElement("head").appendInside(content);
    }
}


void MainWindow::pageIconLoaded()
{
    setWindowIcon(view->icon());
}

void MainWindow::printRequested(QWebFrame *wf)
{
    if (!mainSettings->value("printing/show-printer-dialog").toBool()) {
        if (printer->printerState() != QPrinter::Error) {
            wf->print(printer);
        }
    }
}
