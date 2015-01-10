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
 * $Id: ifrRxFront.c 527 2013-11-18 06:51:40Z zajost $
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
 *  
 */
interrupt void ifrFrontISR(void) // TPM1CH0
{
  static uint16 oldTicks = 0;
  static uint16 pulse, pause;
  static uint8 bitPos = 0;
  static uint8 bytePos = 0;
  rxBits rxBit;
  OnEnterIfrFrontISR(); // switch on led for debug purposes  
  TPM1C0SC_CH0F = 0;
  
  if (PTED_PTED2)
  {
    // pulse
    pulse = TPM1C0V - oldTicks;
    oldTicks = TPM1C0V;
    OnExitIfrFrontISR(); // switch off led for debug purposes
    return;
  }
  else
  {
    // pause
    pause = TPM1C0V - oldTicks;
    oldTicks = TPM1C0V;
  }

  if (pulse > PULSE_LEN_MIN && pulse < PULSE_LEN_MAX) // pulse ~560us
  {
    if (pause > PAUSE_0_LEN_MIN && pause < PAUSE_0_LEN_MAX) rxBit = rx0Bit;     // pause ~560us
    else if (pause > PAUSE_1_LEN_MIN && pause < PAUSE_1_LEN_MAX)rxBit = rx1Bit; // pause ~1690us
  }
  else if (pulse > START_BIT_PULSE_LEN_MIN && pulse < START_BIT_PULSE_LEN_MAX) // pulse ~9ms
  {
    if (pause > START_BIT_PAUSE_LEN_MIN && pause < START_BIT_PAUSE_LEN_MAX) rxBit = rxStartBit;         // pause ~4500us
    else if (pause > RSTART_BIT_PAUSE_LEN_MIN && pause < RSTART_BIT_PAUSE_LEN_MAX) rxBit = rxRStartBit; // pause ~2250us
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
  OnExitIfrFrontISR(); // switch off led for debug purposes
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
      case 0x01 : cmd = 'S'; break;
      case 0x02 : cmd = 'W'; break;
      case 0x03 : cmd = 'T'; break;
      case 0x05 : cmd = '+'; break;
      case 0x04 : cmd = '-'; break;
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
  TPM1C0SC = 0x4c;    // Interrupt enable, input capture on both edges
  
//  TPM1C0SC_MS0x = 0;    // channel 0 => input capture mode
//  TPM1C0SC_ELS0x = 2;   // input capture on both edges
//  TPM1C0SC_CH0IE = 1;   // interrupt enable
}
