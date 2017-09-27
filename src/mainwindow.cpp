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

#include <QDebug>
#include "config.h"
#include "mainwindow.h"

#ifdef QT5
#include <QStandardPaths>
#include <QApplication>
#include <QDesktopWidget>
#include <QtWebKitWidgets/QWebFrame>
#endif

#include "cachingnm.h"
#include "persistentcookiejar.h"

MainWindow::MainWindow() : QMainWindow()
{
    progress        = 0;
    diskCache       = NULL;
    topBox          = NULL;
    loadProgress    = NULL;
    messagesBox     = NULL;

    qwkSettings     = new QwkSettings();

    isUrlRealyChanged = false;

    handler = new UnixSignals();
    connect(handler, SIGNAL(sigBREAK()), SLOT(unixSignalQuit()));
    connect(handler, SIGNAL(sigTERM()), SLOT(unixSignalQuit()));
    connect(handler, SIGNAL(sigINT()), SLOT(unixSignalQuit()));
    connect(handler, SIGNAL(sigHUP()), SLOT(unixSignalHup()));

    network_interface = new QNetworkInterface();

    delayedResize = new QTimer();
    delayedLoad = new QTimer();

    sysTimeTimer = NULL;
    sysTimeTimer = new QTimer();

    pageStoppedLoadingTimer = NULL;
    pageStoppedLoadingTimer = new QTimer();
    connect(pageStoppedLoadingTimer, SIGNAL(timeout()), SLOT(checkPageStoppedLoading()));

    m_secsEpoch = 0;

    m_bPageLoaded = false;
    m_customTimeTagValue="";
    m_bPreventWasSet = false;

#ifdef USE_TESTLIB
    simulateClick = new QTestEventList();
#endif
}

