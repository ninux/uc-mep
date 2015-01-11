/**
 *--------------------------------------------------------------------\n
 *          HSLU T&A Hochschule Luzern Technik+Architektur            \n
 *--------------------------------------------------------------------\n
 *
 * \brief         Sound player
 * \file
 * \author        Leandro Kressebuch, Christian Jost
 * \date          03.11.2014
 *
 * \b Language:   Ansi-C \n\n
 * \b Target:     MC Car \n
 *
 * $Id: sound.h 516 2013-11-11 07:12:01Z zajost $
 *--------------------------------------------------------------------
 */
#ifndef SOUND_H
#define SOUND_H

typedef void (*soundFinishedHandler)(void);

void soundSetCallbackHandler(soundFinishedHandler h);
void soundBeep(uint16 freq, uint16 ticks);

#endif /* SOUND_H_ */
