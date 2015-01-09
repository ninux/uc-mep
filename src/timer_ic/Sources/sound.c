/**
 *--------------------------------------------------------------------\n
 *          HSLU T&A Hochschule Luzern Technik+Architektur            \n
 *--------------------------------------------------------------------\n
 *
 * \brief         Sound player
 * \file
 * \author        Leandro Kressebuch, Christian Jost
 * \date          11.10.2012
 *
 * \b Language:   Ansi-C \n\n
 * \b Target:     MC Car \n
 *
 * $Id: sound.c 271 2012-11-15 15:12:21Z zajost $
 *--------------------------------------------------------------------
 */
#include "platform.h"
#include "sound.h"

#define FCNT              1000000   // TPM1 counter frequency [Hz]
#define MAX_MELODY_SIZE       800   // max number of notes

typedef struct
{
  uint8 note;     // note frequency (0 = a Okt.4 ... 47 = g# Okt.7, 48 = pause)
  uint8 time;     // note duration (in units of 1/64 notes) 
} tNote;


const uint16 noteCounterticks[49]=  // # of counter ticks for half the period duration of some note
{
    // c             c#               d                d#               e                f                f#             g              g#                a              a#              b/h              
    FCNT/(2*261.6),  FCNT/(2*277.2),  FCNT/(2*293.7),  FCNT/(2*311.1),  FCNT/(2*329.6),  FCNT/(2*349.2),  FCNT/(2*370),  FCNT/(2*392),  FCNT/(2*415.3),  FCNT/(2*440),  FCNT/(2*466.2),  FCNT/(2*493.9),  // 4. oktave
    FCNT/(2*523.3),  FCNT/(2*554.4),  FCNT/(2*587.3),  FCNT/(2*622.3),  FCNT/(2*659.3),  FCNT/(2*698.5),  FCNT/(2*740),  FCNT/(2*784),  FCNT/(2*830.6),  FCNT/(2*880),  FCNT/(2*932.3),  FCNT/(2*987.8),  // 5. oktave
    FCNT/(2*1046.5), FCNT/(2*1108.7), FCNT/(2*1174.7), FCNT/(2*1244.5), FCNT/(2*1318.5), FCNT/(2*1396.9), FCNT/(2*1480), FCNT/(2*1568), FCNT/(2*1661.2), FCNT/(2*1760), FCNT/(2*1864.7), FCNT/(2*1975.5), // 6. oktave
    FCNT/(2*2093),   FCNT/(2*2217.4), FCNT/(2*2349.3), FCNT/(2*2489),   FCNT/(2*2637),   FCNT/(2*2793.8), FCNT/(2*2960), FCNT/(2*3136), FCNT/(2*3322.4), FCNT/(2*3520), FCNT/(2*3729.4), FCNT/(2*3951),   // 7. oktave
    65535 //pause 
}; 
                    

static uint16 tick64;    // # of counter ticks for a 1/64 note

static char soundTitle[10];
static tNote melody[MAX_MELODY_SIZE];
static uint16 melodyPos;
static uint16 melodySize;
static uint16 delay;



/**
 * ISR: TPM1 Channel 2 - Output Compare
 * Realizes tone duration by counting down the
 * number of 1/64 notes of current tone
 */
interrupt void soundISRduration(void)
{
  TPM1C2SC_CH2F = 0;
  TPM1C2V += tick64;
  
  if (delay > 0)  // current tone not complete yet
  {
    delay--;
  }
  else            // current tone complete, setup to play next tone
  {    
    melodyPos++;
    if (melodyPos < melodySize)
    {
      TPM1C5V = TPM1CNT + noteCounterticks[melody[melodyPos].note]; // next frequency
      delay = melody[melodyPos].time * 4;                           // next duration
      
      // tone: toggle output, pause (=48): do not toggle output
      TPM1C5SC_ELS5A = (melody[melodyPos].note == 48 ? 0 : 1);
    }
    else
    {
      // end of melody : do not toggle output anymore
      TPM1C5SC = 0;
      TPM1C2SC = 0;
    }
  }
}


/**
 * ISR: TPM1 Channel 5 - Output Compare
 * Realizes tone frequency by toggling output pin 
 * every half period duration of current tone
 */
interrupt void soundISRfreq(void)
{
  TPM1C5SC_CH5F = 0;
  TPM1C5V += noteCounterticks[melody[melodyPos].note];
}


