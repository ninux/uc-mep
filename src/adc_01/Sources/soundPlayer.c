/**
 *--------------------------------------------------------------------\n
 *          HSLU T&A Hochschule Luzern Technik+Architektur            \n
 *--------------------------------------------------------------------\n
 *
 * \brief         Sound player
 * \file
 * \author        Leandro Kressebuch, Christian Jost
 * \date          03.11.2014
 *
 * \b Language:   Ansi-C \n\n
 * \b Target:     MC Car \n
 *
 * $Id: player.c 517 2013-11-11 08:02:02Z zajost $
 *--------------------------------------------------------------------
 */
#include "platform.h"
#include "sound.h"
#include "soundPlayer.h"

#define FCNT              1000000   // TPM1 counter frequency [Hz]
#define MAX_MELODY_SIZE       800   // max number of notes

typedef struct
{
  uint8 note;     // note frequency (0 = a Okt.4 ... 47 = g# Okt.7, 48 = pause)
  uint8 time;     // note duration (in units of 1/64 notes) 
} tNote;


const uint16 noteFrequency[49]=  // # frequency [Hz] of some notes
{
    // c       c#         d         d#        e         f        f#       g      g#         a      a#        b/h              
    (261.6),  (277.2),  (293.7),  (311.1),  (329.6),  (349.2),  (370),  (392),  (415.3),  (440),  (466.2),  (493.9),  // 4. oktave
    (523.3),  (554.4),  (587.3),  (622.3),  (659.3),  (698.5),  (740),  (784),  (830.6),  (880),  (932.3),  (987.8),  // 5. oktave
    (1046.5), (1108.7), (1174.7), (1244.5), (1318.5), (1396.9), (1480), (1568), (1661.2), (1760), (1864.7), (1975.5), // 6. oktave
    (2093),   (2217.4), (2349.3), (2489),   (2637),   (2793.8), (2960), (3136), (3322.4), (3520), (3729.4), (3951),   // 7. oktave
    0 //pause 
};
                    

static char soundTitle[10];             // name of the melody (string max 10 characters)
static tNote melody[MAX_MELODY_SIZE];   // array for the melody
static uint16 melodyPos;                // current tone position in the melody
static uint16 melodySize;               // number of tones in the melody
static bool isPlaying;
static uint8 msPer64Note;


void soundFinishedCallback()
{
    static bool playNote = FALSE;
    if (isPlaying)
    {
        if (playNote)
        {
            melodyPos++;
            if (melodyPos < melodySize)
            {
                soundBeep(noteFrequency[melody[melodyPos].note], melody[melodyPos].time * msPer64Note);
            }   
            else
            {
                isPlaying = FALSE;
            }
        }
        else
        {
            soundBeep(0, 20);
        }
        playNote = !playNote;
    }
}


void soundStart(void)
{
    if (melodyPos == melodySize) melodyPos = 0;
    isPlaying = TRUE;
    soundSetCallbackHandler(soundFinishedCallback);
    soundBeep(noteFrequency[melody[melodyPos].note], melody[melodyPos].time * msPer64Note);
}


/**
 * Stops the current melody
 */
void soundStop(void)
{
    isPlaying = FALSE;
    melodyPos = 0;
}


/**
 * Returns true, if the player is playing a meldoy
 * 
 * @returns
 *    true, if playing, false else
 */
bool soundIsPlaying(void)
{
    return isPlaying;
}


/**
 * Toggles between play and pause. 
 */
void soundTooglePlayPause(void)
{
    if (soundIsPlaying()) soundStop();
    else soundStart();
}

                                                                                  
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
      //case 'b': tick64 = (uint16)(((60 * 1000000l) / (value * 64)) - 1); break; // bpm
      
      //case 'b': msPer64Note = (uint8)((60*1000l) / (value * 16)); // bpm
      case 'b': msPer64Note = (uint8)(3750 / value); // bpm
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
