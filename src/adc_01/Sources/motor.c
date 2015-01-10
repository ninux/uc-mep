/**
 *--------------------------------------------------------------------\n
 *          HSLU T&A Hochschule Luzern Technik+Architektur            \n
 *--------------------------------------------------------------------\n
 *
 * \brief         Motor driver
 * \file
 * \author        Christian Jost
 * \date          15.11.2012
 *
 * \b Language:   Ansi-C \n\n
 * \b Target:     MC Car \n
 *
 * $Id: motor.c 532 2013-11-24 17:45:19Z zajost $
 *--------------------------------------------------------------------
 */
#include <stdlib.h>
#include <string.h>
#include "platform.h"
#include "motor.h"

#define MOTOR_DRIVER_VERSION    3 // 1 => Studentenversion mit Slow Decay
                                  // 2 => mit Offset und Decay Einstellung
                                  // 3 => wie 2 plus Booster

#if MOTOR_DRIVER_VERSION == 1

#define MODULO          (127-1)

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
    EngineL_A_Dir = 0;        // EngineL A = Input = PWM
    EngineL_B_Dir = 1;        // EngineL B = Output
    EngineL_B = 1; 
    TPM2C1V = -value;
  }
  else if(value > 0)          // forward
  {
    EngineL_A_Dir = 1;        // EngineL A = Output
    EngineL_B_Dir = 0;        // EngineL B = Input = PWM
    EngineL_A = 1;
    TPM2C1V = value; 
  } 
  else                        // stop
  {
    EngineL_A_Dir = 1;        // EngineR A = Output
    EngineL_B_Dir = 1;        // EngineL B = Output
    EngineL_A = 1;
    EngineL_B = 1;
    TPM2C1V = 0;
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
    TPM2C0V = -value;
  }
  else if(value > 0)          // vorwärts
  {
    EngineR_A_Dir = 0;        // EngineR A = Input = PWM
    EngineR_B_Dir = 1;        // EngineR B = Output
    EngineR_B = 1;
    TPM2C0V = value; 
  } 
  else                        // stop
  {
    EngineR_A_Dir = 1;        // EngineR A = Output
    EngineR_B_Dir = 1;        // EngineR B = Output
    EngineR_A = 1;
    EngineR_B = 1;
    TPM2C0V = 0;
  }
  pwmRight = value;
}


