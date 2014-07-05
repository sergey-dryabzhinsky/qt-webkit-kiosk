#ifndef QPLAYER_MULTIMEDIA_H
#define QPLAYER_MULTIMEDIA_H

/**
 * Sould be used only with Qt-5.0+
 * Qt 4 don't have MediaPlayer
 */

#include <QtCore>
#ifdef QT5
#include <QtMultimedia/QMediaPlayer>
#endif

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

#endif // QPLAYER_MULTIMEDIA_H
