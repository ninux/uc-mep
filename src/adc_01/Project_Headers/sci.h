/**
 *--------------------------------------------------------------------\n
 *          HSLU T&A Hochschule Luzern Technik+Architektur            \n
 *--------------------------------------------------------------------\n
 * 
 * \brief         SCI-Treiber
 * \file      
 * \author        Christian Jost, christian.jost@hslu.ch
 * \date          07.12.2009
 * 
 * \b Language:   Ansi-C \n\n
 * \b Target:     Medusa & Demoboard \n 
 *
 *
 *  $Id: sci.h 536 2013-11-24 21:03:20Z zajost $
 *
 *--------------------------------------------------------------------
 */
#ifndef SCI_H
#define SCI_H

#include "platform.h"

// 1 = Bluetooth, 2 = Cable
#define SCI_DEFAULT              0      // [0|1|2]  0=>aus, 1=>SCI1, 2=>SCI2
#define SCI_TERMIO_GETCHAR       0      // [0|1|2]  0=>aus, 1=>SCI1, 2=>SCI2 (GetChar für scanf)
#define SCI_TERMIO_PUTCHAR       0      // [0|1|2]  0=>aus, 1=>SCI1, 2=>SCI2 (PutChar für printf)

#define SCI1_EN                  1      // [0|1]    1=>einschalten, 0=>ausschalten                                   
#define SCI1_RX_BUF_SIZE        64      // [1..255] Grösse des Empfangsbuffers
#define SCI1_TX_BUF_SIZE       128      // [1..255] Grösse des Empfangsbuffers

#define SCI2_EN                  1      // [0|1]    1=>einschalten, 0=>ausschalten                                   
#define SCI2_RX_BUF_SIZE       128      // [1..255] Grösse des Empfangsbuffers
#define SCI2_TX_BUF_SIZE       128      // [1..255] Grösse des Empfangsbuffers


#if (SCI1_EN > 0)
char sci1ReadChar(void);
void sci1WriteChar(char ch);
uint16 sci1ReadLine(char *str, uint16 length);
void sci1Write(const char *str);
void sci1WriteLine(const char *str);
bool sci1HasLineReceived(void);
uint8 sci1GetRxCount(void);
void sci1DiscardRxBuffer(void);
void sci1Init(uint32 baudrate);

#if (SCI_DEFAULT == 1)
#define sciReadChar         sci1ReadChar
#define sciWriteChar        sci1WriteChar
#define sciReadLine         sci1ReadLine
#define sciWrite            sci1Write
#define sciWriteLine        sci1WriteLine
#define sciHasLineReceived  sci1HasLineReceived
#define sciGetRxCount       sci1GetRxCount
#define sciDiscardRxBuffer  sci1DiscardRxBuffer
#define sciInit             sci1Init
#endif
#endif


#if (SCI2_EN > 0)
char sci2ReadChar(void);
void sci2WriteChar(char ch);
uint16 sci2ReadLine(char *str, uint16 length);
void sci2Write(const char *str);
void sci2WriteLine(const char *str);
bool sci2HasLineReceived(void);
uint8 sci2GetRxCount(void);
void sci2DiscardRxBuffer(void);
void sci2Init(uint32 baudrate);

#if (SCI_DEFAULT == 2)
#define sciReadChar         sci2ReadChar
#define sciWriteChar        sci2WriteChar
#define sciReadLine         sci2ReadLine
#define sciWrite            sci2Write
#define sciWriteLine        sci2WriteLine
#define sciHasLineReceived  sci2HasLineReceived
#define sciGetRxCount       sci2GetRxCount
#define sciDiscardRxBuffer  sci2DiscardRxBuffer
#define sciInit             sci2Init
#endif
#endif


#endif