/**
 * Initializes the motor driver as follows:
 * - TPM2: Clocksource = 24 MHZ, Prescale = 4
 * - TPM2CH0: edge aligned pwm with low-true pulses
 * - TPM2Ch1: edge aligned pwm with low-true pulses 
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
  
  TPM2MOD = MODULO;   // set PWM frequency.
  
  // PWM motor right
  TPM2C0SC = 0x24;    // edge aligned pwm, low-true pulses       
     
  // PWM motor left
  TPM2C1SC = 0x24;    // edge aligned pwm, low-true pulses
  
  // Start timer
  TPM2SC = 0x0A;      // Busclock, Prescaler=4  
}


#elif MOTOR_DRIVER_VERSION == 2

#define DECAY            1    // 1 slow decay, 0 = fast decay

#define MOTOR_OFFSET    34    //  0 => pwm freq = 47 kHz, 
                              // 60 => pwm freq = 18.9 kHz
                              // 75 => pwm freq = 11.8 kHz

#define MODULO          ((-12700 / (MOTOR_OFFSET-100))) 

#define EngineR_A       PTDD_PTDD4
#define EngineR_B       PTDD_PTDD5
#define EngineL_A       PTDD_PTDD6
#define EngineL_B       PTDD_PTDD7

#define EngineR_A_Dir   PTDDD_PTDDD4
#define EngineR_B_Dir   PTDDD_PTDDD5
#define EngineL_A_Dir   PTDDD_PTDDD6
#define EngineL_B_Dir   PTDDD_PTDDD7

#define BOOST           300

static int16 pwmValueLeft;
static int16 pwmValueRight;

static int8 pwmLeft;
static int8 pwmRight;


/**
 * returns the pwm value of the left motor
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
  if (value < 0)              // backward
  {
    EngineL_A_Dir = 0;        // EngineL A = Input = PWM
    EngineL_B_Dir = 1;        // EngineL B = Output
    EngineL_B = DECAY; 
    TPM2C1V = pwmValueLeft = (MODULO - 127) + (-value);
  }
  else if (value > 0)         // forward
  {
    EngineL_A_Dir = 1;        // EngineL A = Output
    EngineL_B_Dir = 0;        // EngineL B = Input = PWM
    EngineL_A = DECAY;
    TPM2C1V = pwmValueLeft = (MODULO - 127) + (value); 
  } 
  else                        // stop
  {
    EngineL_A_Dir = 1;        // EngineR A = Output
    EngineL_B_Dir = 1;        // EngineL B = Output
    EngineL_A = DECAY;
    EngineL_B = DECAY;
    TPM2C1V = 0;
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
  if (value < 0)              // backward
  {
    EngineR_A_Dir = 1;        // EngineR A = Output
    EngineR_B_Dir = 0;        // EngineR B = Input = PWM
    EngineR_A = DECAY;
    TPM2C0V = pwmValueRight = (MODULO -127) + (-value);
  }
  else if (value > 0)         // forward
  {
    EngineR_A_Dir = 0;        // EngineR A = Input = PWM
    EngineR_B_Dir = 1;        // EngineR B = Output
    EngineR_B = DECAY;
    TPM2C0V = pwmValueRight = (MODULO - 127) + value; 
  } 
  else                        // stop
  {
    EngineR_A_Dir = 1;        // EngineR A = Output
    EngineR_B_Dir = 1;        // EngineR B = Output
    EngineR_A = DECAY;
    EngineR_B = DECAY;
    TPM2C0V = 0;
  }
  pwmRight = value;
}


/**
 * Initializes the motor driver as follows:
 * - TPM2: Clocksource = 24 MHZ, Prescale = 4
 * - TPM2CH0: edge aligned pwm
 * - TPM2Ch1: edge aligned pwm 
 */
void motorInit(void)
{   
  EngineL_A_Dir = 1;
  EngineL_B_Dir = 1;
  EngineR_A_Dir = 1;
  EngineR_B_Dir = 1;
  
  EngineR_A = 0;
  EngineR_B = 0;
  EngineL_A = 0;
  EngineL_B = 0;
  
  TPM2MOD = MODULO - 1;   // set PWM frequency.
  
  // PWM motor right
  TPM2C0SC = 0x28;        // edge aligned pwm, high-true pulses    
  TPM2C0SC_ELS0A = DECAY;
     
  // PWM motor left
  TPM2C1SC = 0x28;        // edge aligned pwm, high-true pulses
  TPM2C1SC_ELS1A = DECAY;
  // Start timer
  TPM2SC = 0x0A;          // Busclock, Prescaler=4  
}

#elif MOTOR_DRIVER_VERSION == 3

#define DECAY            1    // 1 slow decay, 0 = fast decay


// joc 34
#define MOTOR_OFFSET    0    //  0 => pwm freq = 47 kHz, 
                              // 60 => pwm freq = 18.9 kHz
                              // 75 => pwm freq = 11.8 kHz

#define MODULO          (-(127*127) / (MOTOR_OFFSET-127))

#define EngineR_A       PTDD_PTDD4
#define EngineR_B       PTDD_PTDD5
#define EngineL_A       PTDD_PTDD6
#define EngineL_B       PTDD_PTDD7

#define EngineR_A_Dir   PTDDD_PTDDD4
#define EngineR_B_Dir   PTDDD_PTDDD5
#define EngineL_A_Dir   PTDDD_PTDDD6
#define EngineL_B_Dir   PTDDD_PTDDD7

#define EngineR_PWM     PTFD_PTFD4
#define EngineL_PWM     PTFD_PTfD5

#define BOOST           10

static int16 pwmValueLeft;
static int16 pwmValueRight;

static int8 pwmLeft;
static int8 pwmRight;

static uint16 boosterLeft;
static uint16 boosterRight;

/**
 * Booster
 */
