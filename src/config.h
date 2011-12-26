/**
 * All package definitions here
 */

#ifndef __CONFIG_H
#define __CONFIG_H

#ifdef Q_WS_WIN

#ifndef RESOURCES
    define RESOURCES "."
#else
    #undef RESOURCES
    define RESOURCES "."
#endif

#ifndef ICON
    define ICON "qt-webkit-kiosk.png"
#else
    #undef ICON
    define ICON "qt-webkit-kiosk.png"
#endif

#else

#ifndef RESOURCES
    define RESOURCES "."
#endif
#ifndef ICON
    define ICON "qt-webkit-kiosk.png"
#endif

#endif

#endif
