
#include "crc.h"

void InvertUint8(unsigned char *dBuf,unsigned char *srcBuf)
{
	int i;
	unsigned char tmp[4];
	tmp[0] = 0;
	for(i=0;i< 8;i++)
	{
		if(srcBuf[0]& (1 << i))
		tmp[0]|=1<<(7-i);
	}
	dBuf[0] = tmp[0];
}

void InvertUint16(unsigned short *dBuf,unsigned short *srcBuf)
{
	int i;
	unsigned short tmp[4];
	tmp[0] = 0;
	for(i=0;i< 16;i++)
	{
		if(srcBuf[0]& (1 << i))
		tmp[0]|=1<<(15 - i);
	}
	dBuf[0] = tmp[0];
}

unsigned short CRC16Modbus(unsigned char *puchMsg, unsigned int usDataLen)
{
	unsigned short wCRCin = 0xFFFF;
	unsigned short wCPoly = 0x8005;
	unsigned char wChar = 0;

	while (usDataLen--)
	{
		wChar = *(puchMsg++);
		InvertUint8(&wChar,&wChar);
		wCRCin ^= (wChar << 8);
		for(int i = 0;i < 8;i++)
		{
			if(wCRCin & 0x8000)
			wCRCin = (wCRCin << 1) ^ wCPoly;
			else
			wCRCin = wCRCin << 1;
		}
	}
	InvertUint16(&wCRCin,&wCRCin);
	
	return (wCRCin) ;
}

unsigned short GetCheckCode(unsigned char *buf, unsigned short len8)
{
	//CRC??????
	unsigned short checksum = 0xFFFF;
	unsigned short tempsum;
	unsigned short i,j;
	for(i=0;i<len8;i++)
	{
		checksum ^= buf[i];
		for(j=0;j<8;j++)
		{
			tempsum=checksum;
			checksum >>= 1;
			if(tempsum&0x0001)
			{
				checksum ^= 0xA001;
			}
		}
	}
	return checksum;
}

uint8_t CheckAdd8(uint8_t *buf, int len)
{
	uint8_t sum = 0;
	uint16_t i = 0;
	for(i=0;i<len;i++)
	{
		sum += *(buf++);
	}
	//sum = ~sum ;
	return sum;
}