/**
 * Starts a melody from the current position
 *
 * @remarks: Initializes TPM1 as follows:
 *   CH2 : Output Compare without port pin use, Interrupt enable
 *   CH5 : Output Compare, toggle port pin, Interrupt enable
 */
void soundStart(void)
{
  if (melodyPos == melodySize) melodyPos = 0;
  
  // configure "freq" timer
  TPM1C5V = TPM1CNT + noteCounterticks[melody[melodyPos].note];
  TPM1C5SC_CH5F = 0;            // clear channel flag
  TPM1C5SC = 0x54;              // toggle output on compare
    
  // configure "delay" timer
  delay = melody[melodyPos].time;
  TPM1C2V = TPM1CNT + tick64;   // Set counter to a 1/64 Note
  TPM1C2SC_CH2F = 0;            // clear channel flag
  TPM1C2SC = 0x50;              // SW compare only. Interrupt enabled 
}


/**
 * Stops the current melody
 */
void soundStop(void)
{
  TPM1C5SC = 0;
  TPM1C2SC = 0;
}


/**
 * Returns true, if the player is playing a meldoy
 * 
 * @returns
 *    true, if playing, false else
 */
bool soundIsPlaying(void)
{
  return (TPM1C5SC != 0);
}


/**
 * Toggles between play and pause. 
 */
void soundTooglePlayPause(void)
{
  if (soundIsPlaying()) soundStop();
  else soundStart();
}

                                                                                                           //pause
/*
 * Parses the ring tone string and stores frequency and duration
 * for every tone in an array of type tNote.
 * 
 *  @param [in] *soundFile   - RTTTL string  
 */
void soundPlay(const char *p)
{
  uint8 i, value, param, note;
  uint8 duration, defDuration;
  uint8 octave, defOctave;
  
  // Read Title
  for (i=0; i<sizeof(soundTitle); i++)
  {
    if (*p == ':') break;
    soundTitle[i] = *p;
    p++;
  } 
  
  while (*p && *p != ':') p++;
  if (!*p) return;
  p++;
  
  
  // parse default values
  while (*p) {    
    while (*p == ' ') p++;    // Skip Spaces
    if (!*p) return;          // abort at the end of the string
    if (*p == ':') break;
    
    param = *p++;
    if (*p != '=') return;
    
    p++;
    value = 0;
    while (*p >= '0' && *p <= '9') value = value * 10 + (*p++ - '0');
    
    switch (param) {
      case 'd': defDuration = 64 / value; break;
      case 'o': defOctave = ((uint8)value - 4) * 12; break;
      case 'b': tick64 = (uint16)(((60 * 1000000l) / (value * 64)) - 1); break; // bpm
    }
    
    while (*p == ' ') p++;
    if (*p == ',') p++;
  }
  p++;
     
  melodySize = 0;
  while (*p)
  {
    duration = defDuration;
    octave = defOctave;
    
    // Skip whitespace
    while (*p == ' ') p++;
    if (!*p) return;
    
    // Parse duration
    if (*p >= '0' && *p <= '9') 
    {
     value = 0;
     while (*p >= '0' && *p <= '9') value = value * 10 + (*p++ - '0');
     duration = (uint8) (64 / value);
    }
  
    // Parse note
    switch (*p){
      case 0: return;
      case 'C': case 'c': note = 0; break;
      case 'D': case 'd': note = 2; break;
      case 'E': case 'e': note = 4; break;
      case 'F': case 'f': note = 5; break;
      case 'G': case 'g': note = 7; break;
      case 'A': case 'a': note = 9; break;
      case 'H': case 'h': note = 11; break;
      case 'B': case 'b': note = 11; break;
      case 'P': case 'p': note = 48; break;
    }
    p++;
    
    if (*p == '#'){
       note++;
       p++;
    }
     
    if (*p == 'b'){
       note--;
       p++;
    }
  
    // Parse special duration
    if (*p == '.'){
      duration += duration / 2;
      p++;
    }
  
    // Parse octave 4..7
    if (*p >= '0' && *p <= '9'){
      octave = ((*p++ - '0') - 4) * 12;  
    }
     
    // Parse special duration (again...)
    if (*p == '.') {
      duration += duration / 2;
      p++;
    }
  
    // Skip delimiter
    while (*p == ' ') p++;
    if (*p == ',') p++;
    
    note += octave;
    if (note > 48) note = 48;
    
    melody[melodySize].note = note;
    melody[melodySize].time = duration;
    melodySize++;
    if (melodySize >= MAX_MELODY_SIZE) break;
  }  
  
  melodyPos = 0;  
  soundStart();
}
