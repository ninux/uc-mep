/**
 *--------------------------------------------------------------------\n
 *          HSLU T&A Hochschule Luzern Technik+Architektur            \n
 *--------------------------------------------------------------------\n
 * 
 * \brief         I2C-Treiber
 * \file      
 * \author        Christian Jost, christian.jost@hslu.ch
 * \date          23.09.2009
 * 
 * \b Language:   Ansi-C \n\n
 * \b Target:     MC Car \n
 *
 *
 * $Id: i2c.c 525 2013-11-14 15:58:53Z zajost $
 *
 *
 * \par Description:
 *  Der I2C-Treiber stellt Funktionen zur Verfügung um mit I2C-Slaves zu
 *  kommunizieren.
 *
 * \par History:
 *  23.09.09 Joc  Erstellung \n
 *--------------------------------------------------------------------
 */
#include "platform.h"
#include "i2c.h"


//---------------------------------------------------------------------------------------------------------------------
// I 2 C   -   G R U N D F U N K T I O N E N
//---------------------------------------------------------------------------------------------------------------------

/**
 * Generiert eine Startbedingung auf dem I2C-Bus
 *
 * @param [in] adr
 *    die Slave-Adresse
 *
 * @param [in] write
 *    TRUE => Write
 *    FALSE => Read
 *
 * @return 
 *    EC_SUCCESS        falls der Slave antwortet
 *    EC_I2C_NO_ANSWER  falls der Slave nicht antwortet
 */
tError i2cStart(uint8 adr, bool write)
{    
  while (IICS_BUSY);                        // Warte bis Bus frei ist. Notwendig falls 2x Sende-Befehle kurz nacheinander folgen
  
  IICS_IICIF = 1;                           // Interrupt Bit quittieren falls gesetzt
  
  IICC_TXAK = 0;                            // TXAK (ACK senden) deaktivieren falls aktiviert
  IICC |= 0x30;                             // MST=1, TX=1; => StartCondition senden...
        
  if (write) IICD = (adr & 0xFE);           // Adresse senden - Low aktives Write-Bit
  else IICD = adr | 0x01;
  
  while (!IICS_IICIF);                      // Warten bis Übertragung abgeschlossen ist
  IICS_IICIF = 1;                           // Interrupt-Flag löschen
  if (IICS_RXAK)                            // Prüfen ob ACK empfangen wurde
  { 
        IICC_MST = 0;                       // Stop-Condition generieren                       
        IICS_IICIF = 1;                     // Interrupt-Flag löschen
        return EC_I2C_NO_ANSWER;            // NACK => Abbruch
  }
  return EC_SUCCESS;
}


/**
 * Generiert eine Repeated Start Bedingung auf dem Bus
 *
 * @param [in] adr
 *      die Slave-Adresse
 *
 * @param [in] write
 *      TRUE => falls danach Daten zum Slave gesendet werden
 *      FALSE => falls danach Daten vom Slave gelesen werden
 *
 * @return 
 *    EC_SUCCESS        falls der Slave antwortet
 *    EC_I2C_NO_ANSWER  falls der Slave nicht antwortet
 */
tError i2cRepeatedStart(uint8 adr, bool write)
{
  IICC_RSTA = 1;                            // Repeated Start-Condition ausgeben
  
  if (write) IICD = (adr & 0xFE);           // Adresse senden - Low aktives Write-Bit
  else IICD = adr | 0x01;
  
  while (!IICS_IICIF);                      // Warten bis Übertragung abgeschlossen ist
  IICS_IICIF = 1;                           // Interrupt-Flag löschen
  if (IICS_RXAK)                            // Prüfen ob ACK empfangen wurde
  { 
    IICC_MST = 0;                           // Stop-Condition generieren                       
    IICS_IICIF = 1;                         // Interrupt-Flag löschen
    return EC_I2C_NO_ANSWER;                // NACK => Abbruch
  }
  return EC_SUCCESS;
}

/**
 * Empfängt das letzte Byte über I2C.
 * Beim letzten Byte wird kein ACK gesendet. Dafür
 * folgt aber anschliessend eine Stopbedingung.
 *
 * @return
 *    das Empfangene Datenbyte.
 */
uint8 i2cReceiveLastByte(void)
{
  uint8 data;
  
  IICC_TX = 0;                              // Receive-Mode  
  IICC_TXAK = 1;                            // Beim letzten Byte kein Ack senden damit Stop-Condition ausgegeben werden kann
  data = IICD;                              // Dummy-Read um Übertragung zu starten...
    
  while (!IICS_IICIF);                      // Warten bis Übertragung abgeschlossen ist
  IICS_IICIF = 1;                           // Interrupt-Flag löschen
  IICC_MST = 0;                             // Stop-Condition generieren                       
  data = IICD;                              // eigentliche Daten lesen
   
  return data;
}