MainWindow::~MainWindow()
{
    if( sysTimeTimer )
    {
        sysTimeTimer->stop();
        delete sysTimeTimer;
		sysTimeTimer = NULL;
    }

    if( pageStoppedLoadingTimer )
    {
        pageStoppedLoadingTimer->stop();
        delete pageStoppedLoadingTimer;
        pageStoppedLoadingTimer = NULL;
    }


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
        qwkSettings->loadSettings(cfgPath);
    } else {
        qwkSettings->loadSettings(QString(""));
    }

    if (qwkSettings->getBool("signals/enable")) {
        connect(handler, SIGNAL(sigUSR1()), SLOT(unixSignalUsr1()));
        connect(handler, SIGNAL(sigUSR2()), SLOT(unixSignalUsr2()));
    }
    handler->start();

    setMinimumWidth(320);
    setMinimumHeight(200);

    quint16 minimalWidth = qwkSettings->getUInt("view/minimal-width");
    quint16 minimalHeight = qwkSettings->getUInt("view/minimal-height");
    if (minimalWidth) {
        setMinimumWidth(minimalWidth);
    }
    if (minimalHeight) {
        setMinimumHeight(minimalHeight);
    }

    hiddenCurdor = new QCursor(Qt::BlankCursor);

    qDebug() << "Application icon: " << qwkSettings->getQString("application/icon");
    setWindowIcon(QIcon(
       qwkSettings->getQString("application/icon")
    ));

    if (cmdopts->getValue("uri") || cmdopts->getValue('u')) {
        qDebug(">> Uri option in command prompt...");
        QString uri = cmdopts->getValue('u');
        if (uri.isEmpty()) {
            uri = cmdopts->getValue("uri");
        }
        qwkSettings->setValue("browser/homepage", uri);
    }

    QCoreApplication::setOrganizationName(
            qwkSettings->getQString("application/organization")
            );
    QCoreApplication::setOrganizationDomain(
            qwkSettings->getQString("application/organization-domain")
            );
    QCoreApplication::setApplicationName(
            qwkSettings->getQString("application/name")
            );
    QCoreApplication::setApplicationVersion(
            qwkSettings->getQString("application/version")
            );

    // --- Network --- //

    if (qwkSettings->getBool("proxy/enable")) {
        bool system = qwkSettings->getBool("proxy/system");
        if (system) {
            QNetworkProxyFactory::setUseSystemConfiguration(system);
        } else {
            QNetworkProxy proxy;
            proxy.setType(QNetworkProxy::HttpProxy);
            proxy.setHostName(
                qwkSettings->getQString("proxy/host")
            );
            proxy.setPort(qwkSettings->getUInt("proxy/port"));
            if (qwkSettings->getBool("proxy/auth")) {
                proxy.setUser(qwkSettings->getQString("proxy/username"));
                proxy.setPassword(qwkSettings->getQString("proxy/password"));
            }
            QNetworkProxy::setApplicationProxy(proxy);
        }
    }

    // --- Web View --- //
    view = new WebView(this);

    QPalette paletteG = this->palette();
    paletteG.setColor(QPalette::Window, QColor(220,240,220,127));

    QPalette paletteR = this->palette();
    paletteR.setColor(QPalette::Window, QColor(240,220,220,127));

    topBox = new QHBoxLayout(view);
    topBox->setContentsMargins(2, 2, 2, 2);

    if (qwkSettings->getBool("view/show_load_progress")) {
        // --- Progress Bar --- //
        loadProgress = new QProgressBar();
        loadProgress->setContentsMargins(2, 2, 2, 2);
        loadProgress->setMinimumSize(100, 16);
        loadProgress->setMaximumSize(100, 16);
        loadProgress->setAutoFillBackground(true);
        loadProgress->setPalette(paletteG);

        // Do not work... Need Layout...
        loadProgress->setAlignment(Qt::AlignTop | Qt::AlignLeft);
        loadProgress->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);

        loadProgress->hide();

        topBox->addWidget(loadProgress, 0, Qt::AlignTop | Qt::AlignLeft);
    }

    if (qwkSettings->getBool("browser/show_error_messages")) {
        // --- Messages Box --- //
        messagesBox = new QLabel();
        messagesBox->setContentsMargins(2, 2, 2, 2);
        // messagesBox->setWordWrap(true);
        messagesBox->setAutoFillBackground(true);
        messagesBox->setPalette(paletteR);

        // Do not work... Need Layout...
        messagesBox->setAlignment(Qt::AlignVCenter);
        messagesBox->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::MinimumExpanding);

        messagesBox->hide();

        topBox->addStretch(2);
        topBox->addWidget(messagesBox, 1, Qt::AlignTop | Qt::AlignLeft);
    }

    setCentralWidget(view);

    view->setSettings(qwkSettings);
    view->setPage(new QWebPage(view));

    // --- Disk cache --- //
    if (qwkSettings->getBool("cache/enable")) {
        diskCache = new QNetworkDiskCache(this);
        QString location = qwkSettings->getQString("cache/location");
        if (!location.length()) {
#ifdef QT5
            location = QStandardPaths::writableLocation(QStandardPaths::CacheLocation);
#else
            location = QDesktopServices::storageLocation(QDesktopServices::CacheLocation);
#endif
        }
        diskCache->setCacheDirectory(location);
        diskCache->setMaximumCacheSize(qwkSettings->getUInt("cache/size"));

        if (qwkSettings->getBool("cache/clear-on-start")) {
            diskCache->clear();
        }
        else if (cmdopts->getFlag('C') || cmdopts->getFlag("clear-cache")) {
            diskCache->clear();
        }

        CachingNetworkManager *nm = new CachingNetworkManager();
        nm->setCache(diskCache);
        view->page()->setNetworkAccessManager(nm);
    }

    if (qwkSettings->getBool("browser/cookiejar")) {
        view->page()->networkAccessManager()->setCookieJar(new PersistentCookieJar());
    }

    view->settings()->setAttribute(QWebSettings::JavascriptEnabled,
        qwkSettings->getBool("browser/javascript")
    );

    view->settings()->setAttribute(QWebSettings::JavascriptCanOpenWindows,
        qwkSettings->getBool("browser/javascript_can_open_windows")
    );

    view->settings()->setAttribute(QWebSettings::JavascriptCanCloseWindows,
        qwkSettings->getBool("browser/javascript_can_close_windows")
    );

    view->settings()->setAttribute(QWebSettings::WebGLEnabled,
        qwkSettings->getBool("browser/webgl")
    );

    view->settings()->setAttribute(QWebSettings::JavaEnabled,
        qwkSettings->getBool("browser/java")
    );
    view->settings()->setAttribute(QWebSettings::PluginsEnabled,
        qwkSettings->getBool("browser/plugins")
    );

#if QT_VERSION >= 0x050400
    view->settings()->setAttribute(QWebSettings::Accelerated2dCanvasEnabled, true);
#endif
    view->settings()->setAttribute(QWebSettings::AcceleratedCompositingEnabled, true);
