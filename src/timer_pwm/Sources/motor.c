/**
 *--------------------------------------------------------------------\n
 *          HSLU T&A Hochschule Luzern Technik+Architektur            \n
 *--------------------------------------------------------------------\n
 *
 * \brief         Motor driver
 * \file
 * \author        Christian Jost
 * \date          11.11.2013
 *
 * \b Language:   Ansi-C \n\n
 * \b Target:     MC Car \n
 *
 * $Id: motor_template.c 520 2013-11-11 09:55:21Z zajost $
 *--------------------------------------------------------------------
 */
#include <stdlib.h>
#include <string.h>
#include "platform.h"
#include "motor.h"

#define MODULO          128		// @ToDo calculate the correct timer modulo value

#define EngineR_A       PTDD_PTDD4
#define EngineR_B       PTDD_PTDD5
#define EngineL_A       PTDD_PTDD6
#define EngineL_B       PTDD_PTDD7

#define EngineR_A_Dir   PTDDD_PTDDD4
#define EngineR_B_Dir   PTDDD_PTDDD5
#define EngineL_A_Dir   PTDDD_PTDDD6
#define EngineL_B_Dir   PTDDD_PTDDD7

static int8 pwmLeft;
static int8 pwmRight;



/**
 * returns the pwm value of the left motor
 * 
 * There is nothing to do in this function!
 * 
 * @returns
 *    the value 0..+/-127
 */
int8 motorGetPWMLeft(void)
{
  return pwmLeft;
}


/**
 * returns the pwm value of the right motor
 * 
 * There is nothing to do in this function!
 * 
 * @returns
 *    the value 0..+/-127
 */
int8 motorGetPWMRight(void)
{
  return pwmRight;
}


/**
 * increments the speed of the left motor
 * 
 * There is nothing to do in this function!
 * 
 * @param [in] value
 *    the desired offset
 */
void motorIncrementPWMLeft(int8 value)
{
  int16 v = pwmLeft + value;
  if (v > 127) v = 127;
  if (v < -127) v = -127;
  motorSetPWMLeft((int8)v);
}


/**
 * increments the speed of the right motor
 * 
 * There is nothing to do in this function!
 * 
 * @param [in] value
 *    the desired offset
 */
void motorIncrementPWMRight(int8 value)
{
  int16 v = pwmRight + value;
  if (v > 127) v = 127;
  if (v < -127) v = -127;
  motorSetPWMRight((int8)v);
}


/**
 * sets the speed of the left motor
 * 
 * @param [in] value
 *  +1..+127 => speed in forward direction
 *  -1..-127 => speed in backward direction
 *  0 => stop
 */
void motorSetPWMLeft(int8 value)
{
  if(value < 0)               // backward
  {
    EngineL_A_Dir = 0;        // EngineL B = Input = PWM
    EngineL_B_Dir = 1;        // EngineL A = Output
    EngineL_B = 1; 

    // @ToDo set the correct channel value here
    TPM2C0V = (-1)*value;
    
  }
  else if(value > 0)          // forward
  {
    EngineL_A_Dir = 1;        // EngineL B = Output
    EngineL_B_Dir = 0;        // EngineL A = Input = PWM
    EngineL_A = 1;
    
    // @ToDo set the correct channel value here
    TPM2C0V = value;
    
  } 
  else                        // stop
  {
    EngineL_A_Dir = 1;        // EngineR A = Output
    EngineL_B_Dir = 1;        // EngineL B = Output
    EngineL_A = 1;
    EngineL_B = 1;
    
    // @ToDo set the correct channel value here
    TPM2C0V = 0;
  }
  pwmLeft = value;
}


/**
 * sets the speed of the right motor
 * 
 * @param [in] value
 *  +1..+127 => speed in forward direction
 *  -1..-127 => speed in backward direction
 *  0 => stop
 */
void motorSetPWMRight(int8 value)
{
  if(value < 0)               // rückwärts
  {
    EngineR_A_Dir = 1;        // EngineR A = Output
    EngineR_B_Dir = 0;        // EngineR B = Input = PWM
    EngineR_A = 1;
    
    // @ToDo set the correct channel value here
    TPM2C1V = (-1)*value;
  }
  else if(value > 0)          // vorwärts
  {
    EngineR_A_Dir = 0;        // EngineR A = Input = PWM
    EngineR_B_Dir = 1;        // EngineR B = Output
    EngineR_B = 1;
    
    // @ToDo set the correct channel value here
    TPM2C1V = value;
  } 
  else                        // stop
  {
    EngineR_A_Dir = 1;        // EngineR A = Output
    EngineR_B_Dir = 1;        // EngineR B = Output
    EngineR_A = 1;
    EngineR_B = 1;
    
    // @ToDo set the correct channel value here
    TPM2C1V = 0;
  }
  pwmRight = value;
}


/**
 * Initializes the motor driver as follows:
 * - TPM2: Clocksource = 24 MHZ, Prescale = 4
 * - TPM2CH0: edge aligned pwm with low-true pulses
 * - TPM2CH1: edge aligned pwm with low-true pulses 
 */
void motorInit(void)
{
  EngineL_A_Dir = 1;
  EngineL_B_Dir = 1;
  EngineR_A_Dir = 1;
  EngineR_B_Dir = 1;
  
  EngineR_A = 1;
  EngineR_B = 1;
  EngineL_A = 1;
  EngineL_B = 1;
  
  // @Todo complete TPM2, TPM2CH0 and TPM2CH1 init
  
  // timer 2 init
  TPM2MOD = MODULO;	//
  TPM2SC_TOF = 0;			// reset the overflow flag
  TPM2SC_TOIE = 0;			// disable interrupt
  TPM2SC_CPWMS = 0;			// edge-aligned PWM
  TPM2SC_CLKSx = 0b01;		// clock = bus clock (24MHz)
  TPM2SC_PS = 0b010;		// prescaler = 4 (24MHz/4 = 6MHz)
  
  // channel 0
  TPM2C0SC_CH0F = 0;		// reset the overflow flag;
  TPM2C0SC_CH0IE = 0;		// disable interrupt
  TPM2C0SC_MS0x = 0b10;		// PWM low-true pulse
  TPM2C0SC_ELS0x = 0b01;	// 
  TPM2C0V = 0;				// init with 0% duty cycle
  
  // channel 1
  TPM2C1SC_CH1F = 0;		// reset the overflow flag;
  TPM2C1SC_CH1IE = 0;		// disable interrupt
  TPM2C1SC_MS1x = 0b10;		// PWM low-true pulse
  TPM2C1SC_ELS1x = 0b01;	// 
  TPM2C1V = 0;				// init with 0% duty cycle
}
