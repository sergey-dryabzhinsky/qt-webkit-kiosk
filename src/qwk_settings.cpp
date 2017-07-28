#include <qwk_settings.h>

#include <QtGui>
#ifdef QT5
#include <QStandardPaths>
#endif

QwkSettings::QwkSettings(QObject *parent) : QObject(parent)
{
    qsettings = NULL;
}

/**
 * @brief QwkSettings::loadSettings
 * @param ini_file
 */
void QwkSettings::loadSettings(QString ini_file)
{
    if (qsettings != NULL) {
        delete qsettings;
    }
    if (!ini_file.length()) {
        qsettings = new QSettings(QSettings::IniFormat, QSettings::UserScope, "QtWebkitKiosk", "config", this);
    } else {
        qsettings = new QSettings(ini_file, QSettings::IniFormat, this);
    }
    qDebug() << "Ini file: " << qsettings->fileName();

    if (!qsettings->contains("application/organization")) {
        qsettings->setValue("application/organization", "Organization" );
    }
    if (!qsettings->contains("application/organization-domain")) {
        qsettings->setValue("application/organization-domain", "www.example.com" );
    }
    if (!qsettings->contains("application/name")) {
        qsettings->setValue("application/name", "QtWebkitKiosk" );
    }
    if (!qsettings->contains("application/version")) {
        qsettings->setValue("application/version", VERSION );
    }
    if (!qsettings->contains("application/icon")) {
        qsettings->setValue("application/icon", ICON );
    }

    if (!qsettings->contains("proxy/enable")) {
        qsettings->setValue("proxy/enable", false);
    }
    if (!qsettings->contains("proxy/system")) {
        qsettings->setValue("proxy/system", true);
    }
    if (!qsettings->contains("proxy/host")) {
        qsettings->setValue("proxy/host", "proxy.example.com");
    }
    if (!qsettings->contains("proxy/port")) {
        qsettings->setValue("proxy/port", 3128);
    }
    if (!qsettings->contains("proxy/auth")) {
        qsettings->setValue("proxy/auth", false);
    }
    if (!qsettings->contains("proxy/username")) {
        qsettings->setValue("proxy/username", "username");
    }
    if (!qsettings->contains("proxy/password")) {
        qsettings->setValue("proxy/password", "password");
    }

    if (!qsettings->contains("view/fullscreen")) {
        qsettings->setValue("view/fullscreen", true);
    }
    if (!qsettings->contains("view/maximized")) {
        qsettings->setValue("view/maximized", false);
    }
    if (!qsettings->contains("view/fixed-size")) {
        qsettings->setValue("view/fixed-size", false);
    }
    if (!qsettings->contains("view/fixed-width")) {
        qsettings->setValue("view/fixed-width", 800);
    }
    if (!qsettings->contains("view/fixed-height")) {
        qsettings->setValue("view/fixed-height", 600);
    }
    if (!qsettings->contains("view/minimal-width")) {
        qsettings->setValue("view/minimal-width", 320);
    }
    if (!qsettings->contains("view/minimal-height")) {
        qsettings->setValue("view/minimal-height", 200);
    }
    if (!qsettings->contains("view/fixed-centered")) {
        qsettings->setValue("view/fixed-centered", true);
    }
    if (!qsettings->contains("view/fixed-x")) {
        qsettings->setValue("view/fixed-x", 0);
    }
    if (!qsettings->contains("view/fixed-y")) {
        qsettings->setValue("view/fixed-y", 0);
    }

    if (!qsettings->contains("view/startup_resize_delayed")) {
        qsettings->setValue("view/startup_resize_delayed", true);
    }
    if (!qsettings->contains("view/startup_resize_delay")) {
        qsettings->setValue("view/startup_resize_delay", 200);
    }

    if (!qsettings->contains("view/hide_scrollbars")) {
        qsettings->setValue("view/hide_scrollbars", true);
    }

    if (!qsettings->contains("view/stay_on_top")) {
        qsettings->setValue("view/stay_on_top", false);
    }

    if (!qsettings->contains("view/disable_selection")) {
        qsettings->setValue("view/disable_selection", true);
    }

    if (!qsettings->contains("view/show_load_progress")) {
        qsettings->setValue("view/show_load_progress", true);
    }

    if (!qsettings->contains("view/page_scale")) {
        qsettings->setValue("view/page_scale", 1.0);
    }


    if (!qsettings->contains("browser/homepage")) {
        qsettings->setValue("browser/homepage", RESOURCES"default.html");
    }
    if (!qsettings->contains("browser/javascript")) {
        qsettings->setValue("browser/javascript", true);
    }
    if (!qsettings->contains("browser/javascript_can_open_windows")) {
        qsettings->setValue("browser/javascript_can_open_windows", false);
    }
    if (!qsettings->contains("browser/javascript_can_close_windows")) {
        qsettings->setValue("browser/javascript_can_close_windows", false);
    }
    if (!qsettings->contains("browser/webgl")) {
        qsettings->setValue("browser/webgl", false);
    }
    if (!qsettings->contains("browser/java")) {
        qsettings->setValue("browser/java", false);
    }
    if (!qsettings->contains("browser/plugins")) {
        qsettings->setValue("browser/plugins", true);
    }
    // Don't break on SSL errors
    if (!qsettings->contains("browser/ignore_ssl_errors")) {
        qsettings->setValue("browser/ignore_ssl_errors", true);
    }
    if (!qsettings->contains("browser/cookiejar")) {
        qsettings->setValue("browser/cookiejar", false);
    }
    // Show default homepage if window closed by javascript
    if (!qsettings->contains("browser/show_homepage_on_window_close")) {
        qsettings->setValue("browser/show_homepage_on_window_close", true);
    }

    if (!qsettings->contains("browser/startup_load_delayed")) {
        qsettings->setValue("browser/startup_load_delayed", true);
    }
    if (!qsettings->contains("browser/startup_load_delay")) {
        qsettings->setValue("browser/startup_load_delay", 100);
    }

    if (!qsettings->contains("browser/disable_hotkeys")) {
        qsettings->setValue("browser/disable_hotkeys", false);
    }

    if (!qsettings->contains("browser/page_load_timeout")) {
        qsettings->setValue("browser/page_load_timeout", 15000);
    }
    if (!qsettings->contains("browser/network_error_reload_delay")) {
        qsettings->setValue("browser/network_error_reload_delay", 15000);
    }

    if (!qsettings->contains("browser/show_error_messages")) {
        qsettings->setValue("browser/show_error_messages", false);
    }


    if (!qsettings->contains("signals/enable")) {
        qsettings->setValue("signals/enable", true);
    }
    if (!qsettings->contains("signals/SIGUSR1")) {
        qsettings->setValue("signals/SIGUSR1", "");
    }
    if (!qsettings->contains("signals/SIGUSR2")) {
        qsettings->setValue("signals/SIGUSR2", "");
    }


    if (!qsettings->contains("inspector/enable")) {
        qsettings->setValue("inspector/enable", false);
    }
    if (!qsettings->contains("inspector/visible")) {
        qsettings->setValue("inspector/visible", false);
    }


    if (!qsettings->contains("event-sounds/enable")) {
        qsettings->setValue("event-sounds/enable", false);
    }
    if (!qsettings->contains("event-sounds/window-clicked")) {
        qsettings->setValue("event-sounds/window-clicked", RESOURCES"window-clicked.ogg");
    }
    if (!qsettings->contains("event-sounds/link-clicked")) {
        qsettings->setValue("event-sounds/link-clicked", RESOURCES"window-clicked.ogg");
    }

    if (!qsettings->contains("cache/enable")) {
        qsettings->setValue("cache/enable", false);
    }
    if (!qsettings->contains("cache/location")) {
#ifdef QT5
        QString location = QStandardPaths::writableLocation(QStandardPaths::CacheLocation);
#else
        QString location = QDesktopServices::storageLocation(QDesktopServices::CacheLocation);
#endif
        QDir d = QDir(location);
        location += d.separator();
        location += qsettings->value("application/name").toString();
        d.setPath(location);
        qsettings->setValue("cache/location", d.absolutePath());
    }
    if (!qsettings->contains("cache/size")) {
        qsettings->setValue("cache/size", 100*1000*1000);
    }
    if (!qsettings->contains("cache/clear-on-start")) {
        qsettings->setValue("cache/clear-on-start", false);
    }
    if (!qsettings->contains("cache/clear-on-exit")) {
        qsettings->setValue("cache/clear-on-exit", false);
    }

    if (!qsettings->contains("printing/enable")) {
        qsettings->setValue("printing/enable", false);
    }
    if (!qsettings->contains("printing/show-printer-dialog")) {
        qsettings->setValue("printing/show-printer-dialog", false);
    }
    if (!qsettings->contains("printing/printer")) {
        qsettings->setValue("printing/printer", "default");
    }
    if (!qsettings->contains("printing/page_margin_left")) {
        qsettings->setValue("printing/page_margin_left", 5);
    }
    if (!qsettings->contains("printing/page_margin_top")) {
        qsettings->setValue("printing/page_margin_top", 5);
    }
    if (!qsettings->contains("printing/page_margin_right")) {
        qsettings->setValue("printing/page_margin_right", 5);
    }
    if (!qsettings->contains("printing/page_margin_bottom")) {
        qsettings->setValue("printing/page_margin_bottom", 5);
    }

    if (!qsettings->contains("attach/javascripts")) {
        qsettings->setValue("attach/javascripts", "");
    }
    if (!qsettings->contains("attach/styles")) {
        qsettings->setValue("attach/styles", "");
    }
    if (!qsettings->contains("view/hide_mouse_cursor")) {
        qsettings->setValue("view/hide_mouse_cursor", false);
    }

    if (qsettings->fileName().length()) {
        qsettings->sync();
    }
}


