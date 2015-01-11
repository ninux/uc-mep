#ifndef I2C_H
#define I2C_H

#define I2C_SUCCESS		1
#define I2C_FAIL		0

void i2c_init(void);
int i2c_start(uint8 addr, bool write);
int i2c_repeated_start(uint8 addr, bool write);
int i2c_send_byte(uint8 databyte);
void i2c_stop(void);

#endif /* I2C_H */
