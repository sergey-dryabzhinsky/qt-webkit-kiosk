#include "config.h"
#include "qplayer.h"

QPlayer::QPlayer()
{
    player = new QMediaPlayer();
}

void QPlayer::play(QString soundFile)
{
    if (soundFile.length()) {
        player->stop();
        player->setMedia(QUrl::fromLocalFile(soundFile));
        player->play();
    }
}
