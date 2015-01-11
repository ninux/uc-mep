/*
 * util.h
 *
 *  Created on: 31.07.2012
 *      Author: zajost
 */

#ifndef UTIL_H
#define UTIL_H

#include <stdlib.h>
#include "platform.h"


void doSleep1ms(void);
void doSleep10ms(void);
void doSleepMs(uint16 ms);

void utilNum16sToStr(char *dst, size_t dstSize, int val);
void utilNum16uToStr(char *dst, size_t dstSize, word val);

#endif /* UTIL_H */