bool QwkSettings::getBool(QString key, bool defval)
{
    if (qsettings) {
        if (qsettings->contains(key)) {
            return qsettings->value(key).toBool();
        }
    }
    return defval;
}

int QwkSettings::getInt(QString key, int defval)
{
    if (qsettings) {
        if (qsettings->contains(key)) {
            return qsettings->value(key).toInt();
        }
    }
    return defval;
}

uint QwkSettings::getUInt(QString key, uint defval)
{
    if (qsettings) {
        if (qsettings->contains(key)) {
            return qsettings->value(key).toUInt();
        }
    }
    return defval;
}

qreal QwkSettings::getReal(QString key, qreal defval)
{
    if (qsettings) {
        if (qsettings->contains(key)) {
            return qsettings->value(key).toReal();
        }
    }
    return defval;
}

QString QwkSettings::getQString(QString key, QString defval)
{
    if (qsettings) {
        if (qsettings->contains(key)) {
            return qsettings->value(key).toString();
        }
    }
    return defval;
}

QStringList QwkSettings::getQStringList(QString key, QStringList defval)
{
    if (qsettings) {
        if (qsettings->contains(key)) {
            return qsettings->value(key).toStringList();
        }
    }
    return defval;
}

void QwkSettings::setValue(QString key, QVariant value)
{
    if (qsettings) {
        qsettings->setValue(key, value);
    }
    return;
}
