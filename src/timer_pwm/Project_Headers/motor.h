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
 * $Id: motor.h 520 2013-11-11 09:55:21Z zajost $
 *--------------------------------------------------------------------
 */
#ifndef MOTOR_H
#define MOTOR_H



int8 motorGetPWMLeft(void);
void motorSetPWMLeft(int8 value);
void motorIncrementPWMLeft(int8 value);

int8 motorGetPWMRight(void);
void motorSetPWMRight(int8 value);
void motorIncrementPWMRight(int8 value);

void motorInit(void);

#endif /* MOTOR_H_ */
