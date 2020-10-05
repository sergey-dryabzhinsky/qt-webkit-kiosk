/**
 * Add debug macros for QT < 5.5.0
 */

#ifndef __QINFO_H
#define __QINFO_H

#if QT_VERSION < 0x050500
#define qInfo       qDebug
#define qWarning    qDebug
#define qFatal      qDebug
#define qCritical   qDebug
#endif

#endif
