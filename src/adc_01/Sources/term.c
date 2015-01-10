/**
 *--------------------------------------------------------------------\n
 *          HSLU T&A Hochschule Luzern Technik+Architektur            \n
 *--------------------------------------------------------------------\n
 *
 * \brief         terminal with command line parser
 * \file
 * \author        Christian Jost, christian.jost@hslu.ch
 * \date          30.11.2012
 *
 * \b Language:   Ansi-C \n\n
 * \b Target:     MC-Car  \n
 *
 * \par Description:
 *
 * $Id: term.c 547 2013-12-01 18:41:02Z zajost $
 *--------------------------------------------------------------------
 */
#include "platform.h"
#include "term.h"
#include <string.h>
#include "sci.h"
#include "util.h"
#include "pid.h"
#include "linesens.h"



void termWriteNum16s(int16 value)
{
  char str[8];
  utilNum16sToStr(str, sizeof(str), value);
  sci1Write(str);
  sci2Write(str);
}


void termWriteNum16u(uint16 value)
{
  char str[8];
  utilNum16uToStr(str, sizeof(str), value);
  sci1Write(str);
  sci2Write(str);
}


void termWriteSubString(char *str, uint8 start, uint8 length)
{
  str += start;
  while(length-- > 0) 
  {
    sci1WriteChar(*(str++));
    sci2WriteChar(*(str++));
  }
}


void termWriteLine(char *str)
{
  sci1WriteLine(str);
  sci2WriteLine(str);
}


void termWrite(char *str)
{
  sci1Write(str);
  sci2Write(str);
}

void termWriteChar(char ch)
{
    sci1WriteChar(ch);
    sci2WriteChar(ch);
}


void termParseCommand(char *cmd)
{
  uint8 subCmdLength = 0;
  tError result = EC_INVALID_CMD;
  
  if (strcmp(cmd, "help") == 0)
  {
    termWriteLine(NULL);
    termWriteLine(">>> MC-Car terminal <<<");
    termWriteLine("valid commands are:");
    termWriteLine("  pid help");
    termWriteLine("  ls help");
    result = EC_SUCCESS;
  }
  else if (strncmp(cmd, "pid ", sizeof("pid")) == 0)
  {
    subCmdLength = sizeof("pid ") - 1;
    result = pidParseCommand(cmd + sizeof("pid"));
  }  
  else if (strncmp(cmd, "ls ", sizeof("ls")) == 0)
  {
    subCmdLength = sizeof("ls ") - 1;
    result = lsParseCommand(cmd + sizeof("ls"));
  }     
  if (result != EC_SUCCESS)
  {
    termWrite(cmd);
    termWriteLine(": command not found!");
    termWrite("Enter '");
    termWriteSubString(cmd, 0, subCmdLength);
    termWriteLine("help' for a list of valid commands.");
  }
}


void termDoWork(void)
{
    char cmd[64];
    if  (sci1HasLineReceived())
    {
        (void)sci1ReadLine(cmd, sizeof(cmd));
        termParseCommand(cmd);
    }
    
    if  (sci2HasLineReceived())
    {
        (void)sci2ReadLine(cmd, sizeof(cmd));
        termParseCommand(cmd);
    }
}


void termInit(void)
{
  termWriteLine("MC-Car ready :-)");
}
