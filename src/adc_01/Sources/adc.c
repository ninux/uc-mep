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
 * $Id: adc_temp.c 547 2013-12-01 18:41:02Z zajost $
 *--------------------------------------------------------------------
 */
#include "platform.h"
#include "adc.h"

#define ADC_RES_8BIT      0
#define ADC_RES_10BIT     2
#define ADC_RES_12BIT     1

void ADC_complete(void)
{
	while (!ADCSC1_COCO) {
			// wait till conversion is done
	}
}

uint16 adcGet12BitValue(AdcChannels ch)
{
	ADCCFG_MODE = 1;			// 12-bit conversion mode
	ADCSC1_ADCH = (uint8)ch;	// select the AD channel and start conversion
	ADC_complete();				// wait for ADC completion
	return ADCR;
}

uint16 adcGet10BitValue(AdcChannels ch)
{
	ADCCFG_MODE = 2;			// 10-bit conversion mode
	ADCSC1_ADCH = (uint8)ch;	// select the AD channel and start conversion
	ADC_complete();				// wait for ADC completion
	return ADCR;
}

uint8 adcGet8BitValue(AdcChannels ch)
{
	ADCCFG_MODE = 0;			// 8-bit conversion mode
	ADCSC1_ADCH = (uint8)ch;	// select the AD channel and start conversion
	ADC_complete();				// wait for ADC completion
	return ADCRL;
}

void adcInit(void)
{
	// @ToDo implement function code
	ADCSC1_ADCO = 0;			// single conversion mode
		
	ADCSC2_ADTRG = 0;			// trigger conversion by software
	ADCSC2_ACFE = 0;			// disable comparator mode
	
	ADCCFG_ADLPC = 0;			// high speed configuration
	ADCCFG_ADLSMP = 1;			// long sample time
	ADCCFG_ADICLK = 0;			// bus clock (24MHz)
	ADCCFG_ADIV = 2;			// prescaler = 4 (24MHz/4 = 6MHz)
	
	APCTL1_ADPC4 = 1;			// enable ADC for channel 4
	APCTL1_ADPC5 = 1;			// enable ADC for channel 5
	APCTL1_ADPC6 = 1;			// enable ADC for channel 6
	APCTL1_ADPC7 = 1;			// enable ADC for channel 7
}
