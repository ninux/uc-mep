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

void initPorts(void)
{
	PTFDD = PTFDD | 0x01;	// set pin 0 port F = OUT (LED B FL)
	PTFDD = PTFDD | 0x02;	// set pin 1 port F = OUT (LED R FL)
	PTCDD = PTCDD | 0x0F;	// set pin 4 port C = OUT (LED G FL)
}

void set_B_FL(void)
{
	PTFD = PTFD & 0x01;	// set pin 0 port F = ON
	PTFD = PTFD | 0x02;	// set pin 1 port F = OFF
	PTCD = PTCD | 0x0F;	// set pin 4 port C = OFF
}

void main(void) 
{	
	initPorts();
	set_B_FL();
	EnableInterrupts;	// enable interrupts

    for(;;) 
    {   
    	__RESET_WATCHDOG();	// feeds the dog
    }
}

