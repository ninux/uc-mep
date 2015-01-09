/**
 *--------------------------------------------------------------------\n
 *          HSLU T&A Hochschule Luzern Technik+Architektur            \n
 *--------------------------------------------------------------------\n
 *
 * \brief         IO-Port application for the MC Car
 * \file
 * \author        Christian Jost, christian.jost@hslu.ch
 * \date          04.10.2012
 *
 * \b Language:   Ansi-C \n\n
 * \b Target:     MC-Car  \n
 *
 * \par Description:
 *
 * $Id: main.c 528 2013-11-18 06:53:30Z zajost $
 *--------------------------------------------------------------------
 */
#include "platform.h" /* include peripheral declarations */
#include "ifrRx.h"
#include "sound.h"
#include "sound.h"


const char sound1[] = 
    "lastxmas:d=4,o=5,b=112:16d6,e6,8p,e6,8d6,8p,8a,8e6,8e6,8f#6,d.6,8b,8b," \
    "8e6,8e6,f#6,d.6,8b,8c#6,8d6,8c#6,2b.,16e6,f#6,8p,e.6,8p,8b,8f#6,8g6,8f#6," \
    "2e6,8d6,8c#6,8d6,8c#6,c#6,8d6,8p,8c#6,8p,2a,16d6,e6,8p,e6,8d6,8p,8a," \
    "8e6,8e6,8f#6,d.6,8b,8b,8e6,8e6,f#6,d.6,8b,8c#6,8d6,8c#6,2b.,16e6,f#6,8p," \
    "e.6,8p,8b,8f#6,8g6,8f#6,2e6,8d6,8c#6,8d6,8c#6,c#6,8d6,8p,8c#6,8p,a";
    
const char sound2[] = 
    "Swiss:d=4,o=5,b=85:8f,16p,16f,f,a#,8a#,16p,16a,a,p,8f,16p,16f,f,c6,8c6,16p," \
    "16a#,a#,p,d6,8p,8d6,8c6,8c6,c6,8p,8a#,8a,2g,e,2f";

unsigned char sound3[] = 
    "Obelix:d=16,o=7,b=80:f6,c,c,c,8a6,8f6,g6,d,d,d,8a#6,8g6,a6,e,e,e,8c,8a6,f,f,e," \
    "d,4c,8p,f6,c,c,c,8a6,8f6,g6,d,d,d,8a#6,8g6,a6,e,e,e,8c,8a6,f,f,e,d,4c";


//const char sound4[] = "Chariots of Fire:d=16,o=5,b=85:8c#,f#.,g#.,a#.,4g#,4f,8p,8c#," \
//    "f#.,g#.,a#.,2g#,8p,8c#,f#.,g#.,a#.,4g#,4f,8p,8f,f#.,f.,c#.,2c#";

