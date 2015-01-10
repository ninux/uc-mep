/*
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
 * \b Target:     Medusa / Demoboard \n 
 *
 *
 *  $Id: sci2.c 533 2013-11-24 19:07:45Z zajost $
 *
 *
 * \par Description:
 *  Diese Datei beinhaltet einen Treiber für die serielle Schnittstelle
 *  des HCS08. Dabei kann gewählt werden, ob SCI1 oder SCI2 verwendet 
 *  werden möchte. ACHTUNG: Im *.PRM-File müssen die Interrupt-Vektoren
 *  entsprechend gesetzt werden.
 *  Der Treiber arbeitet mit zwei Ringpuffern mit einer Grösse von
 *  1-255 Bytes. Gröser dürfen die Queues nicht sein, da sonst der
 *  Zugriff auf die Count-Variable synchronisiert werden muss da
 *  dann statt einer 8bit Variable eine 16bit Variable verwendet werden
 *  muss.
 *  
 *  Die Konfiguration lautet wie folgt: 
 *  1 Startbit, 8 Datenbits, Kein Paritätsbit, 1 Stopbit
 * 
 * \par History:
 *  07.12.09 Joc  Erstellung \n
 *--------------------------------------------------------------------
 */
#include "sci.h"
#include "platform.h"             // include peripheral declarations

#if (SCI2_EN > 0)

#define NEW_LINE        '\n'


// Sendequeue
static char tx2Buf[SCI2_TX_BUF_SIZE];
static uint8 tx2BufCount;
static uint8 tx2BufWritePos;
static uint8 tx2BufReadPos;

// Empfangsqueue
static char rx2Buf[SCI2_RX_BUF_SIZE];
static uint8 rx2BufCount;
static uint8 rx2BufWritePos;
static uint8 rx2BufReadPos;


#if ((SCI2_RX_BUF_SIZE > 255) || (SCI2_RX_BUF_SIZE < 1)) 
#error ACHTUNG: SCI2_RX_BUF_SIZE muss zwischen 1..255 sein!
#endif

#if (SCI2_TX_BUF_SIZE > 255 || (SCI2_TX_BUF_SIZE < 1)) 
#error ACHTUNG: SCI2_TX_BUF_SIZE muss zwischen 1..255 sein!
#endif       


/**
 * Error-Interrupt-Routine
 *
 * Quittiert aufgetretene Fehler. Ansonsten können keine Daten mehr empfangen werden.
 */
interrupt void sci2Error(void)
{
  (void)SCI2S1;
  (void)SCI2D;
}


/** 
 * RxD Interrupt-Routine
 * 
 * Liest das empfangene Byte und speichert es in der Empfangsqueue.
 *
 * @details
 * Um den Empfangsinterrupt zu quittieren muss zuerst da Statusregister SCIxS1 gelesen
 * werden und anschliessend muss das empfange Zeichen aus SCIxD gelesen werden.
 */
interrupt void sci2RxD(void)
{
  char ch;
  
  OnEnterSci2RxD(); // switch on led for debug purposes
  
  (void)SCI2S1;            // 1. Status lesen
  ch = SCI2D;        // 2. empf. Zeichen lesen => Interrupt wird quittiert.
  
  if (rx2BufCount < SCI2_RX_BUF_SIZE)
  {
    rx2Buf[rx2BufWritePos] = ch;      // empf. Zeichen in der Queue speichern
    rx2BufCount++;
    rx2BufWritePos++;
    if (rx2BufWritePos == SCI2_RX_BUF_SIZE) rx2BufWritePos = 0;
  } 
  
  OnExitSci2RxD(); // switch off led for debug purposes
}


/**
 * TxD Interrupt-Routine 
 *
 * Holt das nächste Byte aus der Queue und versendet es.
 * Falls die Queue leer ist, dann wird der Interrupt deaktiviert.
 */
