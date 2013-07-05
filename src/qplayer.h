#ifndef QPLAYER_H
#define QPLAYER_H

#include <QtCore>
#include <QtMultimedia/QMediaPlayer>

class QPlayer : public QObject
{
    Q_OBJECT

public:
    explicit QPlayer();

    void play(QString soundFile);

protected:

private:

    QMediaPlayer *player;

};

#endif // QPLAYER_H
