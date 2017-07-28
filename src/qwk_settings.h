#ifndef QWK_SETTINGS_H
#define QWK_SETTINGS_H

#include <QtCore>

class QwkSettings : public QObject
{
    Q_OBJECT

public:
    explicit QwkSettings(QObject* parent = 0);

    void loadSettings(QString ini_file);

    bool        getBool(QString key, bool defval=false);
    int         getInt(QString key, int defval=0);
    uint        getUInt(QString key, uint defval=0);
    qreal       getReal(QString key, qreal defval=0);
    QString     getQString(QString key, QString defval=QString());
    QStringList getQStringList(QString key, QStringList defval=QStringList());

    void setValue(QString key, QVariant value);

protected:

    QSettings   *qsettings;
};

#endif // QWK_SETTINGS_H
