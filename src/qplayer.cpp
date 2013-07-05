#include "config.h"
#include "qplayer.h"

QPlayer::QPlayer()
{
    player = NULL;
}

void QPlayer::play(QString soundFile)
{
    if (player == NULL) {
        player = new QMediaPlayer();
    }

    if (soundFile.length()) {
        player->stop();
        player->setMedia(QUrl::fromLocalFile(soundFile));
        player->play();
    }
}
