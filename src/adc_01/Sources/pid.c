/**
 *--------------------------------------------------------------------\n
 *          HSLU T&A Hochschule Luzern Technik+Architektur            \n
 *--------------------------------------------------------------------\n
 *
 * \brief         PID control
 * \file
 * \author        Christian Jost
 * \date          21.11.2012
 *
 * \b Language:   Ansi-C \n\n
 * \b Target:     MC Car \n
 *
 * $Id: pid.c 533 2013-11-24 19:07:45Z zajost $
 *--------------------------------------------------------------------
 */
#include "platform.h"
#include "motor.h"
#include "pid.h"
#include "quad.h"
#include <string.h>
#include "stdlib.h"
#include "util.h"
#include "term.h"

static tQuadSpeed nomValue, nomValueOld; // Istwert

static int16 setValueL, setValueOldL;
static int16 setValueR, setValueOldR;

static uint8 kpL, kiL, kdL;
static uint8 kpR, kiR, kdR;

static int16 integL;
static int16 integR;

static int16 devOldL;
static int16 devOldR;

static uint8 delay=0;
static bool pidEnabled;

static int16 devL, valL;
static int16 devR, valR;

void pidSetSpeedL(int16 speed)
{
  setValueL = speed;
}

void pidSetSpeedR(int16 speed)
{
  setValueR = speed;
}

void pidSetSpeed(int16 speedL, int16 speedR)
{
  setValueL = speedL;
  setValueR = speedR;
}

void pidDoWork(void)
{  
  nomValueOld = nomValue;
  // readSpeed from Encoder
  if (quadGetSpeed(&nomValue) != EC_SUCCESS) return;
  
  if (delay > 0)
  {
    delay--;
    return;
  }
  
  if (setValueOldL == 0 && setValueL != 0)
  {
    delay = 2;
  }
  setValueOldL = setValueL;
  
  
  nomValue.speedLeft = (nomValueOld.speedLeft + nomValue.speedLeft) >> 1;
  nomValue.speedRight = (nomValueOld.speedRight + nomValue.speedRight) >> 1;
  
  
  // left wheel
  if (setValueL == 0) valL = integL = devOldL = 0;
  else
  {
    // deviation (max devL = +1000 - -1000 = 2000)
    devL = (setValueL - nomValue.speedLeft) / 8;
    
    // P-Part (max kpL = 
    valL = (kpL * devL) / 32;
    
    // I-Part with anti-windup
    if (kiL != 0) integL += devL;    
    valL += (kiL * integL) / 32;
    
    // D-Part
    valL += (kdL*(devL-devOldL)/32);
    devOldL = devL;
    
    // limit control point
    if (valL > 127)
    {
      valL = 127;
      integL -= devL;
    }
    else if (valL < -127)
    {
      valL = -127;
      integL -= devL;
    }
  }

  // right wheel
  if (setValueR == 0) valR = integR = devOldR = 0;
  else
  {
    // control deviation 
    devR = (setValueR - nomValue.speedRight) / 8;
    
    // P-Part
    valR = (kpR * devR) / 32;
    
    // I-Part  with anti-windup
    if (kiR != 0) integR += devR;
    valR += (kiR * integR) / 32;

    // D-Part
    valR += (kdR*(devR-devOldR)/32);
    devOldR = devR;
        
    // limit control point
    if (valR > 127)
    {
      valR = 127;
      integR -= devR;
    }
    else if (valR < -127)
    {
      valR = -127;
      integR -= devR;
    }
  }
  if (pidEnabled)
  {
    motorSetPWMLeft((int8)valL);
    motorSetPWMRight((int8)valR);
  }
}





void pidWriteHelp(void)
{
  termWriteLine("pid commands:");
  termWriteLine("  help");
  termWriteLine("  status");
  termWriteLine("  set p <value: 0..100>");
  termWriteLine("  set i <value: 0..100>");
  termWriteLine("  set d <value: 0..100>");
  termWriteLine("  set o <0|1>");
}

void pidWriteStatus(void)
{ 
  //termWrite("  p=23,i=23,d=1,o=1");
  termWriteLine("pid status:");
  termWrite("  p=");
  termWriteNum16u(kpL);
  termWrite(",i=");
  termWriteNum16u(kiL);
  termWrite(",d=");
  termWriteNum16u(kdL);
  termWrite(",o=");
  termWriteNum16u(pidEnabled);  
  
  //termWrite("  L:s=1231,n=2323,d=2323,i=2342,v=123");
  termWrite("\n  L:s=");
  termWriteNum16s(setValueL);
  termWrite(",n=");
  termWriteNum16s(nomValue.speedLeft);
  termWrite(",d=");
  termWriteNum16s(devL);
  termWrite(",i=");
  termWriteNum16s(integL);
  termWrite(",v=");
  termWriteNum16s(valL);
  
  //termWrite("  R:s=1231,n=2323,d=2323,i=2342,v=123");
  termWrite("\n  R:s=");
  termWriteNum16s(setValueR);
  termWrite(",n=");
  termWriteNum16s(nomValue.speedRight);
  termWrite(",d=");
  termWriteNum16s(devR);
  termWrite(",i=");
  termWriteNum16s(integR);
  termWrite(",v=");
  termWriteNum16s(valR);
  termWrite("\n");
}

tError pidParseCommand(char *cmd)
{
  uint8 value;
  if (strcmp(cmd, "help") == 0)
  {
    pidWriteHelp();
    return EC_SUCCESS;
  }
  else if (strcmp(cmd, "status") == 0)
  {
    pidWriteStatus();
    return EC_SUCCESS;
  }
  else if (strncmp(cmd, "set ", sizeof("set")) == 0)
  {
    cmd += sizeof("set ") - 1;
    if (*cmd == 'p')
    { 
      value = (int8)atoi(cmd + 2);
      if (value <= 100)
      {
        kpL = kpR = value;
        return EC_SUCCESS;
      }      
    }
    else if (*cmd == 'i')
    { 
      value = (int8)atoi(cmd + 2);
      if (value <= 100)
      {
        kiL = kiR = value;
        return EC_SUCCESS;
      }      
    }
    else if (*cmd == 'd')
    { 
      value = (int8)atoi(cmd + 2);
      if (value <= 100)
      {
        kdL = kdR = value;
        return EC_SUCCESS;
      }      
    }
    else if (*cmd == 'o')
    { 
      pidEnabled = (int8)atoi(cmd + 2);
      return EC_SUCCESS;
    } 
  }
  return EC_INVALID_CMD;
}


void pidInit(void)
{
  setValueL = setValueR = 0;
  setValueOldL = setValueOldR = 0;
  
  integL = integR = 0;
  devOldL = devOldR = 0;
  
  pidEnabled = TRUE;
  
  kiL = kiR = 10; //10 max 20 
  kpL = kpR = 60; //60 max 128
  kdL = kdR = 0; //10  max 40
}
