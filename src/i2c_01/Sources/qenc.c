#include "platform.h" /* include peripheral declarations */
#include "qenc.h"
#include "i2c.h"


void qenc_init(void)
{
	if (i2c_start(QENC_ADDR, 1)) {
	    if (i2c_send_byte(QENC_CTRL_ADDR)) {
	    	i2c_send_byte(QENC_CTRL_CONFIG);
	    }
	}
	i2c_stop();
}

void qenc_reset(void)
{
	if (i2c_start(QENC_ADDR, 1)) {
	    if (i2c_send_byte(QENC_CTRL_ADDR)) {
	    	i2c_send_byte(QENC_CTRL_RESET);
	    }
	}
	i2c_stop();
}


