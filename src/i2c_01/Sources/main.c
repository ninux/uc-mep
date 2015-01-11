#include "platform.h"
#include "i2c.h"
#include "qenc.h"

#define DEBUG
#undef DEBUG

void LED_init(void)
{
	PTFDD_PTFDD1 = 1;		// pin 1 port F = OUT (LED R FL)
	PTFD_PTFD1 = 0;			// LED R FL = ON
}

void main(void) 
{
	LED_init();
	i2c_init();
	
    EnableInterrupts;		// enable interrupts

    for(;;) 
    {   
    	PTFD_PTFD1 = 0;		// LED R FL = ON
    	PTFD_PTFD1 = 1;		// LED R FL = OFF
    	
    	qenc_init();
    	
    	__RESET_WATCHDOG();	// feeds the dog
    }
}

