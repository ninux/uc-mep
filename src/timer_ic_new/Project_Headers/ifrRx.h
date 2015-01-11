/**
 *--------------------------------------------------------------------\n
 *          HSLU T&A Hochschule Luzern Technik+Architektur            \n
 *--------------------------------------------------------------------\n
 *
 * \brief         IR receiver
 * \file
 * \author        Christian Jost, christian.jost@hslu.ch
 * \date          11.10.2012
 *
 * \b Language:   Ansi-C \n\n
 * \b Target:     MC Car \n
 *
 * $Id: ifrRx.h 516 2013-11-11 07:12:01Z zajost $
 *--------------------------------------------------------------------
 */

#ifndef IFRRX_H_
#define IFRRX_H_

#include "platform.h"


#define TIMER_FREQ               1000000.0

#define START_BIT_PULSE_TIME        9000.0e-6           // 9000 us pulse
#define START_BIT_PAUSE_TIME        4500.0e-6           // 4500 us pause
#define RSTART_BIT_PAUSE_TIME       2250.0e-6           // 2250 us pause
#define PULSE_TIME                   560.0e-6           //  560 us pulse
#define PAUSE_1_TIME                1690.0e-6           // 1690 us pause
#define PAUSE_0_TIME                 560.0e-6           //  560 us pause

#define MIN_TOLERANCE                 0.7				// -30%
#define MAX_TOLERANCE                 1.3				// +30%

// Number of timer ticks for each symbol
#define START_BIT_PULSE_LEN_MIN     ((uint16)(TIMER_FREQ * START_BIT_PULSE_TIME * MIN_TOLERANCE + 0.5)-1)
#define START_BIT_PULSE_LEN_MAX     ((uint16)(TIMER_FREQ * START_BIT_PULSE_TIME * MAX_TOLERANCE + 0.5)-1)
#define START_BIT_PAUSE_LEN_MIN     ((uint16)(TIMER_FREQ * START_BIT_PAUSE_TIME * MIN_TOLERANCE + 0.5)-1)
#define START_BIT_PAUSE_LEN_MAX     ((uint16)(TIMER_FREQ * START_BIT_PAUSE_TIME * MAX_TOLERANCE + 0.5)-1)
#define RSTART_BIT_PAUSE_LEN_MIN    ((uint16)(TIMER_FREQ * RSTART_BIT_PAUSE_TIME * MIN_TOLERANCE + 0.5)-1)
#define RSTART_BIT_PAUSE_LEN_MAX    ((uint16)(TIMER_FREQ * RSTART_BIT_PAUSE_TIME * MAX_TOLERANCE + 0.5)-1)
#define PULSE_LEN_MIN               ((uint16)(TIMER_FREQ * PULSE_TIME * MIN_TOLERANCE + 0.5)-1)
#define PULSE_LEN_MAX               ((uint16)(TIMER_FREQ * PULSE_TIME * MAX_TOLERANCE + 0.5)-1)
#define PAUSE_1_LEN_MIN             ((uint16)(TIMER_FREQ * PAUSE_1_TIME * MIN_TOLERANCE + 0.5)-1)
#define PAUSE_1_LEN_MAX             ((uint16)(TIMER_FREQ * PAUSE_1_TIME * MAX_TOLERANCE + 0.5)-1)
#define PAUSE_0_LEN_MIN             ((uint16)(TIMER_FREQ * PAUSE_0_TIME * MIN_TOLERANCE + 0.5)-1)
#define PAUSE_0_LEN_MAX             ((uint16)(TIMER_FREQ * PAUSE_0_TIME * MAX_TOLERANCE + 0.5)-1)

char ifrRxFrontGetKey(void);
void ifrRxFrontInit(void);

char ifrRxRearGetKey(void);
void ifrRxRearInit(void);

#endif /* IFRRX_H_ */
