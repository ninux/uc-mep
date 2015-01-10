/**
 *--------------------------------------------------------------------\n
 *          HSLU T&A Hochschule Luzern Technik+Architektur            \n
 *--------------------------------------------------------------------\n
 *
 * \brief         Line following
 * \file
 * \author        Christian Jost, christian.jost@hslu.ch
 * \date          26.11.2012
 *
 * \b Language:   Ansi-C \n\n
 * \b Target:     MC-Car  \n
 *
 * \par Description:
 *
 * $Id: main.c 550 2013-12-02 06:57:57Z zajost $
 *--------------------------------------------------------------------
 */
#include "platform.h" 	/* include peripheral declarations */
#include "ifrRx.h"
#include "motor.h"		
#include "quad.h"
#include "pid.h"
#include "i2c.h"
#include "adc.h"
#include "linesens.h"
#include "sci.h"
#include "term.h"

static int16 offset;
static int16 speed;

/**
 * Switch on Rear LEDs on Port D2
 */
void initPorts(void)
{
    PTGDD = 0x00;       // Port G = Input
    PTGPE = 0xff;       // Port G = Pull-Up enable
}


/**
 * TPM1: Counter running with frequency 1 MHz
 * - No TOF interrupt
 * - Modulo = default
 * - Prescale = 1
 */
void initTimer(void)
{
  TPM1SC = 0x10;
}


void updatePID(void)
{
  int16 diff;
  
  if (speed == 0)
  {
    pidSetSpeed(0, 0);
    return;
  }
  
  // make sure that minimum speed of 150 mm/s is maintained
  if (offset > 0)
  {              // 200 -  60    => +10
    diff = 150 - (speed - offset); 
  }
  else
  {              // 200 -  -60   => +10
    diff = 150 - (speed + offset);
  }
   
  if (diff > 0)
  { 
    pidSetSpeed(speed + offset + diff, speed - offset + diff);
  }
  else
  {
    pidSetSpeed(speed + offset, speed - offset);
  }
}


void setSpeed(int8 incValue)
{
  static int8 v = 0;  
  if (incValue > 0) v++;
  else if (incValue < 0) v--;
  else v = 0;
  
  if (v < 0) v = 0;
  if (v > 5) v = 5;
  
  switch (v)
  {
    case 5 : speed = 450; break;
    case 4 : speed = 400; break;
    case 3 : speed = 350; break;
    case 2 : speed = 300; break;
    case 1 : speed = 250; break;    
    case 0 : speed = 0; break;
  }
  updatePID();
}

/**
 * main program
 */  
void main(void) 
{  
    tError e;
    int8 lineSensValue;
    
    initPorts();            // Port init
    initTimer();            // Timer init
    ifrRxFrontInit();       // Infrared init
    motorInit();            // Motor init
    i2cInit();              // I2C init
    pidInit();              // pid init
    adcInit();              // A/D init
    lsInit();               // line sensor init
    sci1Init(9600);
    sci2Init(9600); 
    EnableInterrupts;       // Interrupts enable
    
    termInit();
    
    TPM1C3SC = 0x10;
    
    e = quadSetMode(qmEncoder);
    
    for(;;) 
    {
        if (TPM1C3SC_CH3F)
        {
            TPM1C3SC_CH3F = 0;
            TPM1C3V += 20000;
            lineSensValue = lsGetDir();
            
            if (lineSensValue != LS_NO_SIGNAL)
            {
                // convert line sensor value to direction offset
                // with a factor 12 (empirically determined)
                offset = -lineSensValue * 25;
            }
            
            if (speed != 0) updatePID();
            pidDoWork();
            
            // calibration
            if ((PTGD & 0x07) == 3)
            {
                // joytick key right
                lsSetWhite();
            }
            
            if ((PTGD & 0x07) == 5)
            {
                // joystick key left
                lsSetBlack();
            }
            termDoWork();      
        }
        
        switch (ifrRxFrontGetKey())
        {     
            case 'S' : setSpeed(0); break;
            case 'W' : setSpeed(1); break;
            case 'T' : setSpeed(-1); break; 
        }   
    }
}
