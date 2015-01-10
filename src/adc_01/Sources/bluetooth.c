/**
 *--------------------------------------------------------------------\n
 *          HSLU T&A Hochschule Luzern Technik+Architektur            \n
 *--------------------------------------------------------------------\n
 *
 * \brief         bluetooth driver
 * \file
 * \author        Christian Jost, christian.jost@hslu.ch
 * \date          30.11.2012
 *
 * \b Language:   Ansi-C \n\n
 * \b Target:     MC-Car  \n
 *
 * \par Description:
 *
 * $Id: bluetooth.c 533 2013-11-24 19:07:45Z zajost $
 *--------------------------------------------------------------------
 */
#include "platform.h"
#include "bluetooth.h"
#include "string.h"
#include "util.h"
#include "sci.h"


uint8 sciReadWithTimeout(char* str, uint8 length, uint8 timeout)
{
  while ((timeout--)>0)
  {
    if (sci1HasLineReceived())
    {
      return (uint8)sci1ReadLine(str, length);
    }
    doSleep1ms();
  }
  return 0;
}


uint8 btSendCommand2(const char *cmd, char *awr, uint8 awrLength)
{
  char buf[32];
  volatile uint8 result;
  uint8 cmdLength;
  char *p;
  PTFDD_PTFDD7 = 1; // BT_Command
  PTFD_PTFD7 = 1;
  
  sci1DiscardRxBuffer();
  sci1Write(cmd);
  sci1Write("\r\n");
  
  cmd += 2; // Skip AT
  cmdLength = (uint8)strlen(cmd);
    
  result = sciReadWithTimeout(awr, awrLength, 250);
  
  if (cmd[cmdLength-1] == '?')
  {
    // Response
    // +NAME=  =>  +NAME:
    if (strncmp(cmd, awr, cmdLength-1) == 0)
    {
      p = awr + cmdLength;
      while ((*p != '\r') && (awrLength-- > 0)) *awr++ = *p++;
      *awr = 0;
      
      result = sciReadWithTimeout(buf, sizeof(buf), 250);
    }
  }
  else if (cmd[cmdLength-1] == '=')
  {
    // set command
  }
 
  PTFD_PTFD7 = 0;
  return 0;
}


uint8 btSendCommand(char *cmd, char *awr, uint8 awrLength)
{ 
  uint8 result;
  PTFDD_PTFDD7 = 1; // BT_Command
  PTFD_PTFD7 = 1;
  
  sci1DiscardRxBuffer();
  
  sci1Write(cmd);
  sci1Write("\r\n");
    
  result = sciReadWithTimeout(awr, awrLength, 250);

  PTFD_PTFD7 = 0;
  return result;
}


uint8 btGetName(char *name, uint8 length)
{
  uint16 result;
  char awr[40];
  
  
  //result = btSendCommand2("AT+NAME?", awr, 7);// sizeof(awr));
  result = btSendCommand("AT", awr, sizeof(awr));

  result = btSendCommand("AT+NAME?", awr, sizeof(awr));
  
  if ((result > 0) && strncmp(awr, "+NAME:", sizeof("+NAME:")-1) == 0)
  {
    (void)strncpy(name, &awr[sizeof("+NAME:")-1], length);
    
    result = sciReadWithTimeout(awr, sizeof(awr), 250);
    if ((result > 0) && strncmp(awr, "OK", sizeof("OK")-1) == 0)
    {
      return 0;
    }
  }
  return 1;
}



uint8 btSetName(char *name)
{
  uint8 result;
  char str[40];
  (void)strcpy(str, "AT+NAME=");
  (void)strcat(str, name);
  
  result = btSendCommand(str, str, sizeof(str));

  return result;
}



void btInit(void)
{
  volatile uint8 result;
  char name[40];
    
  PTBDD_PTBDD3 = 1;
  PTBD_PTBD3 = 0;
  doSleep10ms();
  PTBDD_PTBDD3 = 0;
  
  // 600ms is too short, 700ms works
  doSleepMs(1000);
  
  result = btGetName(name, sizeof(name));
  
  if (result == EC_SUCCESS)
  {
    if (strncmp(BLUETOOTH_NAME, name, sizeof(BLUETOOTH_NAME)-1) != 0)
    {
      result = btSetName(BLUETOOTH_NAME);
    }
  }  
}