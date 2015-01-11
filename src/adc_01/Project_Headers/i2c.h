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
 * \b Target:     Medusa-Trainer \n 
 *
 *
 * $Id: i2c.h 525 2013-11-14 15:58:53Z zajost $
 *--------------------------------------------------------------------
 */
#ifndef I2C_H
#define I2C_H


// I2C-Grundfunktionen
tError i2cStart(uint8 adr, bool write);
tError i2cRepeatedStart(uint8 adr, bool write);
uint8 i2cReceiveLastByte(void);
tError i2cSendData(uint8 *buf, uint8 length);
void i2cReceiveData(uint8 *buf, uint8 length);
void i2cStop(void);


// Zusammengesetzte Funktionen
tError i2cTest(uint8 adr);

tError i2cWriteCmdByte(uint8 adr, uint8 cmd, uint8 data);

tError i2cReadCmdData(uint8 adr, uint8 cmd, uint8 *data, uint8 length);
tError i2cWriteCmdData(uint8 adr, uint8 cmd, uint8 *data, uint8 length);

void i2cInit(void);

#endif