#if QT_VERSION >= 0x050200
    view->settings()->setAttribute(QWebSettings::CSSRegionsEnabled, true);
    view->settings()->setAttribute(QWebSettings::CSSGridLayoutEnabled, true);
#endif
    view->settings()->setAttribute(QWebSettings::SiteSpecificQuirksEnabled, true);

    if (qwkSettings->getBool("inspector/enable")) {
        view->settings()->setAttribute(QWebSettings::DeveloperExtrasEnabled, true);

        inspector = new QWebInspector();
        inspector->setVisible(qwkSettings->getBool("inspector/visible"));
        inspector->setMinimumSize(800, 600);
        inspector->setWindowTitle(qwkSettings->getQString("application/name") + " - WebInspector");
        inspector->setWindowIcon(this->windowIcon());
        inspector->setPage(view->page());
    }

    connect(view->page()->mainFrame(), SIGNAL(titleChanged(QString)), SLOT(adjustTitle(QString)));
    connect(view->page()->mainFrame(), SIGNAL(loadStarted()), SLOT(startLoading()));
    connect(view->page()->mainFrame(), SIGNAL(urlChanged(const QUrl &)), SLOT(urlChanged(const QUrl &)));
    connect(view->page(), SIGNAL(loadProgress(int)), SLOT(setProgress(int)));
    connect(view->page()->mainFrame(), SIGNAL(loadFinished(bool)), SLOT(finishLoading(bool)));
    connect(view->page()->mainFrame(), SIGNAL(iconChanged()), SLOT(pageIconLoaded()));
    connect(view, SIGNAL(qwkNetworkError(QNetworkReply::NetworkError,QString)), SLOT(handleQwkNetworkError(QNetworkReply::NetworkError,QString)));
    connect(view, SIGNAL(qwkNetworkReplyUrl(QUrl)), SLOT(handleQwkNetworkReplyUrl(QUrl)));

    QNetworkConfigurationManager manager;
    QNetworkConfiguration cfg = manager.defaultConfiguration();

    n_session = new QNetworkSession(cfg);
    connect(n_session, SIGNAL(stateChanged(QNetworkSession::State)), this, SLOT(networkStateChanged(QNetworkSession::State)));
    n_session->open();

    QDesktopWidget *desktop = QApplication::desktop();
    connect(desktop, SIGNAL(resized(int)), SLOT(desktopResized(int)));

    // Window show, start events loop
    show();

    view->page()->view()->setFocusPolicy(Qt::StrongFocus);
    view->setFocusPolicy(Qt::StrongFocus);

    if (qwkSettings->getBool("view/hide_mouse_cursor")) {
        QApplication::setOverrideCursor(Qt::BlankCursor);
        view->setCursor(*hiddenCurdor);
        QApplication::processEvents(); //process events to force cursor update before press
    }

    int delay_resize = 1;
    if (qwkSettings->getBool("view/startup_resize_delayed")) {
        delay_resize = qwkSettings->getUInt("view/startup_resize_delay");
    }
    delayedResize->singleShot(delay_resize, this, SLOT(delayedWindowResize()));

    int delay_load = 1;
    if (qwkSettings->getBool("browser/startup_load_delayed")) {
        delay_load = qwkSettings->getUInt("browser/startup_load_delay");
    }
    delayedLoad->singleShot(delay_load, this, SLOT(delayedPageLoad()));



    /*check system time changes and refresh page in case */
    if( sysTimeTimer )
    {
        sysTimeTimer->start(MainWindow::SYSTIMER_MSEC);
        QObject::connect(sysTimeTimer,  SIGNAL(timeout()), this, SLOT(onTimeOutCheckTime()));
    }

}

void MainWindow::delayedWindowResize()
{
    if (qwkSettings->getBool("view/fullscreen")) {
        showFullScreen();
    } else if (qwkSettings->getBool("view/maximized")) {
        showMaximized();
    } else if (qwkSettings->getBool("view/fixed-size")) {
        centerFixedSizeWindow();
    }
    QApplication::processEvents(); //process events to force update

    this->setFocusPolicy(Qt::StrongFocus);
    this->focusWidget();

    if (qwkSettings->getBool("view/stay_on_top")) {
        setWindowFlags(Qt::FramelessWindowHint|Qt::WindowStaysOnTopHint);
    }
    QApplication::processEvents(); //process events to force update
}

void MainWindow::resizeEvent(QResizeEvent* event)
{
   QMainWindow::resizeEvent(event);
   // Your code here.
}

