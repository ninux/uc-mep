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
 * $Id: sound.c 527 2013-11-18 06:51:40Z zajost $
 *--------------------------------------------------------------------
 */
#include "platform.h"
#include "sound.h"

#define TIMER_FREQ      1000000                     		// Timer Counter Frequency = 1 MHz
#define TICKS_1_MS		1000

static volatile uint16 counterFreqTicks;                 	// number of timer ticks for the next compare value
static volatile uint16 counterDelayMs;                      // play time in milliseconds
static soundFinishedHandler soundFinishedCallback;  		// optional callback function if one Beep has been finished


/**
 * ISR: TPM1 Channel 2 - Output Compare
 * Realizes tone duration by counting down the
 * number of counterDelayMs
 */
interrupt void soundISRduration(void)
{
	// ToDo
	TPM1C2SC_CH2F = 0;
	TPM1C2V = TPM1C2V + TICKS_1_MS;
	
	if (counterDelayMs > 0)
	{
		counterDelayMs--;
	}
	else
	{
		// Soundausgabe stoppen...
		TPM1C2SC = 0;
		TPM1C5SC = 0;
		if (soundFinishedCallback != NULL) soundFinishedCallback();
	}
}

/**
 * ISR: TPM1 Channel 5 - Output Compare
 * Realizes tone frequency by toggling output pin 
 * every half period duration of current tone
 */
interrupt void soundISRfreq(void)
{
	// ToDo...
	TPM1C5SC_CH5F = 0;
	TPM1C5V += counterFreqTicks;
}

/**
 * plays a tone.
 * @param [in] freq the freqency [Hz]
 * @param [in] time the playing time in milliseconds
 */
void soundBeep(uint16 freq, uint16 timeMS)
{        
    if (freq > 0)
    {
        counterFreqTicks = (uint16)(1000000L / (2 * freq));

        // configure "freq" timer TPM1C5
        // ToDo...
        
        TPM1C5SC_MS5x = 1;		// set channel output compare mode
        TPM1C5SC_ELS5x = 1;		// toggle output on compare
        TPM1C5SC_CH5IE = 1;		// channel 5 interrupt enable
        TPM1C5V = TPM1CNT + counterFreqTicks;
    }
    
    // ToDo timer channel configuration 
    counterDelayMs = timeMS;
    TPM1C2SC_MS2x = 1;			// Set channel output compare mode
    TPM1C2SC_ELS2x = 0;			// don't use pin for timer channel
    TPM1C2SC_CH2IE = 1;			// enable channel 2 interrupt
    TPM1C2V = TPM1CNT + TICKS_1_MS;
}


void soundSetCallbackHandler(soundFinishedHandler h)
{
    soundFinishedCallback = h;
}