const char sound4[] = "Toccata:d=16,o=6,b=63:32a7,32g7,4a7,4p,32g7,32f7,32e7," \
    "32d7,8c#7,4d7,4p.,32a,32g,4a,4p,16e,16f,16c#,4d,4p.,32a5,32g5,4a5,4p,32g5,32f5,32e5," \
    "32d5,8c#5,4d5,4p.,4d5,8c#5,8e5,8g5,8a#5,8c#,4e,8g,8e,2f#,4p,16c#,32d,32e,32c#,32d," \
    "32e,32c#,32d,32e,32c#,16d,16e,32f,32g,32e,32f,32g,32e,32f,32g,32e,16f,16g,32a,32a#," \
    "32g,32a,32a#,32g,32a,32a#,32g,8a,4p,16c#7,32d7,32e7,32c#7,32d7,32e7,32c#7,32d7,32e7," \
    "32c#7,16d7,16e7,32f7,32g7,32e7,32f7,32g7,32e7,32f7,32g7,32e7,16f7,16g7,32a7,32a#7,32g7," \
    "32a7,32a#7,32g7,32a7,32a#7,32g7,8a7,4p,16a7.,32g7,32a#7,32e7,32g7,32a#7,32e7,32f7,32a7," \
    "32d7,32f7,32a7,32d7,32e7,32g7,32c7,32e7,32g7,32c7,32d7,32f7,32a#,32d7,32f7,32a#,32c7," \
    "32e7,32a,32c7,32e7,32a,32a#,32d7,32g,32a#,32d7,32g,32a,32c7,32f,32a,32c7,32f,32g,32a#," \
    "32e,32g,32a#,32e,32f,32a,32d,32f,32a,32d,32e,32g,32c#,32e,32g,32c#,2d,4p,4a#,32p.,32a.," \
    "32g.,32f.,32e.,32d.,32c#.,32b5.,32c#.,32a5.,32c#.,32e.,32g.,16g,32f,32g,32f,32g,32f,32g," \
    "32f,32g,32f,32g,32f,32g,32f,32g,32f,16e.,2d,2p,8a,32d7,32a,32e7,32a,32f7,32a,32d7,32a," \
    "32e7,32a,32f7,32a,32g7,32a,32e7,32a,32f7,32a,32g7,32a,32a7,32a,32f7,32a,32g7,32a,32a7,32a," \
    "32a#7,32a,32g7,32a,32a7,32a,32f7,32a,32g7,32a,32e7,32a,32f7,32a,32d7,32a,32e7,32a,32c#7," \
    "32a,32d7,32a,32a,32a,32a#,32a,32g,32a,32a,32a,32f,32a,32g,32a,32e,32a,32f,32a,32d,32a,32g," \
    "32a,32e,32a,32f,32a,32d,32a,32e,32a,32c#,32a,32d,32a,32a5,32a,32a#5,32a,32g5,32a,32a5,32a," \
    "32f5,32a,32g5,32a,32e5,32a,32f5,32a,32d5,32a,32g5,32a,32e5,32a,32f5,32a,32d5,32a,32e5,32a," \
    "32c#5,32a,4d5,4p,32d,32f,32a#,32f,32c,32e,32a,32e,32a#5,32d,32g,32d,32a5,32c#,32e,32a,16d," \
    "16a#,16c,16a,16a#5,16g,8a,32p,32d,32f,32a#,32f,32c,32e,32a,32e,32a#5,32d,32g,32d,32a5,32c#," \
    "32e,32a,16d,16a#,16c,16a,16a#5,16g,4a,16p,32p,32g,32f,32e,32d,32c#,32b5,32c#,32a5,32b5,32c#," \
    "32d,32e,32f,32g,32a,32g,32f,32e,32f,32d,32f,32a,32c#7,32d7,32a,32b,32c#7,32d7,32e7,32f7,32a7," \
    "8a#7,16d,16a#7,16c,16a7,16a#5,16g7,8a7,32p,32d7,32f7,32a#7,32f7,32c7,32e7,32a7,32e7,32a#,32d7," \
    "32g7,32d7,32a,32c#7,32e7,32a7,16d,16a#7,16c,16a7,16a#5,16g7,4a7,8p,4b,8c#7.,16b,16a,16c#7,32e7," \
    "32g7,16a#7,32a7,32g7,32f7,32e7,32f7,32e7,32d7,32c#7,32d7,32c7,32a#,32a,32g,32f,32e,32d,4c#," \
    "32c#7.,32e7,32c#7,32a#,32c#7,32a#,32c#7,32e7,32c#7,32a#,32c#7,32a#,32c#7,32e7,32c#7,32a#,32c#7," \
    "32a#,32c#7,32e7,32c#7,32a#,32c#7,32a#,32g.,32a#,32g,32e,32g,32e,32g,32a#,32g,32e,32g,32e,32g," \
    "32a#,32g,32e,32g,32e,32g,32a#,32g,32e,32g,32e,32c#.,32e,32c#,32a#5,32c#,32a#5,32c#,32e,32c#," \
    "32a#5,32c#,32a#5,32c#,32e,32c#,32a#5,32c#,32a#5,32c#,32e,32c#,32a#5,32c#,32a#5,32c#.,32e,32c#," \
    "32e,32g,32e,32c#,32e,32c#,32e,32g,32e,32c#,32e,32c#,32e,32g,32e,32c#,32e,32c#,32e,32g,32e,32g.," \
    "32a#,32g,32a#,32g,32a#,32g,32a#,32g,32a#,32g,32a#,32c#7.,32a#,32c#7,32e7,32c#7,32e7,32c#7,32e7," \
    "32c#7,32e7,32c#7,32e7,4a7,4a7,4a#7.,16a7,16g7,4a7.,16e,16f,16d,16e,16c#,16d,16b5,16c#,16a5," \
    "16a#5,16g#5,8a5,8g,4f,4d,2a5,1d5";

const char sound5[] = "Happy:d=4,o=5,b=125:16c, 32p, 32c, 32p, 8d, 32p, 8c, 32p, 8f, 32p, e, 16p, 16c, 32p, 32c, 32p, 8d, 32p, 8c, 32p, 8g, 32p, f, 8p, 16c, 32p, 32c, 32p, 8c6, 32p, 8a, 32p, 8f, 32p, 8e, 32p, 8d, 32p, 16a#, 32p, 32a#, 32p, 8a, 32p, 8f, 32p, 8g, 32p, f";
    
    
/**
 * Configures the port F and G as follows:
 * - Port F as output
 * - Port G as input with pull-up enabled
 */
void initPorts(void)
{
    PTFDD = 0x07;           // Port F = Output
    PTFD = 0x07;
    
    PTDDD = 0x04;
    
    PTGDD = 0x00;           // Port G = Input
    PTGPE = 0xff;           // Port G = Pull-Up enable

    PTEDD_PTEDD2 = 0;		// pin 2 port E = IN
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


/**
 * main program
 * 
 * This program reads D0-D2 of the port G and write it to D0-D2 of port F.
 */  
void main(void) 
{ 
  const char * soundFiles[] = {sound1, sound2, sound3, sound4, sound5};
  uint8 count = sizeof(soundFiles) / sizeof(soundFiles[0]); 
  
  uint8 playIndex = 0;
  
  PTDDD_PTDDD2 = 1;
    
  initPorts();            // Ports konfigurieren
    
  ifrRxFrontInit();       // init IR receiver
  
  initTimer();            // Timer init
  
  EnableInterrupts;       // Interrupts aktivieren
    
  for(;;) 
  {      
    switch (ifrRxFrontGetKey())
    //switch (ifrRxRearGetKey())
    {     
      case 'S' : // play/pause
        soundTooglePlayPause();
        break;
        
      case 'W' : // play next melody
        playIndex++;
        if (playIndex >= count) playIndex = 0;
        soundPlay(soundFiles[playIndex]);
        break;
      
      case 'T' : // play previous melody
        if (playIndex == 0) playIndex = count;
        playIndex--;
        soundPlay(soundFiles[playIndex]);
        break; 
      
      case '+' :  break;
      case '-' : soundStop();  break;
    }
  }
}



