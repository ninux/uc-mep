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
 * $Id: player.h 517 2013-11-11 08:02:02Z zajost $
 *--------------------------------------------------------------------
 */
#ifndef PLAYER_H_
#define PLAYER_H_

bool soundIsPlaying(void);
void soundTooglePlayPause(void);
void soundStart(void);
void soundStop(void);
void soundPlay(const char *soundFile);

#endif /* PLAYER_H_ */
