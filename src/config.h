/**
 * All package definitions here
 */

#ifndef __CONFIG_H
#define __CONFIG_H

#ifdef QT_NO_DEBUG
    #define QT_NO_DEBUG_OUTPUT
#endif

#ifndef RESOURCES
    #define RESOURCES "./resources"
#endif

#ifndef ICON
    #define ICON "./qt-webkit-kiosk.png"
#endif

#ifndef VERSION
    #define VERSION "1.03.01"
#endif

#endif
