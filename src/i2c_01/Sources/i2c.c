#include "platform.h" /* include peripheral declarations */
#include "i2c.h"

#define I2C_SUCCESS 1
#define I2C_FAIL 0

void i2c_init(void)
{
	// f = busclock/(mul*icr) = 24MHz/(2*30) = 400kHz
	IICF_MULT = 1;		// freq. multiplication factor = 2
	IICF_ICR = 0x05;	// I2C clock rate = 30
	
	IICC1_IICEN = 1;	// enable I2C module
}

int i2c_start(uint8 addr, bool write)
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
		IICD = ((addr<<1) & 0xFE);
	} else {			// [addr (7bit)] [R/W=1 (1bit)]	
		IICD = ((addr<<1) | 0x01);
	}
	
	while (!IICS_IICIF) {
		// wait till transmission is done
	}
	
	IICS_IICIF = 1;		// reset the I2C interrupt flag
	
	if (IICS_RXAK) {	// if NACK
		IICC1_MST = 0;	// generate stop condition
		IICS_IICIF = 1;	// reset the I2C interrupt flag
		return I2C_FAIL;
	}
	
	return I2C_SUCCESS;
}

int i2c_repeated_start(uint8 addr, bool write)
{
	IICC_RSTA = 1;		// generate repeated-start condition
	
	if (write) {		// [addr (7bit)] [R/W=0 (1bit)]
			IICD = ((addr<<1) & 0xFE);
		} else {		// [addr (7bit)] [R/W=1 (1bit)]	
			IICD = ((addr<<1) | 0x01);
		}
	
	while (!IICS_IICIF) {
		// wait till transmission is done
	}
	
	IICS_IICIF = 1;		// reset the I2C interrupt flag
	
	if (IICS_RXAK) {	// if NACK
		IICC1_MST = 0;	// generate stop condition
		IICS_IICIF = 1;	// reset the I2C interrupt flag
		return I2C_FAIL;
	}
}

int i2c_send_byte(uint8 databyte)
{
	IICD = databyte;
	
	while (!IICS_IICIF) {
		// wait till transmission is done
	}
	
	IICS_IICIF = 1;		// reset the I2C interrupt flag
	
	if (IICS_RXAK) {	// if NACK
		IICC1_MST = 0;	// generate stop condition
		IICS_IICIF = 1;	// reset the I2C interrupt flag
		return I2C_FAIL;
	}
	
	return I2C_SUCCESS;
}

void i2c_stop(void)
{
	IICC_MST = 0;		// generate stop condition
	IICS_IICIF = 1;		// reset the I2C interrupt flag
}


