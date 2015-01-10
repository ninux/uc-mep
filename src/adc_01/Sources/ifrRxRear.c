/*
 * ifrRx.c
 *
 *  Created on: 03.07.2012
 *      Author: zajost
 */

#include "platform.h"
#include "ifrRx.h"

/**
 * Sets a bit of the @ref rxBuf to '1'
 *
 * @param[in] buf
 *      the buffer to set a bit
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
 *      the buffer to clear a bit
 * @param[in] bytePos
 *      the desired byte [0..(rxBufSize - 1)]
 * @param[in] bitPos
 *      the desired bit [0...7]
 */
#define ClearRxBit(buf, bytePos, bitPos)    (buf[bytePos] &= ~(1 << bitPos))
           

typedef struct
{
  uint16 adr;
  uint8 cmd;
  //uint8 repeated;
} tIfrCommand;

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

static tCommand rxCommand;

#define rxBuf     (rxCommand.buf)


#define RX_PULSES_BUF_SIZE        16        //!< max pulses to receive
static uint8 rxPulsesCount;
static uint8 rxPulsesWritePos;
static uint8 rxPulsesReadPos;
static uint16 pulses[RX_PULSES_BUF_SIZE][2];

static rxStates rxState;

interrupt void ifrRearISR(void) // TPM1CH1
{  
  static uint16 oldTicks = 0;
  static uint16 pulse;  
  uint16 pause;
  OnEnterIfrRearISR(); // switch on led for debug purposes
    
  TPM1C1SC_CH1F = 0;
  
  if (PTED_PTED3)
  {
    // pulse
    pulse = TPM1C1V - oldTicks;
    oldTicks = TPM1C1V;
    OnExitIfrRearISR(); // switch off led for debug purposes
    return;
  }
  else
  {
    // pause
    pause = TPM1C1V - oldTicks;
    oldTicks = TPM1C1V;
  }
  
  if (rxState == rxReceived) return;
  
  if (rxPulsesCount < RX_PULSES_BUF_SIZE)
  {
    pulses[rxPulsesWritePos][0] = pulse;
    pulses[rxPulsesWritePos][1] = pause;
    rxPulsesWritePos++;
    if (rxPulsesWritePos >= RX_PULSES_BUF_SIZE) rxPulsesWritePos = 0;
    rxPulsesCount++;
  }
  OnExitIfrRearISR(); // switch off led for debug purposes  
}


void ifrRxDoWork(void)
{
  static uint8 bitPos = 0;
  static uint8 bytePos = 0;
  uint16 pulse, pause;
  rxBits rxBit;
  
  if (rxPulsesCount == 0) return;
  
  // read pulse/pause from the queue
  pulse = pulses[rxPulsesReadPos][0];
  pause = pulses[rxPulsesReadPos][1];
  rxPulsesReadPos++;
  if (rxPulsesReadPos >= RX_PULSES_BUF_SIZE) rxPulsesReadPos = 0;
  rxPulsesCount--;
  
  // decide which bit
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
}


char ifrRxRearGetKey(void)
{
  uint16 adr;
  uint8 cmd = 0;
  
  ifrRxDoWork();
  
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


void ifrRxRearInit(void)
{
	TPM1C1SC = 0x4c;    // Interrupt enable, input capture on both edges
}
