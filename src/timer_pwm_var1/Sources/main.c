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

void LED_init(void)
{
	PTFDD_PTFDD1 = 1;		// pin 1 port F = OUT (LED R FL)
	PTFD_PTFD1 = 0;			// LED R FL = ON
}

void timer1_init(void)
{
	TPM1SC_TOF = 0;			// reset the timer-overflow flag
	TPM1SC_TOIE = 0;		// disable timer interrupt
	TPM1SC_CLKSx = 2;		// fixed system clock (1MHz)
	TPM1SC_PS = 0;			// prescaler = 1 (tick = 1us)
	
	TPM1MOD = 20000-1;		// T = 20ms = 20000*1us
}

void timer1_ch3_init()
{
	TPM1C3SC_CH3F = 0;		// reset the channel-overflow flag
	TPM1C3SC_CH3IE = 0;		// disable channel interrupt
	TPM1C3SC_MS3x = 2;		// edge aligned
	TPM1C3SC_ELS3x = 10;	// high true pulse
	
	TPM1C3V = 1500;			// pulse = 1.5ms = 1500*1us
}

void main(void) 
{	
	LED_init();
	timer1_init();
	timer1_ch3_init();
	
    EnableInterrupts;		// enable interrupts

    for(;;) 
    {   
    	__RESET_WATCHDOG();	// feeds the dog
    }
}

