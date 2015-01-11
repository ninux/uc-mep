/**
 *--------------------------------------------------------------------\n
 *          HSLU T&A Hochschule Luzern Technik+Architektur            \n
 *--------------------------------------------------------------------\n
 *
 * \brief         Encoder driver
 * \file
 * \author        Christian Jost
 * \date          19.11.2012
 *
 * \b Language:   Ansi-C \n\n
 * \b Target:     MC Car \n
 *
 * $Id: quad.h 525 2013-11-14 15:58:53Z zajost $
 *--------------------------------------------------------------------
 */
#ifndef QUAD_H
#define QUAD_H

typedef enum
{
  qmOff = 0,
  qmEncoder = 1,
  qmCalibration = 2,
  qmUnkownError = 127
} tQuadMode;


typedef struct
{
  int16 speedLeft;
  int16 speedRight;
} tQuadSpeed, *pQuadSpeed;

typedef struct
{
  int16 ticksLeft;
  int16 ticksRight;
} tQuadTicks, *pQuadTicks;

typedef struct
{
  uint8 errorsLeft;
  uint8 errorsRight;
} tQuadErrors, *pQuadErrors;


tQuadMode quadGetMode(void);
tError quadSetMode(tQuadMode mode);

tError quadGetSpeed(pQuadSpeed speed);

tError quadGetTicks(pQuadTicks ticks);
tError quadResetTicks(void);

tError quadGetErrors(pQuadErrors errors);
tError quadResetErrors(void);

#endif /* QUAD_H_ */
