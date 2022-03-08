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
#ifndef QWK_MAINWINDOW_H
#define QWK_MAINWINDOW_H

#include <QtGui>
#include <QHBoxLayout>
#include <QLabel>
#include <QProgressBar>
#include <QMainWindow>
#include <QtNetwork>
#include <QtWebKit>

#ifdef USE_TESTLIB
#include <QtTest/QTestEventList>
#endif

#ifdef QT5
#include <QtWebKitWidgets/QWebInspector>
#endif

#include "webview.h"
#include "anyoption.h"
#include "unixsignals.h"
#include "qwk_settings.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow();

    void init(AnyOption *cmdopts);

    void clearCache();
    void clearCacheOnExit();

protected slots:

    void cleanupSlot();

    void adjustTitle(QString);
    void setProgress(int p);
    void startLoading();
    void urlChanged(const QUrl &);
    void finishLoading(bool);
    void pageIconLoaded();

    void desktopResized(int p);

    void setupWindow();

    void delayedWindowResize();
    void delayedPageLoad();
    void delayedPageReload();

    void handleQwkNetworkError(QNetworkReply::NetworkError, QString);
    void handleQwkNetworkReplyUrl(QUrl);

    void networkStateChanged(QNetworkSession::State state);

    // TERM or INT - Quit from App
    void unixSignalQuit();
    // HUP - Reload config
    void unixSignalHup();
    // USR1 - Reload config and back to homepage, or load page from config
    void unixSignalUsr1();
    // USR2 - Back to homepage, or load page from config
    void unixSignalUsr2();

protected:
    void centerFixedSizeWindow();
    void attachJavascripts();
    void attachStyles();
    void putWindowUp();
    bool hideScrollbars();
    bool disableSelection();
    void keyPressEvent(QKeyEvent *event);
    void keyReleaseEvent(QKeyEvent *event);
    void resizeEvent(QResizeEvent* event);

private:
    WebView         *view;              // Webkit Page View
    QHBoxLayout     *topBox;            // Box for progress and messages
    QProgressBar    *loadProgress;      // progress bar to display page loading
    QLabel          *messagesBox;       // error messages

    QwkSettings     *qwkSettings;
    QNetworkDiskCache *diskCache;
    QWebInspector *inspector;

    QCursor     *hiddenCursor;
    QKeyEvent   *eventExit;

    AnyOption   *cmdopts;
    UnixSignals *handler;

#ifdef USE_TESTLIB
    QTestEventList *simulateClick;
#endif

    int progress;
    bool isScrollBarsHidden;
    bool isSelectionDisabled;
    bool isUrlRealyChanged;

    QNetworkSession *n_session;
    QNetworkInterface *network_interface;

    QTimer *delayedLoad;
};

#endif // QWK_MAINWINDOW_H
