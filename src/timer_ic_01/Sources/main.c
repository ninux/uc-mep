#include "platform.h" /* include peripheral declarations */

// The MC-Car has to be hacked for this code!

static uint16 oldTicks = 0;
static uint16 time;
static const uint16 debounce_time = 250;
static uint8 debounce = 0;

void joystick_init(void)
{
	PTGDD_PTGDD2 = 0;		// pin 2 port F = IN
	PTGD_PTGD2 = 0;			// pullup disable
	
	PTFDD_PTFDD3 = 0;		// pin 3 port F = IN (Joystick PB2)
	PTFPE_PTFPE3 = 1;		// pullup-enable
}

void LED_init(void)
{
	PTFDD_PTFDD1 = 1;		// pin 1 port F = OUT (LED R FL)
	PTFD_PTFD1 = 1;			// LED R FL = OFF
}

void timer1_init(void)
{
	TPM1SC_TOF = 0;			// clear the timer flag
	TPM1SC_TOIE = 0;		// disable timer-interrupt
	TPM1SC_CPWMS = 0;		// edge aligned
	TPM1SC_CLKSx = 2;		// fixed system clock (1MHz)
	TPM1SC_PS = 7;			// prescaler = 7 (tick = 128us)
}

void timer1_ch3_init(void)	// used to generate the LED toggle
{
	TPM1C3SC_CH3F = 0;		// clear channel flag
	TPM1C3SC_CH3IE = 1;		// enable interrupt
	TPM1C3SC_MS3x = 1;		// output compare
	TPM1C3SC_ELS3x = 1;		// toggle channel pin
}

void timer1_ch5_init(void)	// used to measure the time
{
	TPM1C5SC_CH5F = 0;		// clear the channel-overflow flag
	TPM1C5SC_CH5IE = 1;		// enable channel-interrupt
	TPM1C5SC_MS5x = 0;		// input capture
	TPM1C5SC_ELS5x = 3;		// rising & falling edge
}

interrupt void timer1_ch3_ISR(void)
{
	TPM1C3SC_CH3F = 0;			// reset the channel flag
	TPM1C3V = TPM1CNT + time;	// set the new time
}

interrupt void timer1_ch5_ISR(void)
{
	TPM1C5SC_CH5F = 0;			// reset the channel flag
	if (!debounce) {
		if (!PTFD_PTFD3) {
			oldTicks = TPM1C5V;
		} else {
			time = TPM1C5V - oldTicks;
		}
	} else {
		// wait for debounce
	}
}
 
void main(void) 
{
	joystick_init();
	LED_init();
	timer1_init();
	timer1_ch3_init();
	timer1_ch5_init();
	
    EnableInterrupts;			// enable interrupts

    for(;;) 
    {   
    	__RESET_WATCHDOG();		// feeds the dog
    }
    
  /* please make sure that you never leave main */
}