/**
 * Sendet ein Byte. Es wird vorausgesetzt, 
 * dass zuvor i2cStart() erfolgreich war.
 *
 * @param [in] data
 *    das zu sendende Byte
 *
 * @return
 *    EC_SUCCESS    falls der Slave immer mit ACK geantwortet hat    
 *    EC_I2C_NAK    falls kein ACK empf. wurde.
 */
tError i2cSendByte(uint8 data)
{
  IICD = data;                              // Datenbyte senden
  while (!IICS_IICIF);                      // Warten bis Übertragung abgeschlossen ist
  IICS_IICIF = 1;                           // Interrupt-Flag löschen  
  if (IICS_RXAK)                            // Prüfen ob ACK empfangen wurde
  {
    IICC_MST = 0;                           // Stop-Condition generieren
    IICS_IICIF = 1;                         // Interrupt-Flag löschen
    return EC_I2C_NAK;                      // NACK => Abbruch
  }
  return EC_SUCCESS;
}


/**
 * Sendet einen Puffer mit Daten. Es wird vorausgesetzt,
 * dass zuvor i2cStart() erfolgreich war.
 *
 * @param [in] buf
 *    die zu sendenden Daten
 * @param [in] length
 *    die Anzahl zu sendenden Bytes
 *
 * @return
 *    EC_SUCCESS    falls der Slave immer mit ACK geantwortet hat    
 *    EC_I2C_NAK    falls kein ACK empf. wurde.
 */
tError i2cSendData(uint8 *buf, uint8 length)
{
  uint8 i;
  for (i=0; i<length; i++)
  {
    IICD = buf[i];                          // Datenbyte senden
    while (!IICS_IICIF);                    // Warten bis Übertragung abgeschlossen ist
    IICS_IICIF = 1;                         // Interrupt-Flag löschen  
    if (IICS_RXAK)                          // Prüfen ob ACK empfangen wurde
    {
      IICC_MST = 0;                         // Stop-Condition generieren
      IICS_IICIF = 1;                       // Interrupt-Flag löschen
      return EC_I2C_NAK;                    // NACK => Abbruch
    }
  }
  return EC_SUCCESS; 
}


/**
 * Liest Daten von einem I2C-Gerät. Es wird vorausgesetzt,
 * dass zuvor i2cStart() erfolgreich war.
 *
 * Achtung. Es wird am Schluss eine Stop-Bedingung generiert!
 *
 * @param [out] buf
 *      die zu emfpangenden Daten
 * @param [in] length
 *      die Anzahl Bytes, die empfangen werden.
 */
void i2cReceiveData(uint8 *buf, uint8 length)
{
  uint8 i;
  IICC_TX = 0;                              // Receive-Mode
    
  if(length > 1)
  {      
    IICC_TXAK = 0;                          // Ab jetzt für empf. Bytes Ack's erzeugen

    buf[0] = IICD;                          // Dummy-Read um Übertragung zu starten und damit 1. Byte zu lesen...
    while (!IICS_IICIF);                    // Warten bis Übertragung abgeschlossen ist
    IICS_IICIF = 1;                         // Interrupt-Flag löschen

    for (i=0; i<length-2; i++)
    {
      buf[i] = IICD;                        // 1. Datenbyte lesen und letzte Übertragung starten...
      while (!IICS_IICIF);                  // Warten bis Übertragung abgeschlossen ist
      IICS_IICIF = 1;                       // Interrupt-Flag löschen
    }

    IICC_TXAK = 1;                          // Beim letzten Byte kein Ack senden damit Stop-Condition ausgegeben werden kann
    buf[length - 2] = IICD;                 // 1. Datenbyte lesen und letzte Übertragung starten...
    while (!IICS_IICIF);                    // Warten bis Übertragung abgeschlossen ist
    IICS_IICIF = 1;                         // Interrupt-Flag löschen
      
    IICC_MST = 0;                           // Stop-Condition generieren
    buf[length-1] = IICD;                   // Letztes Datenbyte lesen
  } 
  else
  {
    IICC_TXAK = 1;                          // Beim letzten Byte kein Ack senden damit Stop-Condition ausgegeben werden kann
    buf[0] = IICD;                          // Dummy-Read und damit letzte Übertragung starten...
    while (!IICS_IICIF);                    // Warten bis Übertragung abgeschlossen ist
    IICS_IICIF = 1;                         // Interrupt-Flag löschen
    
    IICC_MST = 0;                           // Stop-Condition generieren
    buf[0] = IICD;                          // Letztes Datenbyte lesen
  }
}


/**
 * Generiert eine Stopbedingung.
 */
void i2cStop()
{
  IICC_MST = 0;                             // Stop-Condition generieren
  IICS_IICIF = 1;                           // Interrupt-Flag löschen
}





//---------------------------------------------------------------------------------------------------------------------
// I 2 C   -   Z U S A M M E N G E S E T Z T E   F U N K T I O N E N
//---------------------------------------------------------------------------------------------------------------------