void MainWindow::delayedPageLoad()
{
    // Wait 1 second. May be not here?
    n_session->waitForOpened(1000);

    view->loadHomepage();
}

void MainWindow::delayedPageReload()
{
    // Wait 1 second. May be not here?
    n_session->waitForOpened(1000);

    view->reload();
}

void MainWindow::clearCache()
{
    if (qwkSettings->getBool("cache/enable")) {
        if (diskCache) {
            diskCache->clear();
        }
    }
}

void MainWindow::clearCacheOnExit()
{
    if (qwkSettings->getBool("cache/enable")) {
        if (qwkSettings->getBool("cache/clear-on-exit")) {
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
    quint16 widowWidth = qwkSettings->getUInt("view/fixed-width");
    quint16 widowHeight = qwkSettings->getUInt("view/fixed-height");

    quint16 screenWidth = QApplication::desktop()->screenGeometry().width();
    quint16 screenHeight = QApplication::desktop()->screenGeometry().height();

    qDebug() << "Screen size: " << screenWidth << "x" << screenHeight;

    quint16 x = 0;
    quint16 y = 0;

    if (qwkSettings->getUInt("view/fixed-centered")) {
        x = (screenWidth - widowWidth) / 2;
        y = (screenHeight - widowHeight) / 2;
    } else {
        x = qwkSettings->getUInt("view/fixed-x");
        y = qwkSettings->getUInt("view/fixed-y");
    }

    qDebug() << "Move window to: (" << x << ";" << y << ")";

    move ( x, y );
    setFixedSize( widowWidth, widowHeight );
}


void MainWindow::keyPressEvent(QKeyEvent *event)
{
    if (qwkSettings->getUInt("browser/disable_hotkeys")) {
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
    case Qt::Key_Backspace:
        view->page()->triggerAction(QWebPage::Back);
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
    case Qt::Key_F8:
        view->stop();
        break;
    case Qt::Key_F12:
        if (qwkSettings->getUInt("inspector/enable")) {
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

void MainWindow::handleQwkNetworkError(QNetworkReply::NetworkError error, QString message)
{
    qDebug() << QDateTime::currentDateTime().toString()
             << "MainWindow::handleQwkNetworkError"
                ;
    if (message.contains("Host ") && message.contains(" not found")) {
        // Don't give a damn
        qDebug() << "Don't give a damn";
        return;
    }
/*    if (message.contains("Error downloading ") && message.contains("server replied: Not Found")) {
        // Don't give a damn
        qDebug() << "Don't give a damn";
        return;
    }*/

    // Unknown error if eth0 if up but cable out
    if (error == QNetworkReply::UnknownNetworkError) {
        if (message.contains("Network access is disabled")) {
            // Check all interfaces if them has link up

            bool hasLinkUp = false;
            foreach (QNetworkInterface interface, network_interface->allInterfaces()) {
                if ((interface.flags() & QNetworkInterface::IsUp) &&
                        (interface.flags() & QNetworkInterface::IsRunning)
                        )
                    hasLinkUp = true;
            }

            if (hasLinkUp && view->page()->networkAccessManager()->networkAccessible() != QNetworkAccessManager::Accessible) {

                qDebug() << QDateTime::currentDateTime().toString()
                         << "MainWindow::networkStateChanged -"
                         << "Has interfaces Up and network not Accessible? Force Accessible state and NetworkSession restart!"
                            ;

                // force network accessibility to get local content
                view->page()->networkAccessManager()->setNetworkAccessible(QNetworkAccessManager::Accessible);
                // force NetSession restart
                n_session->close();
                n_session->open();
                message += QString("\nNetwork session restarted!");

                qint32 delay_reload = qwkSettings->getInt("browser/network_error_reload_delay", 15000);

                if (delay_reload >= 0) {
                    qDebug() << QDateTime::currentDateTime().toString()
                             << "MainWindow::networkStateChanged -"
                             << "Delay WebView reload by" << delay_reload/1000. << "sec."
                                ;
                    message += QString("\nPage reload queued! Plase wait ") + QVariant(delay_reload / 1000.).toString() + QString(" seconds...");
                    // Try reload broken view downloads
                    delayedLoad->singleShot(
                        delay_reload,
                        this, SLOT(delayedPageReload())
                    );
                }
            }

        }
    }

    if (messagesBox) {
        messagesBox->show();
        QString txt = messagesBox->text();
        if (txt.length()) {
            txt += "\n";
        }
        messagesBox->setText(txt + QDateTime::currentDateTime().toString() + " :: " + message);
    }
}

void MainWindow::desktopResized(int p)
{
    qDebug() << "Desktop resized event: " << p;
    if (qwkSettings->getBool("view/fullscreen")) {
        showFullScreen();
    } else if (qwkSettings->getBool("view/maximized")) {
        showMaximized();
    } else if (qwkSettings->getBool("view/fixed-size")) {
        centerFixedSizeWindow();
    }
}

/**
 * @brief MainWindow::pageIconLoaded
 * This is triggered by WebView->page()->mainFrame now
 */
void MainWindow::pageIconLoaded()
{
    setWindowIcon(view->icon());
}

/**
 * @brief MainWindow::startLoading
 * This is triggered by WebView->page()->mainFrame now
 */
void MainWindow::startLoading()
{
    qDebug()  << QDateTime::currentDateTime().toString() << "Start loading...";

    progress = 0;

    isUrlRealyChanged = false;

    adjustTitle(view->title());

    QWebSettings::clearMemoryCaches();

    if (loadProgress) {
        loadProgress->show();
    }

    if (messagesBox) {
        messagesBox->setText("");
        messagesBox->hide();
    }
	
	
    if( pageStoppedLoadingTimer )
    {
        pageStoppedLoadingTimer->stop(); //stop first before next one gets started

        //use full timer instead of singleshot because stop is needed
        pageStoppedLoadingTimer->setInterval(TIMER_LOAD_WAIT);
        pageStoppedLoadingTimer->setSingleShot(true);
        pageStoppedLoadingTimer->start();
    }
    m_bPageLoaded = false;

}

void MainWindow::adjustTitle(QString title)
{
    if (progress <= 0 || progress >= 100) {
        setWindowTitle(title);
    } else {
        setWindowTitle(QString("%1 (%2%)").arg(title).arg(progress));
    }
}

/**
 * @brief MainWindow::setProgress
 * This is triggered every resource loaded by WebView->page->all frames
 * @param p
 */
void MainWindow::setProgress(int p)
{
    progress = p;
    adjustTitle(view->title());

    qDebug() << QDateTime::currentDateTime().toString() << "Loading progress: " << p;

    if (loadProgress) {
        loadProgress->setValue(p);
        if (p != 100) {
            loadProgress->show();
            view->resetLoadTimer();
        } else {
            loadProgress->hide();
            view->stopLoadTimer();
        }
    }

    // 1. Hide scrollbars (and add some styles)
    // If there complete head and body start loaded...
    if (!isScrollBarsHidden && isUrlRealyChanged) {
        isScrollBarsHidden = hideScrollbars();
    }
}

/**
 * @brief MainWindow::urlChanged
 * This is triggered by WebView->page()->mainFrame now
 * @param url
 */
void MainWindow::urlChanged(const QUrl &url)
{
    qDebug() << QDateTime::currentDateTime().toString() << "URL changes: " << url.toString();

    // Where is a real change in webframe! Drop flags.
    isScrollBarsHidden = false;
    isSelectionDisabled = false;
    isUrlRealyChanged = true;

    if (qwkSettings->getReal("view/page_scale")) {
        view->page()->mainFrame()->setZoomFactor(qwkSettings->getReal("view/page_scale"));
    }

    view->resetLoadTimer();

    // This is real link clicked
    view->playSound("event-sounds/link-clicked");
}

/**
 * @brief MainWindow::finishLoading
 * This is triggered by WebView->page()->mainFrame now
 * @param ok
 */
void MainWindow::finishLoading(bool ok)
{
    qDebug() << QDateTime::currentDateTime().toString()
             << "MainWindow::finishLoading - "
             << "ok =" << (int)ok
                ;

    view->stopLoadTimer();
	m_bPageLoaded = true;
	
    if (!ok) {
        if (messagesBox) {
            messagesBox->show();
            QString txt = messagesBox->text();
            if (txt.length()) {
                txt += "\n";
            }
            messagesBox->setText(txt + QDateTime::currentDateTime().toString() +  " :: Page not fully loaded! Loosed network connection or page load timeout.");
        }

        // Don't do any action
        return;
    }

    progress = 100;
    adjustTitle(view->title());

    if (messagesBox) {
        if (!messagesBox->text().length() && !messagesBox->isHidden() ) {
            messagesBox->hide();
        }
    }

    if (loadProgress) {
        loadProgress->hide();
    }

    // On AJAX it's triggered too?
    if (isUrlRealyChanged) {
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
	    attachCustomJavascript();

        // 3. Focus window and click into it to stimulate event loop after signal handling
        putWindowUp();
    }
}

/**
 * @return bool - true - if don't need to hide scrollbars or hide them, false - if need, but there is no html body loaded.
 * @brief MainWindow::hideScrollbars
 */
bool MainWindow::hideScrollbars()
{
    qDebug("MainWindow::hideScrollbars");

    if (qwkSettings->getBool("view/hide_scrollbars")) {
        qDebug("Try to hide scrollbars...");

        view->page()->mainFrame()->setScrollBarPolicy( Qt::Vertical, Qt::ScrollBarAlwaysOff );
        view->page()->mainFrame()->setScrollBarPolicy( Qt::Horizontal, Qt::ScrollBarAlwaysOff );
    }

    return true;
}

bool MainWindow::disableSelection()
{
    qDebug("MainWindow::disableSelection");

    if (qwkSettings->getBool("view/disable_selection")) {
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
            content += "body, div, p, span, h1, h2, h3, h4, h5, h6, caption, td, li, dt, dd {\n";
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
    qDebug("MainWindow::attachJavascripts");

    QStringList scripts = qwkSettings->getQStringList("attach/javascripts");
    if (!scripts.length()) {
        return;
    }

    QWebElement bodyElem = view->page()->mainFrame()->findFirstElement("body");
    if (bodyElem.isNull() || bodyElem.toInnerXml().trimmed().isEmpty()) {
        // No body here... We need something in <body> to interact with?
        return;
    }

    QString content = "";
    QStringListIterator scriptsIterator(scripts);
    QFileInfo finfo = QFileInfo();
    QString file_name;
    quint32 countScripts = 0;

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

/*Function for attaching custom javascript*/
void MainWindow::attachCustomJavascript()
{
    qDebug("MainWindow::attachCustomJavascript");
    QWebElement headElem = view->page()->mainFrame()->findFirstElement("head");
    if (headElem.isNull() || headElem.toInnerXml().trimmed().isEmpty()) {
        // No head here...
        return;
    }

    QWebElement bodyEle = view->page()->mainFrame()->findFirstElement("body");
    if (bodyEle.isNull() || bodyEle.toInnerXml().trimmed().isEmpty()) {
        // No body here...
        return;
    }

    bodyEle.appendInside("<div id=\"customTimeTagQT\" style=\"display:none;\"></div>");

    QString content = "";
    content += "clearInterval( window.idIntervalQtTimeCheck ); \n\n "; //clear global Interval always first to prevent interval overlaps
    content += "function setTimeHeadQT()\n";
    content += "{\n";
    content += "    if( document.getElementById('customTimeTagQT') !== null ) { \n";
    content += "        document.getElementById('customTimeTagQT').innerHTML = Date.now();\n";
    content += "    }\n";
    content += "}\n";
    content += "function startQtTimer(){ \n";
    content += "  if( typeof window.idIntervalQtTimeCheck == \"undefined\"  || window.idIntervalQtTimeCheck == -1 ){ \n";
    content += "        window.idIntervalQtTimeCheck = setInterval(function(){\n";
    content += "          setTimeHeadQT(); \n";
    content += "        },1500);\n";
    content += "    } ";
    content += "} \n\n ";
    content += "function stopQtTimer(){ \n";
    content += "    clearInterval( window.idIntervalQtTimeCheck ); window.idIntervalQtTimeCheck = -1; \n";
    content += "} \n\n ";
    content += " setTimeHeadQT(); \n";
    content += " startQtTimer();  \n";


    view->page()->mainFrame()->evaluateJavaScript(content);


    qDebug() << "customTimeTags appended to DOM";
}

void MainWindow::attachStyles()
{
    qDebug("MainWindow::attachStyles");

    QStringList styles = qwkSettings->getQStringList("attach/styles");
    if (!styles.length()) {
        return;
    }

    QWebElement headElem = view->page()->mainFrame()->findFirstElement("head");
    if (headElem.isNull() || headElem.toInnerXml().trimmed().isEmpty()) {
        // Page without head... We need something in <head> to interact with?
        return;
    }

    QString content = "";
    QStringListIterator stylesIterator(styles);
    QString file_name;
    QFileInfo finfo = QFileInfo();
    quint32 countStyles = 0;

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
 * Activate window after signal
 */
void MainWindow::putWindowUp()
{
    qDebug("Try to activate window...");

    QApplication::setActiveWindow(this);
    this->focusWidget();

#ifdef USE_TESTLIB
    qDebug("... by click simulation...");
    simulateClick->clear();
    simulateClick->addMouseClick(Qt::LeftButton, 0, this->pos(), -1);
    simulateClick->simulate(this);
#endif

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
        qwkSettings->loadSettings(cfgPath);
    } else {
        qwkSettings->loadSettings(QString(""));
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
    if (!qwkSettings->getQString("signals/SIGUSR1").isEmpty()) {
        qDebug(">> SIGUSR1 >> Load URI from config file...");
        view->loadCustomPage(qwkSettings->getQString("signals/SIGUSR1"));
    } else {
        qDebug(">> SIGUSR1 >> Load config file...");
        if (cmdopts->getValue("config") || cmdopts->getValue('c')) {
            qDebug(">> Config option in command prompt...");
            QString cfgPath = cmdopts->getValue('c');
            if (cfgPath.isEmpty()) {
                cfgPath = cmdopts->getValue("config");
            }
            qwkSettings->loadSettings(cfgPath);
        } else {
            qwkSettings->loadSettings(QString(""));
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
    if (!qwkSettings->getQString("signals/SIGUSR2").isEmpty()) {
        qDebug(">> SIGUSR2 >> Load URI from config file...");
        view->loadCustomPage(qwkSettings->getQString("signals/SIGUSR2"));
    } else {
        qDebug(">> SIGUSR2 >> Load homepage URI...");
        view->loadHomepage();
    }
}

// ----------------------- NETWORK -----------------------------

/**
 * @brief MainWindow::networkStateChanged
 * It's triggered if global network online status changed
 * @param state
 */
void MainWindow::networkStateChanged(QNetworkSession::State state)
{
    qDebug() << QDateTime::currentDateTime().toString()
             << "MainWindow::networkStateChanged -"
             << state
                ;
    bool doReload = false;
    if (state == QNetworkSession::Connected) {
        // Reload current page, network here again!
        doReload = true;
    }
    QString errStr = "";
    switch (state) {
    case QNetworkSession::NotAvailable:
        errStr = "Network not available! Check your cable!";
        doReload = true;
        break;
    case QNetworkSession::Disconnected:
        errStr = "Network is down! Check your network settings!";
        doReload = true;
        break;
    default:
        break;
    }
    if (doReload) {

        bool hasLoFace = false;
        foreach (QNetworkInterface interface, network_interface->allInterfaces()) {
            if ((interface.flags() & QNetworkInterface::IsUp) &&
                    (interface.flags() & QNetworkInterface::IsRunning) &&
                    (interface.flags() & QNetworkInterface::IsLoopBack)
                    )
                hasLoFace = true;
        }

        if (hasLoFace && view->page()->networkAccessManager()->networkAccessible() != QNetworkAccessManager::Accessible) {

            qDebug() << QDateTime::currentDateTime().toString()
                     << "MainWindow::networkStateChanged -"
                     << "Has loopBack interface and network not accessible? Force accessible state!"
                        ;

            // force network accessibility to get local content
            view->page()->networkAccessManager()->setNetworkAccessible(QNetworkAccessManager::Accessible);
            // force NetSession restart
            n_session->close();
            n_session->open();
            errStr += QString("\nNetwork session restarted!");
        }

        qint32 delay_reload = qwkSettings->getInt("browser/network_error_reload_delay", 15000);

        if (delay_reload >= 0) {
            qDebug() << QDateTime::currentDateTime().toString()
                     << "MainWindow::networkStateChanged -"
                     << "Delay WebView reload by" << delay_reload/1000. << "msec."
                        ;

            errStr += QString("\nPage reload queued! Plase wait ") + QVariant(delay_reload / 1000.).toString() + QString(" seconds...");
            delayedLoad->singleShot(delay_reload, this, SLOT(delayedPageReload()));
        }
    }
    if (errStr.length()) {
       if (messagesBox) {
           messagesBox->show();
           QString txt = messagesBox->text();
           if (txt.length()) {
               txt += "\n";
           }
           messagesBox->setText(txt + QDateTime::currentDateTime().toString() +  " :: " + errStr);
       }
    }
}

void MainWindow::handleQwkNetworkReplyUrl(QUrl url)
{
    qDebug() << QDateTime::currentDateTime().toString()
             << "MainWindow::handleQwkNetworkReplyUrl - "
             << "url=" << url.toString()
                ;
}




//restart application detached
void MainWindow::appReboot()
{
    QStringList qListArg =  QApplication::arguments();
    qListArg.removeFirst();     //remove application name because will fillup on each restart
    qListArg.append("-qws");    //need to add qws becasue not captured by argument
    qDebug() << "Performing application reboot..." << qListArg;


    if( QProcess::startDetached( QApplication::applicationFilePath(), qListArg ,  QDir::currentPath() ))
    {
        qDebug() << "application rebooted ";
    }
    else
    {
        qDebug() << "application reboot failed ";
    }

    QCoreApplication::exit();

}

//check every x seconds if time has changed and do reload of page if it is the case
void MainWindow::onTimeOutCheckTime()
{

    // QString qstrUrl = view->url().toString();
    // QString qstrUrlPage = qstrUrl.mid( qstrUrl.lastIndexOf("/") + 1);


    if( view && m_bPageLoaded )
    {
        //check if custom tag with id is set , if it is set we do not check for timer stop on this page
        QWebElement customReloadPrevention = view->page()->mainFrame()->findFirstElement("#qt_tag_do_not_restart");
        bool bReloadPrev = customReloadPrevention.isNull() ? false : true;

        if( !bReloadPrev )
        {
            bool bSecEpochTimerReset = false;
            //we had a site change from a prevent page to normal page, we need to restart and set Time
            if( m_bPreventWasSet )
            {
                view->page()->mainFrame()->evaluateJavaScript("setTimeHeadQT();");
                view->page()->mainFrame()->evaluateJavaScript("startQtTimer();");
                m_bPreventWasSet = false; //reset var for next page switch
                m_customTimeTagValue = "RESET_CASE";  //set to different value because of time change detection
                bSecEpochTimerReset = true;
            }

            //do handling if page is loaded
            if( m_bPageLoaded )
            {
                //check first if own tag time is not changed (hang state)
                QWebElement customTimeTag = view->page()->mainFrame()->findFirstElement("#customTimeTagQT");

                QString customDOM = customTimeTag.toInnerXml().trimmed();
                if (!customTimeTag.isNull() && !customDOM.isEmpty() ) {

                    //if same value is seen the time is not updated =>  assume hang
                    if( m_customTimeTagValue == customDOM )
                    {
                        qDebug() << "onTimeOutCheckTime customTimeTagQT content exact match HANG,  application restart";
                        qDebug() << "m_customTimeTagValue: " << m_customTimeTagValue << " customDOM: " << customDOM ;
                        appReboot();
                    }

                    //qDebug() << "onTimeOutCheckTime  m_customTimeTagValue"<< m_customTimeTagValue << " - " << customDOM ;

                    m_customTimeTagValue = customDOM; //set current value

                }


#ifdef HANDLE_TIME_CHANGE_RELOAD
                qint64 currTimeStamp = QDateTime::currentMSecsSinceEpoch();
                //first  call set time
                if( m_secsEpoch == 0 )
                {
                   m_secsEpoch = currTimeStamp;
                }

                unsigned int uiTime = SYSTIMER_DIFF_MSEC;
                if( bSecEpochTimerReset )
                {
                    uiTime = SYSTIMER_DIFF_MSEC_STOPPED_TIMER;
                }
                if(  currTimeStamp - ( m_secsEpoch + SYSTIMER_MSEC)  >=  uiTime  )
                {
                    qDebug() << "application reload because of time differences future "<< uiTime  << " ms difference";
                    view->reload();
                }
                else if (  ( m_secsEpoch + SYSTIMER_MSEC) - currTimeStamp >=  uiTime)
                {
                    qDebug() << "application reload because of time differences past "<< uiTime  << " ms difference";
                    view->reload();
                }

                m_secsEpoch = currTimeStamp; //update timestamp
#endif
            }
        }
        else
        { 
            m_bPreventWasSet = true;
            view->page()->mainFrame()->evaluateJavaScript("stopQtTimer();");
        }
    }

    //qDebug() << "m_secsEpoch:" << m_secsEpoch << "currTimeStamp" << currTimeStamp;

}

//check if flag is set else reboot app
void MainWindow::checkPageStoppedLoading()
{
    qDebug("checkPageStoppedLoading...");

    if ( !m_bPageLoaded )
    {
        qDebug("checkPageStoppedLoading still not loaded, trigger app reboot");
        appReboot();
    }
}
