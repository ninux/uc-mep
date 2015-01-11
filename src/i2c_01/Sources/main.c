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

#define DEBUG
#undef DEBUG
#define I2C_SUCCESS 1
#define I2C_FAIL 0

#define QENC_ADDR			0x2A
#define QENC_CTRL_ADDR		0x00
#define QENC_CTRL_CONFIG	0b00011101
#define QENC_CTRL_RESET		0x00

void LED_init(void)
{
	PTFDD_PTFDD1 = 1;		// pin 1 port F = OUT (LED R FL)
	PTFD_PTFD1 = 0;			// LED R FL = ON
}

void i2c_init(void)
{
	// f = busclock/(mul*icr) = 24MHz/(2*30) = 400kHz
	IICF_MULT = 1;		// freq. multiplication factor = 2
	IICF_ICR = 0x05;	// I2C clock rate = 30
	
	IICC1_IICEN = 1;	// enable I2C module
}

int i2c_start(uint8 slave_addr, bool write)
{	
	while (IICS_BUSY) {
		// wait till I2C-Bus is free
	}
	
	IICS_IICIF = 1;		// reset the I2C interrupt flag
	IICC1_TXAK = 1;		// no acknowledge will be sent
	
	IICC |= 0x30;		// generate start condition 
	// IICC_MST = 1;
	// IICC_TX = 1;
	
	if (write) {		// [addr (7bit)] [R/W=0 (1bit)]
		IICD = ((slave_addr<<1) & 0xFE);
	} else {			// [addr (7bit)] [R/W=1 (1bit)]	
		IICD = ((slave_addr<<1) | 0x01);
	}
	
	while (!IICS_IICIF) {
		// wait till transmission is done
	}
	
	IICS_IICIF = 1;		// reset the I2C interrupt flag
	
	#ifdef DEBUG
	
	#else
	if (IICS_RXAK) {	// if NACK
		IICC1_MST = 0;	// generate stop condition
		IICS_IICIF = 1;	// reset the I2C interrupt flag
		return I2C_FAIL;
	}
	#endif
	
	return I2C_SUCCESS;
}

int i2c_repeated_start(uint8 slave_addr, bool write)
{
	IICC_RSTA = 1;		// generate repeated-start condition
	
	if (write) {		// [addr (7bit)] [R/W=0 (1bit)]
			IICD = ((slave_addr<<1) & 0xFE);
		} else {		// [addr (7bit)] [R/W=1 (1bit)]	
			IICD = ((slave_addr<<1) | 0x01);
		}
	
	while (!IICS_IICIF) {
		// wait till transmission is done
	}
	
	IICS_IICIF = 1;		// reset the I2C interrupt flag
	
	#ifdef DEBUG
	
	#else
	if (IICS_RXAK) {	// if NACK
		IICC1_MST = 0;	// generate stop condition
		IICS_IICIF = 1;	// reset the I2C interrupt flag
		return I2C_FAIL;
	}
	#endif
}

int i2c_send_byte(uint8 databyte)
{
	IICD = databyte;
	
	while (!IICS_IICIF) {
		// wait till transmission is done
	}
	
	IICS_IICIF = 1;		// reset the I2C interrupt flag
	
	#ifdef DEBUG
	
	#else
	if (IICS_RXAK) {	// if NACK
		IICC1_MST = 0;	// generate stop condition
		IICS_IICIF = 1;	// reset the I2C interrupt flag
		return I2C_FAIL;
	}
	#endif
	
	return I2C_SUCCESS;
}

void i2c_stop(void)
{
	IICC_MST = 0;		// generate stop condition
	IICS_IICIF = 1;		// reset the I2C interrupt flag
}

int i2c_send_command(uint8 slave_addr, bool write, uint8 command)
{
	if (i2c_start(slave_addr, write)) {
		i2c_send_byte(command);
	}
}

void qenc_init(void)
{
	if (i2c_start(QENC_ADDR, 1)) {
	    if (i2c_send_byte(QENC_CTRL_ADDR)) {
	    	i2c_send_byte(QENC_CTRL_CONFIG);
	    }
	}
	i2c_stop();
}

void qenc_reset(void)
{
	if (i2c_start(QENC_ADDR, 1)) {
	    if (i2c_send_byte(QENC_CTRL_ADDR)) {
	    	i2c_send_byte(QENC_CTRL_RESET);
	    }
	}
	i2c_stop();
}

/**
 * main program
 */  
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
    	
    	__RESET_WATCHDOG();  	/* feeds the dog */
    }
    
  /* please make sure that you never leave main */
}

