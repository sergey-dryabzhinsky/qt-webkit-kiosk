#ifndef QWEB_H
#define QWEB_H

#ifdef WEB_ENGINE

#include <QtWebEngine>
#include <QtWebEngineWidgets>

#include "webengine/fakewebview.h"
#include "webengine/webview.h"
#endif

#ifdef WEB_KIT

#include <QtWebKit>

#ifdef QT5
#include <QtWebKitWidgets/QWebInspector>
#endif

#include "webkit/fakewebview.h"
#include "webkit/webview.h"
#endif

#endif // QWEB_H
