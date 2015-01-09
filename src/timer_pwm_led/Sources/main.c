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

#define MODULO 23999
static uint16 value = 0;
static uint8 direction = 1;
static uint8 pause = 0;

void initPorts(void)
{
	PTFDD_PTFDD0 = 1;		// set pin 0 port F = OUT (LED B FL)
	PTFDD_PTFDD2 = 1;		// set pin 2 port F = OUT (LED R FR)
}

void timer1_init(void)
{
	TPM1SC_CLKSx = 1;		// bus clock (24MHz)
	TPM1SC_PS = 0;			// prescaler = 1 ---> T = 2.73ms, tick = 41.6ns
	TPM1MOD = MODULO;		// MODULO at 1ms (23999)
	TPM1SC_CPWMS = 0;		// edge alined
	TPM1SC_TOF = 0;			// reset the timer-overflow-flag
	TPM1SC_TOIE = 0;		// disable interrupts
}

void timer1_ch2_init(void)
{
	TPM1C2SC_MS2x = 2;		// edge aligned
	TPM1C2SC_ELS2x = 1;		// low-true pulse
	TPM1C2SC_CH2F = 0;		// clear channel flag
	TPM1C2SC_CH2IE = 1;		// enable interrupts
	TPM1C2V = MODULO/2;		// init with 50% duty cycle
}

void change_pwm(void);

interrupt timer1_ch2_isr(void)
{
	TPM1C2SC_CH2F = 0;		// clear channel flag
	change_pwm();			// change PWM duty cycle
}

void main(void) 
{	
	initPorts();
	timer1_init();
	timer1_ch2_init();
	EnableInterrupts;		// enable interrupts

    for(;;) 
    {   
    	__RESET_WATCHDOG();	// feeds the dog
    }
}

void change_pwm(void)
{
	if (pause < 5) {
			pause++;
		} else {
			pause = 0;
			PTFD_PTFD2 = ~PTFD_PTFD2;
			if (direction) {
				value += 240;
				TPM1C2V = value;
				if ((value) >= MODULO-240) {
					direction = 0;
				}
			} else {
				value -= 240;
				TPM1C2V = value;
				if (value < (MODULO/8)) {
					direction = 1;
				}
			}
		}
}
