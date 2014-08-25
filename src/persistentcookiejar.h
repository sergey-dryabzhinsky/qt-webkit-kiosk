#ifndef PERSISTENTCOOKIEJAR_H
#define PERSISTENTCOOKIEJAR_H

#include <QNetworkCookieJar>

class PersistentCookieJar : public QNetworkCookieJar
{
    Q_OBJECT
public:
    explicit PersistentCookieJar(QObject *parent = 0);

    bool setCookiesFromUrl(const QList<QNetworkCookie> &cookieList, const QUrl &url);
    void store();
    void load();
private:
    QString cookiejar_location;
    static const char *line_separator;
signals:

public slots:

};

#endif // PERSISTENTCOOKIEJAR_H
