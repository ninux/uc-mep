/**
 *--------------------------------------------------------------------\n
 *          HSLU T&A Hochschule Luzern Technik+Architektur            \n
 *--------------------------------------------------------------------\n
 *
 * \brief         Line sensor driver
 * \file
 * \author        Christian Jost, christian.jost@hslu.ch
 * \date          26.11.2012
 *
 * \b Language:   Ansi-C \n\n
 * \b Target:     MC-Car  \n
 *
 * \par Description:
 *
 * $Id: linesens.h 547 2013-12-01 18:41:02Z zajost $
 *--------------------------------------------------------------------
 */
#ifndef LINESENS_H
#define LINESENS_H

#include "platform.h"

#define LS_NO_SIGNAL    (-128)

void lsDoScan(void);
int8 lsGetDir(void);

void lsSetWhite(void);
void lsSetBlack(void);

tError lsParseCommand(char *cmd);

tError lsWriteStatus(void);

tError lsParseCommand(char *cmd);

void lsInit(void);

#endif /* LINESENS_H_ */
