/*
 * util.c
 *
 *  Created on: 31.07.2012
 *      Author: zajost
 */

#include "util.h"




void doSleep1ms(void)
{
  uint16 start = TPM1CNT;
  while ((TPM1CNT - start) < 1000) {}
}

void doSleep10ms(void)
{
  uint16 start = TPM1CNT;
  while ((TPM1CNT - start) < 10000) {}
}

/**
 * sleeps ms
 */
void doSleepMs(uint16 ms)
{
  uint16 start;
  while (ms-- > 0)
  {
    start = TPM1CNT;
    while ((TPM1CNT - start) < 1000) {}
  }
}


void utilNum16uToStr(char *dst, size_t dstSize, word val)
{
  unsigned char *ptr = ((unsigned char *)dst);
  unsigned char i=0, j;
  unsigned char tmp;

  dstSize--; /* for zero byte */
  if (val == 0 && dstSize > 0){
    ptr[i++] = '0';
    dstSize--;
  }
  while (val > 0 && dstSize > 0) {
    ptr[i++] = (unsigned char)((val % 10) + '0');
    dstSize--;
    val /= 10;
  }
  for(j=0; j<(i/2); j++) { /* swap buffer */
    tmp = ptr[j];
    ptr[j] = ptr[(i-j)-1];
    ptr[(i-j)-1] = tmp;
  }
  ptr[i] = '\0';
}


/*!
  \brief Converts a 16bit signed number into a string.
  \param[in,out] dst String buffer to store the number.
  \param[in] dstSize Size of the destination buffer in bytes.
  \param[in] val 16bit signed number to convert.
 */
void utilNum16sToStr(char *dst, size_t dstSize, int val)
{
  unsigned char *ptr =  ((unsigned char *)dst);
  unsigned char i=0, j;
  unsigned char tmp;
  unsigned char sign = (unsigned char)(val < 0);

  dstSize--; /* for zero byte */
  if (sign){
    val *= -1;
  }
  if (val == 0 && dstSize > 0){
    ptr[i++] = '0';
    dstSize--;
  }
  while (val > 0 && dstSize > 0) {
    ptr[i++] = (unsigned char)((val % 10) + '0');
    dstSize--;
    val /= 10;
  }
  if (sign && dstSize > 0){
    ptr[i++] = '-';
    dstSize--;
  }
  for(j=0; j<(i/2); j++) { /* swap buffer */
    tmp = ptr[j];
    ptr[j] = ptr[(i-j)-1];
    ptr[(i-j)-1] = tmp;
  }
  ptr[i] = '\0';
}