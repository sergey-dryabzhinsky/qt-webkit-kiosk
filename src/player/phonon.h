#ifndef QPLAYER_PHONON_H
#define QPLAYER_PHONON_H

#include <QtCore>
#include <phonon/audiooutput.h>
#include <phonon/mediaobject.h>
#include <phonon/backendcapabilities.h>

class QPlayer : public QObject
{
    Q_OBJECT

public:
    QPlayer();

    void play(QString soundFile);

protected:

private:

    Phonon::MediaObject *player;
    Phonon::AudioOutput *audioOutput;

};

#endif // QPLAYER_PHONON_H
