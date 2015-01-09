/**
 *--------------------------------------------------------------------\n
 *          HSLU T&A Hochschule Luzern Technik+Architektur            \n
 *--------------------------------------------------------------------\n
 *
 * \brief         Front IR receiver
 * \file
 * \author        Christian Jost, christian.jost@hslu.ch
 * \date          07.11.2013
 *
 * \b Language:   Ansi-C \n\n
 * \b Target:     MC Car \n
 *
 * $Id: ifrRxFront_template.c 514 2013-11-11 07:08:13Z zajost $
 *--------------------------------------------------------------------
 */
#include "platform.h"
#include "ifrRx.h"

/**
 * Sets a bit of the @ref rxBuf to '1'
 *
 * @param[in] buf
 *      the desired buffer to set the bit
 * @param[in] bytePos
 *      the desired byte [0..(rxBufSize - 1)]
 * @param[in] bitPos
 *      the desired bit [0...7]
 */
#define SetRxBit(buf, bytePos, bitPos)      (buf[bytePos] |= (1 << bitPos))

/**
 * Sets a bit of the @ref rxBuf to '0'
 *
 * @param[in] buf
 *      the desired buffer to clear the bit
 * @param[in] bytePos
 *      the desired byte [0..(rxBufSize - 1)]
 * @param[in] bitPos
 *      the desired bit [0...7]
 */
#define ClearRxBit(buf, bytePos, bitPos)    (buf[bytePos] &= ~(1 << bitPos))
           

typedef enum
{
  rxIdle,
  rxReceiving,
  rxReceived
} rxStates;

typedef enum 
{
  rxStartBit,
  rxRStartBit,
  rx1Bit,
  rx0Bit
  
} rxBits;

typedef union
{
  uint8 buf[4];
  struct 
  {
    uint16 adr;
    uint8 cmd;
    uint8 cmdN;
  } cmd;
} tCommand;

#define rxBuf     (rxCommand.buf)
static tCommand rxCommand;
static rxStates rxState;

/**
 * Front infrared receiver interrupt service routine
 * This ISR is called on every edge of the input signal.
 */
interrupt void ifrFrontISR(void) // TPM1CH0
{
	static uint16 oldTicks = 0;
	static uint16 pulse, pause;
	static uint8 bitPos = 0;
	static uint8 bytePos = 0;
	rxBits rxBit;
	
	PTFD_PTFD1 = 0; // switch on led for debug purposes

	// @ToDo 2.) implement time measuring of pulse and pause 
	// pulse = ... [ticks]
	// pause = ... [ticks]
	
	TPM1C0SC_CH0F = 0;
	PTED_PTED2 = 0;
	
	if (PTED_PTED2) {
		pulse = TPM1C0V - oldTicks;
		oldTicks = TPM1C0V;
	} else {
		pause = TPM1C0V - oldTicks;
		oldTicks = TPM1C0V;
	}


	// the function code below this line is correct and works!
	  if (pulse > PULSE_LEN_MIN && pulse < PULSE_LEN_MAX)
	  {
	    if (pause > PAUSE_0_LEN_MIN && pause < PAUSE_0_LEN_MAX) rxBit = rx0Bit;
	    else if (pause > PAUSE_1_LEN_MIN && pause < PAUSE_1_LEN_MAX)rxBit = rx1Bit;
	  }
	  else if (pulse > START_BIT_PULSE_LEN_MIN && pulse < START_BIT_PULSE_LEN_MAX)
	  {
	    if (pause > START_BIT_PAUSE_LEN_MIN && pause < START_BIT_PAUSE_LEN_MAX) rxBit = rxStartBit;
	    else if (pause > RSTART_BIT_PAUSE_LEN_MIN && pause < RSTART_BIT_PAUSE_LEN_MAX) rxBit = rxRStartBit;
	  }
	  
	  switch (rxState)
	  {
	    case rxIdle:
	      if (rxBit == rxStartBit)
	      {
	        bitPos = bytePos = 0;
	        rxState = rxReceiving;
	      }
	      else if (rxBit == rxRStartBit) { /* Repeated Start */ }
	      break;
	  
	    case rxReceiving:
	      if (rxBit == rxStartBit || rxBit == rxRStartBit)
	      {
	        bitPos = bytePos = 0;
	      }
	      else
	      {
	        if (rxBit == rx1Bit) SetRxBit(rxBuf, bytePos, bitPos);
	        else ClearRxBit(rxBuf, bytePos, bitPos);
	        
	        if (++bitPos > 7)
	        {
	          bitPos = 0;
	          bytePos++;
	          if (bytePos >= 4)
	          {
	            if ((rxCommand.cmd.cmd ^ rxCommand.cmd.cmdN) == 0xff) rxState = rxReceived;
	            else rxState = rxIdle;
	          }
	        }
	      }
	      break;
	      
	    case rxReceived: break;

	    default: rxState = rxIdle; break;
	  }
	PTFD_PTFD1 = 1; // switch off led for debug purposes 
	}


/**
 * Returns a key or 0 if no key was pressed.
 */
uint8 ifrRxFrontGetKey(void)
{
  volatile uint16 adr; // not used
  uint8 cmd = 0;
  
  if (rxState == rxReceived)
  {
    adr = rxCommand.cmd.adr;
    switch (rxCommand.cmd.cmd)
    {
// @ToDo 3.) complete cases of the switch statement      
    	case 1 : cmd = 'S'; break;
    	case 2 : cmd = 'W'; break;
    	case 3 : cmd = 'T'; break;
    	case 5 : cmd = '+'; break;
    	case 4 : cmd = '-'; break;
    }
    rxState = rxIdle;
  }
  return cmd;
}


/**
 * This function configures the TPM1 channel 0 as follows:
 * - input capture on both edges
 * - with interrupt enabled
 */
void ifrRxFrontInit(void)
{
	// @ToDo
	// TPM1C0SC_MS0x = 0;		// input capture
	TPM1C0SC_MS0A = 0;
	TPM1C0SC_MS0B = 0;
	
	// TPM1C0SC_ELS0x = 0x03;	// both edges
	TPM1C0SC_ELS0A = 1;
	TPM1C0SC_ELS0B = 1;
	
	TPM1C0SC_CH0IE = 1;			// enable interrupt
}