interrupt void sci2TxD()
{
  OnEnterSci2TxD(); // switch on led for debug purposes
  
  (void)SCI2S1;
  if (tx2BufCount > 0)
  {
    SCI2D = tx2Buf[tx2BufReadPos];
    tx2BufCount--;
    tx2BufReadPos++;
    if (tx2BufReadPos == SCI2_TX_BUF_SIZE) tx2BufReadPos = 0;      
  }
  else
  {
    // Buffer leer => Transmit Data Register Empty Interrupt deaktivieren.
    // wird später durch sciWriteByte() wieder aktiviert...
    SCI2C2_TIE = 0;
  }
  
  OnExitSci2TxD(); // switch off led for debug purposes
}


/**
 * Liest ein Zeichen von der seriellen Schnittstelle.
 *
 * @return
 *      Der Aufruf ist blockierend, d.h. falls der Empfangsbuffer leer ist wird 
 *      gewartet bis ein Zeichen empfangen wird.
 */
char sci2ReadChar(void)
{
  char ch;
  while(rx2BufCount == 0);                 // Warten bis min. 1 Zeichen verfügbar ist
  
  ch = rx2Buf[rx2BufReadPos];
  rx2BufCount--;
  rx2BufReadPos++;
  if (rx2BufReadPos == SCI2_RX_BUF_SIZE) rx2BufReadPos = 0;
  return ch;
}


/**
 * Schreibt ein Byte in die Sendequeue
 *
 * @details
 *      durch das aktivieren des SendeInterrupts (TIE) wird sofort ein Interrupt ausgelöst,
 *      falls Platz im Sendepuffer des SCI-Moduls vorhanden ist.
 *
 * @param [in] ch
 *      das zu sendende Byte
 */
void sci2WriteChar(char ch)
{
  while (tx2BufCount >= SCI2_TX_BUF_SIZE);  // Warten bis Platz in der Sendequeue
  
  tx2Buf[tx2BufWritePos] = ch;
  tx2BufCount++;
  tx2BufWritePos++;
  if (tx2BufWritePos == SCI2_TX_BUF_SIZE) tx2BufWritePos = 0;
  
  SCI2C2_TIE = 1;                         // Sendeinterrupt aktivieren falls nicht aktiviert.
}


/**
 * liest einen String von der seriellen Schnittstelle
 *
 * @details
 * Liest solange Zeichen von der seriellen Schnittstelle bis er mit einem LineFeed abgeschlossen
 * wird oder die maximale Länge erreicht wird. Das LineFeed-Zeichen wird durch das nullterminierungs
 * Zeichen ersetzt.
 *
 * @param [in] str
 *      ein Puffer, in der der String geschrieben werden kann
 * @param [in] length
 *      die Länge des Puffers
 *
 * @return
 *      die Anzahl gelesener Zeichen.
 */
uint16 sci2ReadLine(char *str, uint16 length)
{
  unsigned int i;
  for (i=1; i<length; i++)
  {
    *str = sci2ReadChar();
    if (*str == NEW_LINE)
    {
        *str = '\0';
        break;
    }
    str++;
  }
  return i;
}


/**
 * Gibt einen String auf der seriellen Schnittstelle aus.
 *
 * Der String muss nullterminiert sein!
 *
 * @param [in] str
 *      der nullterminierte String
 */
void sci2Write(const char *str)
{
  if (str == 0) return;
  while(*str != 0) sci2WriteChar(*str++);
}



/**
 * Gibt einen String auf der seriellen Schnittstelle aus.
 *
 * Der String muss nullterminiert sein! Anschliessend wird noch ein Linefeed hinzugefügt.
 *
 * @param [in] str
 *      der nullterminierte String
 */
void sci2WriteLine(const char *str)
{
  sci2Write(str);
  sci2WriteChar(NEW_LINE);
}


/**
 * Prüft, ob eine Zeile empfangen wurde, d.h ein Newline im Buffer steht.
 * 
 * @return
 *      true, falls ein Newline-Zeichen vorhanden ist, ansonsten false (0)
 */
