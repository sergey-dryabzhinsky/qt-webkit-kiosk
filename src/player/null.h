#ifndef QPLAYER_NULL_H
#define QPLAYER_NULL_H

#include <QtCore>

class QPlayer : public QObject
{
    Q_OBJECT

public:
    QPlayer();

    void play(QString soundFile);

};

#endif // QPLAYER_NULL_H
