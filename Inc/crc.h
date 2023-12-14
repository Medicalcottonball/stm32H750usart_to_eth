#ifndef _CRC16_H
#define _CRC16_H

#include "main.h"

unsigned short CRC16Modbus(unsigned char *buf, unsigned int len);
unsigned short GetCheckCode(unsigned char *buf, unsigned short len8);
uint8_t CheckAdd8(unsigned char *buf, int  len);
#endif
