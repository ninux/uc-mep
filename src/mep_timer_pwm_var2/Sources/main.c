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
#include "../Project_Headers/platform.h" /* include peripheral declarations */

void LED_init(void)
{
	PTFDD_PTFDD0 = 1;		// pin 1 port F = OUT (LED B FL)
	PTFDD_PTFDD1 = 1;		// pin 1 port F = OUT (LED R FL)
	PTFD_PTFD0 = 0;			// LED B FL = ON
	PTFD_PTFD1 = 0;			// LED R FL = ON
}

void timer1_init(void)
{	
	TPM1SC_CLKSx = 2;		// fixed system clock (1MHz)
	TPM1SC_PS = 0;			// prescaler = 1 (tick = 1us)
	TPM1SC_CPWMS = 0;		// edge aligned
	TPM1SC_TOF = 0;			// clear overflow flag
	TPM1SC_TOIE = 1;		// enable interrupt
	
	TPM1MOD = 20000-1;		// T = 20ms = 20000*1us
}

void timer1_ch3_init(void)
{	
	TPM1C3SC_MS3x = 1;		// 
	TPM1C3SC_ELS3x = 0;		// don't use channel pin
	TPM1C3SC_CH3F = 0;		// clear overflow flag
	TPM1C3SC_CH3IE = 1;		// enable channel interrupt
	
	TPM1C3V = 1500;			// pulse = 1.5ms = 1500*1us
}

interrupt void timer1_ISR(void)
{
	TPM1SC_TOF = 0;			// reset the timer-overflow flag
	PTFD_PTFD0 = 1;			// LED B FL = OFF
	PTFD_PTFD1 = 1;			// LED R FL = OFF
}

interrupt void timer1_ch3_ISR(void)
{
	TPM1C3SC_CH3F = 0;		// reset the overflow flag
	PTFD_PTFD0 = 0;			// LED B FL = ON
	PTFD_PTFD1 = 0;			// LED R FL = ON
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
