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

#include <signal.h>
#include <QtGui>
#include <QtNetwork>
#include <QtWebKit>
#include <QDebug>
#include "mainwindow.h"
#include <QStandardPaths>
#include <QApplication>
#include <QDesktopWidget>
#include <QtWebKitWidgets/QWebFrame>
#include "cachingnm.h"

MainWindow::MainWindow() : QMainWindow()
{
    progress = 0;
    diskCache = NULL;
    mainSettings = NULL;

    handler = new UnixSignals();
    connect(handler, SIGNAL(sigBREAK()), SLOT(unixSignalQuit()));
    connect(handler, SIGNAL(sigTERM()), SLOT(unixSignalQuit()));
    connect(handler, SIGNAL(sigINT()), SLOT(unixSignalQuit()));
    connect(handler, SIGNAL(sigHUP()), SLOT(unixSignalHup()));

    delayedResize = new QTimer();
    delayedLoad = new QTimer();
}

void MainWindow::init(AnyOption *opts)
{
    cmdopts = opts;

    if (cmdopts->getValue("config") || cmdopts->getValue('c')) {
        qDebug(">> Config option in command prompt...");
        QString cfgPath = cmdopts->getValue('c');
        if (cfgPath.isEmpty()) {
            cfgPath = cmdopts->getValue("config");
        }
        loadSettings(cfgPath);
    } else {
        loadSettings(QString(""));
    }

    if (mainSettings->value("signals/enable").toBool()) {
        connect(handler, SIGNAL(sigUSR1()), SLOT(unixSignalUsr1()));
        connect(handler, SIGNAL(sigUSR2()), SLOT(unixSignalUsr2()));
    }
    handler->start();

    setMinimumWidth(320);
    setMinimumHeight(200);

    quint16 minimalWidth = mainSettings->value("view/minimal-width").toUInt();
    quint16 minimalHeight = mainSettings->value("view/minimal-height").toUInt();
    if (minimalWidth) {
        setMinimumWidth(minimalWidth);
    }
    if (minimalHeight) {
        setMinimumHeight(minimalHeight);
    }

    qDebug() << "Application icon: " << mainSettings->value("application/icon").toString();
    setWindowIcon(QIcon(
       mainSettings->value("application/icon").toString()
    ));

    if (cmdopts->getValue("uri") || cmdopts->getValue('u')) {
        qDebug(">> Uri option in command prompt...");
        QString uri = cmdopts->getValue('u');
        if (uri.isEmpty()) {
            uri = cmdopts->getValue("uri");
        }
        mainSettings->setValue("browser/homepage", uri);
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

    // --- Web View --- //
    view = new WebView(this);

    // --- Progress Bar --- //
    loadProgress = new QProgressBar(view);
    loadProgress->setContentsMargins(2, 2, 2, 2);
    loadProgress->setMinimumWidth(100);
    loadProgress->setMinimumHeight(16);
    loadProgress->setFixedHeight(16);
    loadProgress->setAutoFillBackground(true);
    QPalette palette = this->palette();
    palette.setColor(QPalette::Window, QColor(255,255,255,63));
    loadProgress->setPalette(palette);

    // Do not work... Need Layout...
    loadProgress->setAlignment(Qt::AlignTop);
    loadProgress->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

    loadProgress->hide();

    setCentralWidget(view);

    view->setSettings(mainSettings);
    view->setPage(new QWebPage(view));

    // --- Disk cache --- //
    if (mainSettings->value("cache/enable").toBool()) {
        diskCache = new QNetworkDiskCache(this);
        QString location = mainSettings->value("cache/location").toString();
        if (!location.length()) {
            location = QStandardPaths::writableLocation(QStandardPaths::CacheLocation);
        }
        diskCache->setCacheDirectory(location);
        diskCache->setMaximumCacheSize(mainSettings->value("cache/size").toUInt());

        if (mainSettings->value("cache/clear-on-start").toBool()) {
            diskCache->clear();
        }
        else if (cmdopts->getFlag('C') || cmdopts->getFlag("clear-cache")) {
            diskCache->clear();
        }

        CachingNetworkManager *nm = new CachingNetworkManager();
        nm->setCache(diskCache);
        view->page()->setNetworkAccessManager(nm);
    }

    view->settings()->setAttribute(QWebSettings::JavascriptEnabled,
        mainSettings->value("browser/javascript").toBool()
    );

    view->settings()->setAttribute(QWebSettings::JavascriptCanOpenWindows,
        mainSettings->value("browser/javascript_can_open_windows").toBool()
    );

    view->settings()->setAttribute(QWebSettings::JavascriptCanCloseWindows,
        mainSettings->value("browser/javascript_can_close_windows").toBool()
    );

    view->settings()->setAttribute(QWebSettings::WebGLEnabled,
        mainSettings->value("browser/webgl").toBool()
    );

    view->settings()->setAttribute(QWebSettings::JavaEnabled,
        mainSettings->value("browser/java").toBool()
    );
    view->settings()->setAttribute(QWebSettings::PluginsEnabled,
        mainSettings->value("browser/plugins").toBool()
    );

    if (mainSettings->value("inspector/enable").toBool()) {
        view->settings()->setAttribute(QWebSettings::DeveloperExtrasEnabled, true);

        inspector = new QWebInspector();
        inspector->setVisible(mainSettings->value("inspector/visible").toBool());
        inspector->setMinimumSize(800, 600);
        inspector->setWindowTitle(mainSettings->value("application/name").toString() + " - WebInspector");
        inspector->setWindowIcon(this->windowIcon());
        inspector->setPage(view->page());
    }

    connect(view, SIGNAL(titleChanged(QString)), SLOT(adjustTitle()));
    connect(view, SIGNAL(loadStarted()), SLOT(startLoading()));
    connect(view, SIGNAL(urlChanged(const QUrl &)), SLOT(urlChanged(const QUrl &)));
    connect(view, SIGNAL(loadProgress(int)), SLOT(setProgress(int)));
    connect(view, SIGNAL(loadFinished(bool)), SLOT(finishLoading(bool)));
    connect(view, SIGNAL(iconChanged()), SLOT(pageIconLoaded()));

    QDesktopWidget *desktop = QApplication::desktop();
    connect(desktop, SIGNAL(resized(int)), SLOT(desktopResized(int)));

    show();

    view->page()->view()->setFocusPolicy(Qt::StrongFocus);
    view->setFocusPolicy(Qt::StrongFocus);

    int delay_resize = 0;
    if (mainSettings->value("view/startup_resize_delayed").toBool()) {
        delay_resize = mainSettings->value("view/startup_resize_delay").toInt();
    }
    delayedResize->singleShot(delay_resize, this, SLOT(delayedWindowResize()));

    int delay_load = 0;
    if (mainSettings->value("browser/startup_load_delayed").toBool()) {
        delay_load = mainSettings->value("browser/startup_load_delay").toInt();
    }
    delayedLoad->singleShot(delay_load, this, SLOT(delayedPageLoad()));

}

void MainWindow::delayedWindowResize()
{
    if (mainSettings->value("view/fullscreen").toBool()) {
        showFullScreen();
    } else if (mainSettings->value("view/maximized").toBool()) {
        showMaximized();
    } else if (mainSettings->value("view/fixed-size").toBool()) {
        centerFixedSizeWindow();
    }

    this->setFocusPolicy(Qt::StrongFocus);
    this->focusWidget();

    if (mainSettings->value("view/stay_on_top").toBool()) {
        setWindowFlags(Qt::FramelessWindowHint|Qt::WindowStaysOnTopHint);
    }
}

void MainWindow::delayedPageLoad()
{
    view->loadHomepage();
}

void MainWindow::clearCache()
{
    if (mainSettings->value("cache/enable").toBool()) {
        if (diskCache) {
            diskCache->clear();
        }
    }
}

void MainWindow::clearCacheOnExit()
{
    if (mainSettings->value("cache/enable").toBool()) {
        if (mainSettings->value("cache/clear-on-exit").toBool()) {
            if (diskCache) {
                diskCache->clear();
            }
        }
    }
}

void MainWindow::cleanupSlot()
{
    qDebug("Cleanup Slot (application exit)");
    handler->stop();
    clearCacheOnExit();
    QWebSettings::clearMemoryCaches();
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
    if (mainSettings->value("browser/disable_hotkeys").toBool()) {
        QMainWindow::keyPressEvent(event);
        return;
    }

    switch (event->key()) {
    case Qt::Key_Up:
        view->scrollUp();
        break;
    case Qt::Key_Down:
        view->scrollDown();
        break;
    case Qt::Key_PageUp:
        view->scrollPageUp();
        break;
    case Qt::Key_PageDown:
        view->scrollPageDown();
        break;
    case Qt::Key_End:
        view->scrollEnd();
        break;
    case Qt::Key_HomePage:
        view->loadHomepage();
        break;
    case Qt::Key_Home:
        if (int(event->modifiers()) == Qt::CTRL) {
            view->loadHomepage();
        } else {
            view->scrollHome();
        }
        break;
    case Qt::Key_Q:
        if (int(event->modifiers()) == Qt::CTRL) {
            clearCacheOnExit();
            QApplication::exit(0);
        }
        break;
    case Qt::Key_R:
        if (int(event->modifiers()) == Qt::CTRL) {
            clearCache();
            view->reload();
        }
        break;
    case Qt::Key_F5:
        view->reload();
        break;
    case Qt::Key_F12:
        if (mainSettings->value("inspector/enable").toBool()) {
            if (!inspector->isVisible()) {
                inspector->setVisible(true);
            } else {
                inspector->setVisible(false);
            }
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

/**
 * @TODO: move to separate class
 *
 * @brief MainWindow::loadSettings
 * @param ini_file
 */
void MainWindow::loadSettings(QString ini_file)
{
    if (!ini_file.length()) {
        mainSettings = new QSettings(QSettings::IniFormat, QSettings::UserScope, "QtWebkitKiosk", "config", this);
    } else {
        mainSettings = new QSettings(ini_file, QSettings::IniFormat, this);
    }
    qDebug() << "Ini file: " << mainSettings->fileName();

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
    if (!mainSettings->contains("view/minimal-width")) {
        mainSettings->setValue("view/minimal-width", 320);
    }
    if (!mainSettings->contains("view/minimal-height")) {
        mainSettings->setValue("view/minimal-height", 200);
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

    if (!mainSettings->contains("view/startup_resize_delayed")) {
        mainSettings->setValue("view/startup_resize_delayed", false);
    }
    if (!mainSettings->contains("view/startup_resize_delay")) {
        mainSettings->setValue("view/startup_resize_delay", 2000);
    }

    if (!mainSettings->contains("view/hide_scrollbars")) {
        mainSettings->setValue("view/hide_scrollbars", false);
    }

    if (!mainSettings->contains("view/stay_on_top")) {
        mainSettings->setValue("view/stay_on_top", false);
    }

    if (!mainSettings->contains("view/disable_selection")) {
        mainSettings->setValue("view/disable_selection", true);
    }

    if (!mainSettings->contains("view/show_load_progress")) {
        mainSettings->setValue("view/show_load_progress", true);
    }

    if (!mainSettings->contains("view/page_scale")) {
        mainSettings->setValue("view/page_scale", 1.0);
    }


    if (!mainSettings->contains("browser/homepage")) {
        mainSettings->setValue("browser/homepage", RESOURCES"default.html");
    }
    if (!mainSettings->contains("browser/javascript")) {
        mainSettings->setValue("browser/javascript", true);
    }
    if (!mainSettings->contains("browser/javascript_can_open_windows")) {
        mainSettings->setValue("browser/javascript_can_open_windows", false);
    }
    if (!mainSettings->contains("browser/javascript_can_close_windows")) {
        mainSettings->setValue("browser/javascript_can_close_windows", false);
    }
    if (!mainSettings->contains("browser/webgl")) {
        mainSettings->setValue("browser/webgl", false);
    }
    if (!mainSettings->contains("browser/java")) {
        mainSettings->setValue("browser/java", false);
    }
    if (!mainSettings->contains("browser/plugins")) {
        mainSettings->setValue("browser/plugins", true);
    }
    // Don't break on SSL errors
    if (!mainSettings->contains("browser/ignore_ssl_errors")) {
        mainSettings->setValue("browser/ignore_ssl_errors", true);
    }
    // Show default homepage if window closed by javascript
    if (!mainSettings->contains("browser/show_homepage_on_window_close")) {
        mainSettings->setValue("browser/show_homepage_on_window_close", true);
    }

    if (!mainSettings->contains("browser/startup_load_delayed")) {
        mainSettings->setValue("browser/startup_load_delayed", false);
    }
    if (!mainSettings->contains("browser/startup_load_delay")) {
        mainSettings->setValue("browser/startup_load_delay", 100);
    }

    if (!mainSettings->contains("browser/disable_hotkeys")) {
        mainSettings->setValue("browser/disable_hotkeys", false);
    }


    if (!mainSettings->contains("signals/enable")) {
        mainSettings->setValue("signals/enable", true);
    }
    if (!mainSettings->contains("signals/SIGUSR1")) {
        mainSettings->setValue("signals/SIGUSR1", "");
    }
    if (!mainSettings->contains("signals/SIGUSR2")) {
        mainSettings->setValue("signals/SIGUSR2", "");
    }


    if (!mainSettings->contains("inspector/enable")) {
        mainSettings->setValue("inspector/enable", false);
    }
    if (!mainSettings->contains("inspector/visible")) {
        mainSettings->setValue("inspector/visible", false);
    }


    if (!mainSettings->contains("event-sounds/enable")) {
        mainSettings->setValue("event-sounds/enable", false);
    }
    if (!mainSettings->contains("event-sounds/window-clicked")) {
        mainSettings->setValue("event-sounds/window-clicked", RESOURCES"window-clicked.ogg");
    }
    if (!mainSettings->contains("event-sounds/link-clicked")) {
        mainSettings->setValue("event-sounds/link-clicked", RESOURCES"window-clicked.ogg");
    }

    if (!mainSettings->contains("cache/enable")) {
        mainSettings->setValue("cache/enable", false);
    }
    if (!mainSettings->contains("cache/location")) {
        QString location = QStandardPaths::writableLocation(QStandardPaths::CacheLocation);
        QDir d = QDir(location);
        location += d.separator();
        location += mainSettings->value("application/name").toString();
        d.setPath(location);
        mainSettings->setValue("cache/location", d.absolutePath());
    }
    if (!mainSettings->contains("cache/size")) {
        mainSettings->setValue("cache/size", 100*1000*1000);
    }
    if (!mainSettings->contains("cache/clear-on-start")) {
        mainSettings->setValue("cache/clear-on-start", false);
    }
    if (!mainSettings->contains("cache/clear-on-exit")) {
        mainSettings->setValue("cache/clear-on-exit", false);
    }

    if (!mainSettings->contains("printing/enable")) {
        mainSettings->setValue("printing/enable", false);
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

void MainWindow::desktopResized(int p)
{
    qDebug() << "Desktop resized event: " << p;
    if (mainSettings->value("view/fullscreen").toBool()) {
        showFullScreen();
    } else if (mainSettings->value("view/maximized").toBool()) {
        showMaximized();
    } else if (mainSettings->value("view/fixed-size").toBool()) {
        centerFixedSizeWindow();
    }
}

void MainWindow::startLoading()
{
    progress = 0;
    isScrollBarsHidden = false;
    isSelectionDisabled = false;

    adjustTitle();

    QWebSettings::clearMemoryCaches();

    if (mainSettings->value("view/show_load_progress").toBool()) {
        loadProgress->show();
    }

    if (mainSettings->contains("view/page_scale")) {
        view->page()->mainFrame()->setZoomFactor(mainSettings->value("view/page_scale").toReal());
    }

    qDebug("Start loading...");
}

void MainWindow::setProgress(int p)
{
    progress = p;
    adjustTitle();

    qDebug() << "Loading progress: " << p;

    if (mainSettings->value("view/show_load_progress").toBool()) {
        loadProgress->setValue(p);
    }

    // 1. Hide scrollbars (and add some styles)
    // If there complete head and body start loaded...
    if (!isScrollBarsHidden) {
        isScrollBarsHidden = hideScrollbars();
    }
    if (!isSelectionDisabled) {
        isSelectionDisabled = disableSelection();
    }
}

void MainWindow::urlChanged(const QUrl &url)
{
    qDebug() << "URL changes: " << url.toString();

    // Where is a real change in webframe! Drop flags.
    isScrollBarsHidden = false;
    isSelectionDisabled = false;

    // This is real link clicked
    view->playSound("event-sounds/link-clicked");
}

void MainWindow::finishLoading(bool)
{
    qDebug("Finish loading...");

    progress = 100;
    adjustTitle();

    if (mainSettings->value("view/show_load_progress").toBool()) {
        loadProgress->hide();
    }

    // 1. Hide scrollbars (and add some styles)
    if (!isScrollBarsHidden) {
        isScrollBarsHidden = hideScrollbars();
    }
    if (!isSelectionDisabled) {
        isSelectionDisabled = disableSelection();
    }
    // 2. Add more styles which can override previous styles...
    attachStyles();
    attachJavascripts();
}

/**
 * @return bool - true - if don't need to hide scrollbars or hide them, false - if need, but there is no html body loaded.
 * @brief MainWindow::hideScrollbars
 */
bool MainWindow::hideScrollbars()
{
    if (mainSettings->value("view/hide_scrollbars").toBool()) {
        qDebug("Try to hide scrollbars...");

        view->page()->mainFrame()->setScrollBarPolicy( Qt::Vertical, Qt::ScrollBarAlwaysOff );
        view->page()->mainFrame()->setScrollBarPolicy( Qt::Horizontal, Qt::ScrollBarAlwaysOff );
    }

    return true;
}

bool MainWindow::disableSelection()
{
    if (mainSettings->value("view/disable_selection").toBool()) {
        qDebug("Try to disable text selection...");

        // Then webkit loads page and it's "empty" - empty html DOM loaded...
        // So we wait before real page DOM loaded...
        QWebElement bodyElem = view->page()->mainFrame()->findFirstElement("body");
        if (!bodyElem.isNull() && !bodyElem.toInnerXml().trimmed().isEmpty()) {
            QWebElement headElem = view->page()->mainFrame()->findFirstElement("head");
            if (headElem.isNull() || headElem.toInnerXml().trimmed().isEmpty()) {
                qDebug("... html head not loaded ... wait...");
                return false;
            }

            //qDebug() << "... head element content:\n" << headElem.toInnerXml();

            // http://stackoverflow.com/a/5313735
            QString content;
            content = "<style type=\"text/css\">\n";
            content += "body, div, p, span, h1, h2, h3, h4, h5, h6, caption, td {\n";
            content += " -moz-user-select: none;\n";
            content += " -khtml-user-select: none;\n";
            content += " -webkit-user-select: none;\n";
            content += " user-select: none;\n";
            content += " }\n";
            content += "</style>\n";

            // Ugly hack, but it's works...
            if (!headElem.toInnerXml().contains(content)) {
                headElem.setInnerXml(headElem.toInnerXml() + content);
                qDebug("... html head loaded ... hack inserted...");
            } else {
                qDebug("... html head loaded ... hack already inserted...");
            }

            //headElem = view->page()->mainFrame()->findFirstElement("head");
            //qDebug() << "... head element content after:\n" << headElem.toInnerXml() ;

        } else {
            qDebug("... html body not loaded ... wait...");
            return false;
        }
    }

    return true;
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
    QStringListIterator scriptsIterator(scripts);
    QFileInfo finfo = QFileInfo();
    QString file_name;
    quint32 countScripts = 0;

    QWebElement bodyElem = view->page()->mainFrame()->findFirstElement("body");
    QString content = "";

    while (scriptsIterator.hasNext()) {
        file_name = scriptsIterator.next();

        if (!file_name.trimmed().length()) continue;

        qDebug() << "-- attach " << file_name;

        countScripts++;


        finfo.setFile(file_name);
        if (finfo.isFile()) {
            qDebug("-- it's local file");
            QFile f(file_name);
            content += "\n<script type=\"text/javascript\">";
            content += QString(f.readAll());
            content += "</script>\n";
            f.close();
        } else {
            qDebug("-- it's remote file");
            content += "\n<script type=\"text/javascript\" src=\"";
            content += file_name;
            content += "\"></script>\n";
        }
    }
    if (countScripts > 0 && content.trimmed().length() > 0) {
        bodyElem.setInnerXml(bodyElem.toInnerXml() + content);

        qDebug() << "Page loaded, found " << countScripts << " user javascript files...";
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
    QStringListIterator stylesIterator(styles);
    QString file_name;
    QFileInfo finfo = QFileInfo();
    quint32 countStyles = 0;

    QWebElement headElem = view->page()->mainFrame()->findFirstElement("head");
    QString content = "";

    while (stylesIterator.hasNext()) {
        file_name = stylesIterator.next();

        if (!file_name.trimmed().length()) continue;

        qDebug() << "-- attach " << file_name;
        countStyles++;

        finfo.setFile(file_name);

        if (finfo.isFile()) {
            qDebug("-- it's local file");
            QFile f(file_name);
            content += "\n<style type=\"text/css\">\n";
            content += QString(f.readAll());
            content += "</style>\n";
            f.close();
        } else {
            qDebug("-- it's remote file");
            content += "\n<link type=\"text/css\" rel=\"stylesheet\" href=\"";
            content += file_name;
            content += "\"/>\n";
        }
    }

    if (countStyles > 0 && content.trimmed().length() > 0) {
        headElem.setInnerXml(headElem.toInnerXml() + content);

        qDebug() << "Page loaded, found " << countStyles << " user style files...";
    }
}


void MainWindow::pageIconLoaded()
{
    setWindowIcon(view->icon());
}

// ----------------------- SIGNALS -----------------------------

/**
 * Force quit on Unix SIGTERM or SIGINT signals
 * @brief MainWindow::unixSignalQuit
 */
void MainWindow::unixSignalQuit()
{
    // No cache clean - quick exit
    qDebug(">> Quit Signal catched. Exiting...");
    QApplication::exit(0);
}

/**
 * Do something on Unix SIGHUP signal
 * Usualy:
 *  1. Reload config
 *
 * @brief MainWindow::unixSignalHup
 */
void MainWindow::unixSignalHup()
{
    if (cmdopts->getValue("config") || cmdopts->getValue('c')) {
        qDebug(">> Config option in command prompt...");
        QString cfgPath = cmdopts->getValue('c');
        if (cfgPath.isEmpty()) {
            cfgPath = cmdopts->getValue("config");
        }
        loadSettings(cfgPath);
    } else {
        loadSettings(QString(""));
    }
}

/**
 * Do something on Unix SIGUSR1 signal
 * Usualy:
 *  1. Reload config and load home page URI
 *  2. If option 'signals/SIGUSR1' defined and not empty - try to load defined URI
 *
 * @brief MainWindow::unixSignalUsr1
 */
void MainWindow::unixSignalUsr1()
{
    if (mainSettings->contains("signals/SIGUSR1") && !mainSettings->value("signals/SIGUSR1").toString().isEmpty()) {
        qDebug(">> SIGUSR1 >> Load URI from config file...");
        view->loadCustomPage(mainSettings->value("signals/SIGUSR1").toString());
    } else {
        qDebug(">> SIGUSR1 >> Load config file...");
        if (cmdopts->getValue("config") || cmdopts->getValue('c')) {
            qDebug(">> Config option in command prompt...");
            QString cfgPath = cmdopts->getValue('c');
            if (cfgPath.isEmpty()) {
                cfgPath = cmdopts->getValue("config");
            }
            loadSettings(cfgPath);
        } else {
            loadSettings(QString(""));
        }
        view->loadHomepage();
    }
}

/**
 * Do something on Unix SIGUSR2 signal
 * Usualy:
 *  1. Load home page URI
 *  2. If option 'signals/SIGUSR2' defined and not empty - try to load defined URI
 *
 * @brief MainWindow::unixSignalUsr2
 */
void MainWindow::unixSignalUsr2()
{
    if (mainSettings->contains("signals/SIGUSR2") && !mainSettings->value("signals/SIGUSR2").toString().isEmpty()) {
        qDebug(">> SIGUSR2 >> Load URI from config file...");
        view->loadCustomPage(mainSettings->value("signals/SIGUSR2").toString());
    } else {
        qDebug(">> SIGUSR2 >> Load homepage URI...");
        view->loadHomepage();
    }
}
