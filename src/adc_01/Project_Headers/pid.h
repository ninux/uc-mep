/**
 *--------------------------------------------------------------------\n
 *          HSLU T&A Hochschule Luzern Technik+Architektur            \n
 *--------------------------------------------------------------------\n
 *
 * \brief         PID control
 * \file
 * \author        Christian Jost
 * \date          21.11.2012
 *
 * \b Language:   Ansi-C \n\n
 * \b Target:     MC Car \n
 *
 * $Id: pid.h 533 2013-11-24 19:07:45Z zajost $
 *--------------------------------------------------------------------
 */
#ifndef PID_H
#define PID_H

void pidSetSpeedL(int16 speed);
void pidSetSpeedR(int16 speed);
void pidSetSpeed(int16 speedL, int16 speedR);
void pidDoWork(void);

tError pidParseCommand(char *cmd);

void pidInit(void);

#endif /* PID_H */