bool sci2HasLineReceived(void)
{   
  uint16 i;   
  uint16 index = rx2BufReadPos;

  for (i=0; i<rx2BufCount; i++)
  {        
    if (rx2Buf[index] == NEW_LINE) return 1;
    if (++index == SCI2_RX_BUF_SIZE) index = 0;
  }
  
  return 0;
}


/**
 * returns the number of bytes in the receive buffer
 * 
 * @return
 *    the number of bytes in the receive buffer.
 */
unsigned char sci2GetRxCount(void)
{
  return rx2BufCount;
}



/**
 * Clears the receive buffer.
 */
void sci2DiscardRxBuffer(void)
{
  uint8 rie = SCI2C2_RIE;   // Empfangsinterrupt sperren
  
  rx2BufCount = 0;          // RX-Buffer initialisieren
  rx2BufWritePos = 0;
  rx2BufReadPos = 0;
  
  SCI2C2_RIE = rie;        // Empfangsinterrupt wiederherstellen
}


#if (SCI_TERMIO_GETCHAR == 1)
/**
 * GetChar-Funktion für scanf
 *
 * @return
 *      Liefert ein Zeichen aus der Empfangsqueue. Der Aufruf ist blockierend,
 *      d.h. falls der Empfangsbuffer leer ist wird gewartet bis ein Zeichen empfangen wird.   
 */
char TERMIO_GetChar(void)
{  
  return sciReadChar();
}
#endif


#if (SCI_TERMIO_PUTCHAR == 1)
/**
 * PutChar-Funktion für printf
 *
 * @param [in] ch
 *      das Zeichen das ausgegeben werden soll. Der Aufruf ist blockierend falls die
 *      Sendequeue voll ist.
 */
void TERMIO_PutChar(char ch)
{
  sciWriteChar(ch);       
}
#endif


/**
 * Initialisiert die serielle Schnittstelle mit der gewünschten Baudrate.
 *
 * @details
 * Für die Baudraten wurde eine Rundungsfunktion implementiert, da bei sehr 
 * hohen Baudraten eine Abweichung von nur einer Einheit bereits Fehler
 * bei der Übertragung verursachen kann. Ein Beispiel mit fBus = 24 MHz
 * \code
 * Baudrate   SCIxBD (normal)  SCIxBD (gerundet)  
 *    9600    156.25 => 156    156.75 => 156
 *   38400     39.06 =>  39     39.56 =>  39
 *   57600     26.04 =>  26     26.54 =>  26
 *  115200     13.02 =>  13     13.52 =>  13
 * \endcode
 *                          
 * @param [in] baudrate
 *      die gewünschte Baudrate, z.B. 4800, 9600, 38400 etc.
 */
void sci2Init(uint32 baudrate)
{
    // Berechnung Baudrate normalerweise: SCIxBD = Busclock / (16 x Baudrate)
    SCI2BD = (uint16)(((BUSCLOCK * (uint32)10) / ((uint32)16 * baudrate) + 5) / 10);
    
    
    SCI2C3 = 0x0F;              // Fehler-Interrupts aktivieren

    tx2BufCount = 0;            // TX-Buffer initialisieren
    tx2BufWritePos = 0;
    tx2BufReadPos = 0;
    
    SCI2C2_TE = 1;              // Sender einschalten
    
    
    rx2BufCount = 0;            // RX-Buffer initialisieren
    rx2BufWritePos = 0;
    rx2BufReadPos = 0;
    
    SCI2C2_RE = 1;              // Empfänger einschalten;
    SCI2C2_RIE = 1;             // Empfangsinterrupt aktivieren 
}
#else

interrupt void sci2TxD() { 
    for (;;) asm BGND; // Error, SCI2 not enabled
}

interrupt void sci2RxD() { 
    for (;;) asm BGND; // Error, SCI2 not enabled
}

interrupt void sci2Error() { 
    for (;;) asm BGND; // Error, SCI2 not enabled
}

#endif