/**
 * Prüft, ob ein I2C-Device antwortet.
 *
 * @param [in] ucAdr
 *      die I2C-Adresse des gewünschten Gerätes
 *
 * @return
 *      EC_SUCCESS          falls der Slave immer mit ACK geantwortet hat    
 *      EC_I2C_NO_ANSWER    falls I2C-Gerät nicht geantwortet hat
 */
tError i2cTest(uint8 adr)
{
  tError result;
  
  result = i2cStart(adr, TRUE);
  if (result != EC_SUCCESS) return result;
     
  i2cStop();    
  return EC_SUCCESS;
}



/**
 * Sendet Daten zu einem I2C-Gerät dass zusätzlich ein 
 * Befehls-Byte benötigt.
 *
 * @param [in] adr
 *      die I2C-Adresse des gewünschten Gerätes
 * @param [in] cmd
 *      der gewünschte Befehl (Laut Datenblatt vom Slave)
 * @param [in] data
 *      das zu sendende Datenbyte
 *
 * @return
 *      EC_SUCCESS          falls der Slave immer mit ACK geantwortet hat    
 *      EC_I2C_NAK          falls kein ACK empf. wurde.
 *      EC_I2C_NO_ANSWER    falls I2C-Gerät nicht geantwortet hat
 */
tError i2cWriteCmdByte(uint8 adr, uint8 cmd, uint8 data)
{
  tError result;
  result = i2cStart(adr, TRUE);                   // I2C-Adr senden
  if (result != EC_SUCCESS) return result;
  
  result = i2cSendByte(cmd);                      // Command senden
  if (result != EC_SUCCESS) return result;
  
  result = i2cSendByte(data);                     // Daten senden
  if (result != EC_SUCCESS) return result;
  
  i2cStop();
  
  return EC_SUCCESS;
}


 /**
 * Liest Daten von einem I2C-Gerät dass zusätzlich ein 
 * Befehls-Byte benötigt.
 *
 * @param [in] adr
 *      die I2C-Adresse des gewünschten Gerätes
 * @param [in] cmd
 *      der gewünschte Befehl (Laut Datenblatt vom Slave)
 * @param [out] data
 *      die zu sendenden Daten
 * @param [in] length
 *      die Anzahl Bytes, die gesendet werden.
 *
 * @return
 *      EC_SUCCESS          falls der Slave immer mit ACK geantwortet hat    
 *      EC_I2C_NAK          falls kein ACK empf. wurde.
 *      EC_I2C_NO_ANSWER    falls I2C-Gerät nicht geantwortet hat
 */
tError i2cReadCmdData(uint8 adr, uint8 cmd, uint8 *data, uint8 length)
{
  tError result;
  result = i2cStart(adr, TRUE);                 // I2C-Adr senden
  if (result != EC_SUCCESS) return result;
  
  result = i2cSendData(&cmd, 1);                // Cmd senden
  if (result != EC_SUCCESS) return result;
  
  result = i2cRepeatedStart(adr, FALSE);        // Repeated Start, Wechsel von Write zu Read
  if (result != EC_SUCCESS) return result;
  
  i2cReceiveData(data, length);                 // Daten lesen
  return EC_SUCCESS;   
}


/**
 * Sendet Daten zu einem I2C-Gerät dass zusätzlich ein 
 * Befehls-Byte benötigt.
 *
 * @param [in] adr
 *      die I2C-Adresse des gewünschten Gerätes
 * @param [in] cmd
 *      der gewünschte Befehl (Laut Datenblatt vom Slave)
 * @param [in] data
 *      die zu sendenden Daten
 * @param [in] length
 *      die Anzahl Bytes, die gesendet werden.
 *
 * @return
 *      EC_SUCCESS          falls der Slave immer mit ACK geantwortet hat    
 *      EC_I2C_NAK          falls kein ACK empf. wurde.
 *      EC_I2C_NO_ANSWER    falls I2C-Gerät nicht geantwortet hat
 */
tError i2cWriteCmdData(uint8 adr, uint8 cmd, uint8 *data, uint8 length)
{
  tError result;
  result = i2cStart(adr, TRUE);             // I2C-Adr senden
  if (result != EC_SUCCESS) return result;
  
  result = i2cSendData(&cmd, 1);              // Command senden
  if (result != EC_SUCCESS) return result;
  
  result = i2cSendData(data, length);        // Daten senden
  if (result != EC_SUCCESS) return result;
  
  i2cStop();
  
  return EC_SUCCESS;
}


/**
 * Initialisiert den I2C-Bus
 */
void i2cInit()
{
  // Frequency Divider Register
  IICF_ICR = 0x05;          // 24 MHz/(2*30) = 400kHz
  IICF_MULT = 0x01;
  
  // Control Register
  IICC_IICEN = 1;           // I2C einschalten
}
