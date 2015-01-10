/**
 *--------------------------------------------------------------------\n
 *          HSLU T&A Hochschule Luzern Technik+Architektur            \n
 *--------------------------------------------------------------------\n
 *
 * \brief         Encoder driver
 * \file
 * \author        Christian Jost
 * \date          19.11.2012
 *
 * \b Language:   Ansi-C \n\n
 * \b Target:     MC Car \n
 *
 * $Id: quad.c 525 2013-11-14 15:58:53Z zajost $
 *--------------------------------------------------------------------
 */
#include "platform.h"
#include "quad.h"
#include "i2c.h"


#define I2C_QUAD_ADR            0x54

#define QUAD_STATUS_CONTROL     0
#define QUAD_SPEED              1
#define QUAD_TICKS              5
#define QUAD_ERRORS             9
#define QUAD_CARRIER_MODULO    11


typedef union                           // Register 00: control/status
{
  unsigned char Byte;
  struct
  {
    tQuadMode encMode           :2;     // Bit[0-1]: Encoder Mode: 0=off, 1=Encoder, 2=Calibration 
    uint8 encResetTicks         :1;     // Bit[2]: 1 = resets ticks left & right
    uint8 encResetError         :1;     // Bit[3]: 1 = resets error counter
    uint8 carrierEnable         :1;     // Bit[4]: 0=off, 1=on
    uint8 oledEnable            :1;     // Bit[5]: 0=off, 1=on
    uint8 reserved              :1;     // -
    uint8 power                 :1;     // Bit[7]: 1=power off immediately
  } Bits;
} tQuadStatusControlReg;


typedef union
{
  struct
  {
    tQuadStatusControlReg statusControl;
    int16 speedL;
    int16 speedR; 
    int16 ticksL;
    int16 ticksR;
    uint8 errorL;
    uint8 errorR;
    uint16 carrierModulo;
  } tFields;
  
  char data[sizeof(tFields)];
} tQuadReg;



/**
 * Returns the operating mode
 *  
 * @return
 *    a tQuadMode enum value.
 */
tQuadMode quadGetMode(void)
{
  tQuadStatusControlReg sc;
  tError result = i2cReadCmdData(I2C_QUAD_ADR, QUAD_STATUS_CONTROL, &sc.Byte, sizeof(tQuadStatusControlReg));
  
  if (result == EC_SUCCESS) return sc.Bits.encMode;
  return qmUnkownError;
}


/**
 * Sets the operating mode:
 * - 0 => Off (power saving mode)
 * - 1 => normal operating mode
 * - 2 => calibration mode
 * - 3 => reserved
 * 
 * @param [in] mode
 *    a pointer to a tQuadMode structure.
 * 
 * @return
 *    EC_SUCCESS if the command was executed sucessfully
 *    EC_I2C_NAK if the device answered with an NAK
 *    EC_I2C_NO_ANSWER if the device did not answer (wrong address?)    
 */
tError quadSetMode(tQuadMode mode)
{
  tQuadStatusControlReg sc; 
  tError result = i2cReadCmdData(I2C_QUAD_ADR, QUAD_STATUS_CONTROL, &sc.Byte, sizeof(tQuadStatusControlReg));
  
  if (result == EC_SUCCESS)
  {
    sc.Bits.encMode = (uint8)mode;
    result = i2cWriteCmdData(I2C_QUAD_ADR, QUAD_STATUS_CONTROL, (char*)&sc, sizeof(tQuadStatusControlReg));
  }
  return result;  
}


/**
 * Returns the speed of the left and right wheel.
 * The Speed is in mm/sec.
 * 
 * @param [out] speed
 *    a pointer to a tQuadSpeed structure that includes both speed values
 * 
 * @return
 *    EC_SUCCESS if the command was executed sucessfully
 *    EC_I2C_NAK if the device answered with an NAK
 *    EC_I2C_NO_ANSWER if the device did not answer (wrong address?)    
 */
tError quadGetSpeed(pQuadSpeed speed)
{
  return i2cReadCmdData(I2C_QUAD_ADR, QUAD_SPEED, (char*)speed, sizeof(tQuadSpeed));
}


/**
 * Returns both ticks counter (left and right)
 * 
 * @param [out] ticks
 *    a pointer to a tQuadTicks structure that includes both counters
 * 
 * @return
 *    EC_SUCCESS if the command was executed sucessfully
 *    EC_I2C_NAK if the device answered with an NAK
 *    EC_I2C_NO_ANSWER if the device did not answer (wrong address?)    
 */
tError quadGetTicks(pQuadTicks ticks)
{
  return i2cReadCmdData(I2C_QUAD_ADR, QUAD_TICKS, (char*)ticks, sizeof(tQuadTicks));
}


/**
 * Resets both ticks counter to zero (left and right).
 * 
 * @return
 *    EC_SUCCESS if the command was executed sucessfully
 *    EC_I2C_NAK if the device answered with an NAK
 *    EC_I2C_NO_ANSWER if the device did not answer (wrong address?)    
 */
tError quadResetTicks(void)
{
  tQuadStatusControlReg sc;
  
  tError result = i2cReadCmdData(I2C_QUAD_ADR, QUAD_STATUS_CONTROL, &sc.Byte, sizeof(tQuadStatusControlReg));
  
  if (result == EC_SUCCESS)
  {
    sc.Bits.encResetTicks = TRUE;
    result = i2cWriteCmdData(I2C_QUAD_ADR, QUAD_STATUS_CONTROL, (char*)&sc, sizeof(tQuadStatusControlReg));
  }
  
  return result; 
}


/**
 * Returns both ticks error counter (left and right)
 * 
 * @param [out] errors
 *    a pointer to a tQuadErrors structure that includes both error counters
 * 
 * @return
 *    EC_SUCCESS if the command was executed sucessfully
 *    EC_I2C_NAK if the device answered with an NAK
 *    EC_I2C_NO_ANSWER if the device did not answer (wrong address?)    
 */
tError quadGetErrors(pQuadErrors errors)
{
  return i2cReadCmdData(I2C_QUAD_ADR, QUAD_ERRORS, (char*)errors, sizeof(tQuadErrors));
}


/**
 * Resets both ticks error counter to zero.
 * 
 * @return
 *    EC_SUCCESS if the command was executed sucessfully
 *    EC_I2C_NAK if the device answered with an NAK
 *    EC_I2C_NO_ANSWER if the device did not answer (wrong address?)    
 */
tError quadResetErrors(void)
{
  tQuadStatusControlReg sc;
  
  tError result = i2cReadCmdData(I2C_QUAD_ADR, QUAD_STATUS_CONTROL, &sc.Byte, sizeof(tQuadStatusControlReg));
  
  if (result == EC_SUCCESS)
  {
    sc.Bits.encResetError = TRUE;
    result = i2cWriteCmdData(I2C_QUAD_ADR, QUAD_STATUS_CONTROL, &sc.Byte, sizeof(tQuadStatusControlReg));
  }
  
  return result; 
}
