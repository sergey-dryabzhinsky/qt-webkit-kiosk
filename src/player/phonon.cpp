#include "../config.h"
#include "phonon.h"

QPlayer::QPlayer()
{
    player = NULL;
}

void QPlayer::play(QString soundFile)
{
    if (player == NULL) {
        audioOutput = new Phonon::AudioOutput(Phonon::MusicCategory, this);
        player = new Phonon::MediaObject(this);

        player->setTickInterval(1000);

        Phonon::createPath(player, audioOutput);
    }

    if (soundFile.length()) {
        player->stop();
        player->clearQueue();

        Phonon::MediaSource source(soundFile);

        player->setCurrentSource(source);
        player->play();
    }
}
