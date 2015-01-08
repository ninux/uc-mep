/**
 *--------------------------------------------------------------------\n
 *          HSLU T&A Hochschule Luzern Technik+Architektur            \n
 *--------------------------------------------------------------------\n
 *
 * \brief         C Template for the MC Car
 * \file
 * \author        Christian Jost, christian.jost@hslu.ch
 * \date          12.09.2014
 *
 * \b Language:   Ansi-C \n\n
 * \b Target:     MC-Car  \n
 *
 * \par Description:
 *
 * $Id: main.c 904 2014-09-12 10:58:43Z chj-hslu $
 *--------------------------------------------------------------------
 */
#include "platform.h" /* include peripheral declarations */

// #define TIME_1000MS 62500
#define TIME_1000MS 0xF424
#define TIME_1000MS_L 0x24
#define TIME_1000MS_H 0xF4

void LED_init(void)
{
	PTFDD = PTFDD | 0x01;	// set pin 0 port F = OUT (LED B FL)
	PTFD = PTFD & 0xFE;		// set pin 0 port F = ON
	
	PTFDD_PTFDD2 = 1;		// set pin 2 port F = OUT (LED R RF)
	PTFD_PTFD2 = 1;			// set pin 2 port F = OFF
}

void timer1_init(void)
{
	// TPM1SC_CLKSx = 3;	// clock source = fixed internal 1MHz
	TPM1SC_CLKSA = 0;
	TPM1SC_CLKSB = 1;
	
	// TPM1SC_PS = 4;		// prescaler = 16
	TPM1SC_PS0 = 0;
	TPM1SC_PS1 = 0;
	TPM1SC_PS2 = 1;
	
	// TPM1MOD = 0;			// set MODULO to zero
	TPM1MODL = 0;
	TPM1MODH = 0;
	
	if (TPM1SC_TOF) {		// reset TOF if needed
		TPM1SC_TOF = 0;
	}
}

void timer1_ch2_init(void)
{
	// TPM1C2V = TIME_1000MS;	// channel value = 1s
	TPM1C2VL = TIME_1000MS_L;
	TPM1C2VH = TIME_1000MS_H;
	
	// TPM1C2SC_MS2x = 1;		// output compare
	TPM1C2SC_MS2A = 1;
	TPM1C2SC_MS2B = 0;
	
	// TPM1C2SC_ELS2x = 1;		// toggle output
	TPM1C2SC_ELS2A = 1;
	TPM1C2SC_ELS2B = 0;
	
	TPM1C2SC_CH2F = 0;			// reset interrupt channel flag

	TPM1C2SC_CH2IE = 1;			// enable interrupt
}

interrupt timer1_ch2_update(void)
{
	TPM1C2SC_CH2F = 0;			// reset interrupt channel flag
	
	TPM1C2V += TIME_1000MS;		// set the new value
	
	PTFD_PTFD2 = ~PTFD_PTFD2;	// toggle LED R RF
}

void main(void) 
{
	LED_init();
	timer1_ch2_init();
	timer1_init();
	
    EnableInterrupts;			// enable interrupts

    for(;;) 
    {
    	__RESET_WATCHDOG();		// feeds the dog
    }
}
