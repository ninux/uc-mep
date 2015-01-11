/**
 *--------------------------------------------------------------------\n
 *          HSLU T&A Hochschule Luzern Technik+Architektur            \n
 *--------------------------------------------------------------------\n
 *
 * \brief         A/D converter driver
 * \file
 * \author        Christian Jost, christian.jost@hslu.ch
 * \date          26.11.2012
 *
 * \b Language:   Ansi-C \n\n
 * \b Target:     MC-Car  \n
 *
 * \par Description:
 *
 * $Id: adc.h 533 2013-11-24 19:07:45Z zajost $
 *--------------------------------------------------------------------
 */
#ifndef ADC_H_
#define ADC_H_

#include "platform.h"

typedef enum 
{
  adcLsL = 7,
  adcLsML = 6,
  adcLsMR = 5,
  adcLsR = 4,
  adcChargeStatus = 10,
  adcIMonitor = 8,
  adcUMonitor = 9,
  adcBtStatus = 3
} AdcChannels;



uint16 adcGet12BitValue(AdcChannels ch);
uint16 adcGet10BitValue(AdcChannels ch);
uint8 adcGet8BitValue(AdcChannels ch);
void adcInit(void);


#endif /* ADC_C_ */
