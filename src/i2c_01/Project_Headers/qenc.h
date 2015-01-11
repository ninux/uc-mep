#ifndef QENC_H
#define QENC_H

#define QENC_ADDR			0x2A
#define QENC_CTRL_ADDR		0x00
#define QENC_CTRL_CONFIG	0b00011101
#define QENC_CTRL_RESET		0x00

void qenc_init(void);
void qenc_reset(void);

#endif /* QENC_H */
