#ifndef QPLAYER_H
#define QPLAYER_H

#ifdef PLAYER_MULTIMEDIA
#include "player/multimedia.h"
#endif

#ifdef PLAYER_PHONON
#include "player/phonon.h"
#endif

#ifdef PLAYER_NULL
#include "player/null.h"
#endif

#endif // QPLAYER_H