interrupt void motorBoosterISR(void)
{
  OnEnterMotorBoosterISR(); // switch on led for debug purposes
  
  TPM2SC_TOF = 0;
 
  if (boosterLeft > 0) boosterLeft--;
  if (boosterLeft == 0) TPM2C1V = pwmValueLeft;  

  if (boosterRight > 0) boosterRight--;
  if (boosterRight == 0) TPM2C0V = pwmValueRight;

  if (boosterRight == 0 && boosterLeft == 0) TPM2SC_TOIE = 0;
  
  OnExitMotorBoosterISR(); // switch off led for debug purposes
}


/**
 * returns the pwm value of the left motor
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
  if (value < 0)              // backward
  {
    EngineL_A_Dir = 0;        // EngineL A = Input = PWM
    EngineL_B_Dir = 1;        // EngineL B = Output
    EngineL_B = DECAY; 
    TPM2C1V = pwmValueLeft = (MODULO - 127) + (-value);
  }
  else if (value > 0)         // forward
  {
    EngineL_A_Dir = 1;        // EngineL A = Output
    EngineL_B_Dir = 0;        // EngineL B = Input = PWM
    EngineL_A = DECAY;
    TPM2C1V = pwmValueLeft = (MODULO - 127) + (value); 
  } 
  else                        // stop
  {
    EngineL_A_Dir = 1;        // EngineR A = Output
    EngineL_B_Dir = 1;        // EngineL B = Output
    EngineL_A = DECAY;
    EngineL_B = DECAY;
    TPM2C1V = 0;
  }
  
  if (value != 0 && pwmLeft == 0)
  {
    TPM2C1V = MODULO;
    boosterLeft = BOOST;
    if (!TPM2SC_TOIE)
    {
      TPM2SC_TOF = 0;
      TPM2SC_TOIE = 1;
    }
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
  if (value < 0)              // backward
  {
    EngineR_A_Dir = 1;        // EngineR A = Output
    EngineR_B_Dir = 0;        // EngineR B = Input = PWM
    EngineR_A = DECAY;
    TPM2C0V = pwmValueRight = (MODULO -127) + (-value);
  }
  else if (value > 0)         // forward
  {
    EngineR_A_Dir = 0;        // EngineR A = Input = PWM
    EngineR_B_Dir = 1;        // EngineR B = Output
    EngineR_B = DECAY;
    TPM2C0V = pwmValueRight = (MODULO - 127) + value; 
  } 
  else                        // stop
  {
    EngineR_A_Dir = 1;        // EngineR A = Output
    EngineR_B_Dir = 1;        // EngineR B = Output
    EngineR_A = DECAY;
    EngineR_B = DECAY;
    TPM2C0V = 0;
  }
  
  if (value != 0 && pwmRight == 0)
  {
    TPM2C0V = MODULO;
    boosterRight = BOOST;
    if (!TPM2SC_TOIE)
    {
      TPM2SC_TOF = 0;
      TPM2SC_TOIE = 1;
    }
  }
  
  pwmRight = value;
}


void motorInit(void)
{   
  EngineL_A_Dir = 1;
  EngineL_B_Dir = 1;
  EngineR_A_Dir = 1;
  EngineR_B_Dir = 1;
  
  EngineR_A = 0;
  EngineR_B = 0;
  EngineL_A = 0;
  EngineL_B = 0;
  
  TPM2MOD = MODULO - 1;   // set PWM frequency.
  
  // PWM motor right
  TPM2C0SC = 0x28;        // edge aligned pwm, high-true pulses    
  TPM2C0SC_ELS0A = DECAY;
     
  // PWM motor left
  TPM2C1SC = 0x28;        // edge aligned pwm, high-true pulses
  TPM2C1SC_ELS1A = DECAY;
  
  // Start timer
  TPM2SC = 0x0A;          // Busclock, Prescaler=4  
}

#endif


#if MOTOR_DRIVER_VERSION != 3
interrupt void motorBoosterISR() { 
    for (;;) asm BGND; // hi hacker, did you forget to set up the interrupt vector in project.prm???
}
#endif
