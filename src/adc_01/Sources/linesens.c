/**
 *--------------------------------------------------------------------\n
 *          HSLU T&A Hochschule Luzern Technik+Architektur            \n
 *--------------------------------------------------------------------\n
 *
 * \brief         Line sensor driver
 * \file
 * \author        Christian Jost, christian.jost@hslu.ch
 * \date          26.11.2012
 *
 * \b Language:   Ansi-C \n\n
 * \b Target:     MC-Car  \n
 *
 * \par Description:
 *
 * $Id: linesens.c 547 2013-12-01 18:41:02Z zajost $
 *--------------------------------------------------------------------
 */
#include "platform.h"
#include "adc.h"
#include "linesens.h"
#include <string.h>
#include "term.h"
#include "sci.h"

#define LS_R    PTAD_PTAD0
#define LS_MR   PTAD_PTAD1
#define LS_M    PTAD_PTAD2
#define LS_ML   PTAD_PTAD3
#define LS_L    PTAD_PTAD4
  
static int16 spots[8];
static uint8 spotsBlack[8];
static uint8 spotsWhite[8];
static uint8 spotsThresholds[8];

#define threshold 65 // 65% Helligkeit => weiss, ansonsten schwarz


/**
 * sleeps 150us
 */
void doSleep150us(void)
{
  uint16 start = TPM1CNT;
  while ((TPM1CNT - start) < 200) {}
}


void lsDoScan(void)
{
  // measure ambient light for each spot 
  spots[0] = spots[1] = adcGet8BitValue(adcLsL);
  spots[2] = spots[3] = adcGet8BitValue(adcLsML);
  spots[4] = spots[5] = adcGet8BitValue(adcLsMR);
  spots[6] = spots[7] = adcGet8BitValue(adcLsR);
  
  LS_L = 0;
  doSleep150us();
  spots[0] = adcGet8BitValue(adcLsL) - spots[0];
  LS_L = 1;
  
  LS_ML = 0;
  doSleep150us();
  spots[1] = adcGet8BitValue(adcLsL) - spots[1];
  spots[2] = adcGet8BitValue(adcLsML) - spots[2];
  LS_ML = 1;
  
  LS_M = 0;
  doSleep150us();
  spots[3] = adcGet8BitValue(adcLsML) - spots[3];
  spots[4] = adcGet8BitValue(adcLsMR) - spots[4];
  LS_M = 1;
  
  LS_MR = 0;
  doSleep150us();
  spots[5] = adcGet8BitValue(adcLsMR) - spots[5];
  spots[6] = adcGet8BitValue(adcLsR) - spots[6];
  LS_MR = 1;  
  
  LS_R = 0;
  doSleep150us();
  spots[7] = adcGet8BitValue(adcLsR) - spots[7];
  LS_R = 1;   
}


uint8 lsGetSpots(void)
{
  int8 value;
  int8 i;
    
  lsDoScan();
   
  value = 0;
  for (i=0; i<8; i++)
  {
    spots[i] -= spotsBlack[i];
    if (spots[i] < 0) spots[i] = 0;
    
    if (spots[i] < spotsThresholds[i])
    {
      value |= (1<<i);
    }
  }
  return value;
}


int8 lsGetDir(void)
{
  int8 i;
  int8 v = 0;
    
  lsDoScan();
  
  for (i=0; i<8; i++)
  {
    spots[i] -= spotsBlack[i];
    if (spots[i] < 0) spots[i] = 0;
  }
  
  // count white spots beginning from the left
  for (i=0; i<=7; i++)
  {
    if (spots[i] > spotsThresholds[i]) v--;
    else break;
  }  
  
  // only white spots found
  if (v == -8)
  {
    return LS_NO_SIGNAL;
  }
  
  // count white spots beginning from the right
  for (i=7; i>=0; i--)
  {
    if (spots[i] > spotsThresholds[i]) v++;
    else break;
  }
  return v;
}


/**
 * White calibration. 
 * Place the MC-Car on a white area before this function is called. 
 */
void lsSetWhite(void)
{
    uint8 i;
    lsDoScan();
    for (i=0; i<8; i++)
    {
        spotsWhite[i] = (uint8)spots[i];
    }
    
    for (i=0; i<8; i++)
    {
        spotsThresholds[i] = (uint8)(((int16)threshold * (spotsWhite[i] - spotsBlack[i])) / 100);
    } 
}


/**
 * Black calibration. 
 * Place the MC-Car on a black area before this function is called. 
 */
void lsSetBlack(void)
{
    uint8 i;
    lsDoScan();
    for (i=0; i<8; i++)
    {
        spotsBlack[i] = (uint8)spots[i];
    }
    
    for (i=0; i<8; i++)
    {
        spotsThresholds[i] = (uint8)(((int16)threshold * (spotsWhite[i] - spotsBlack[i])) / 100);
    } 
}


tError lsWriteStatus(void)
{
  uint8 i;
  uint8 value; 
  
  termWriteLine("line sensor status:");
  value = lsGetSpots();
  
  termWrite("  ");
  for (i=0; i<8; i++)
  {
    if (value & (1<<i)) termWriteChar('X');
    else termWriteChar('-');
  }
  termWriteChar('\n');
  
  return EC_SUCCESS;
}


tError lsParseCommand(char *cmd)
{
    if (strcmp(cmd, "help") == 0)
    {
        termWriteLine("ls");
        termWriteLine("  help");
        termWriteLine("  status");        
        return EC_SUCCESS;
    }
    else if (strcmp(cmd, "status") == 0)
    {
        return lsWriteStatus();
    }
    return EC_INVALID_CMD;
}


void lsInit(void)
{
  PTADD = PTADD | 0x1F; // set direction for IR-Leds to output
    
  LS_R = 1;
  LS_MR = 1;
  LS_M = 1;
  LS_ML = 1;
  LS_L = 1;

  // default value if no calibration is performed
  spotsBlack[0] = 31;
  spotsBlack[1] = 30;
  spotsBlack[2] = 31;
  spotsBlack[3] = 40;
  spotsBlack[4] = 32;
  spotsBlack[5] = 26;
  spotsBlack[6] = 22;
  spotsBlack[7] = 21;

  // replace numbers below: 168 etc.
  spotsThresholds[0] = (uint8)(((int16)threshold * (168 - spotsBlack[0])) / 100);
  spotsThresholds[1] = (uint8)(((int16)threshold * (173 - spotsBlack[1])) / 100);
  spotsThresholds[2] = (uint8)(((int16)threshold * (177 - spotsBlack[2])) / 100);
  spotsThresholds[3] = (uint8)(((int16)threshold * (210 - spotsBlack[3])) / 100);
  spotsThresholds[4] = (uint8)(((int16)threshold * (164 - spotsBlack[4])) / 100);
  spotsThresholds[5] = (uint8)(((int16)threshold * (157 - spotsBlack[5])) / 100);
  spotsThresholds[6] = (uint8)(((int16)threshold * (134 - spotsBlack[6])) / 100);
  spotsThresholds[7] = (uint8)(((int16)threshold * (127 - spotsBlack[7])) / 100);
}
