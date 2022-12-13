#ifndef AM2320
#define AM2320
#include <avr/io.h>
#include <stdlib.h>
#include <stdio.h>
#include <util/delay.h>
#include "I2C_Master.h"
uint8_t data_AM2320[7];
unsigned int CRC_data = 0x0000;
unsigned int CRC_temp = 0x0000;

void Wake_AM2320(){
	I2C_Start(0xB8);
	I2C_Stop(); 
}
void AM2320_readBytes()
{
	Wake_AM2320();
	/*write function code to point to data register of sensor*/
	I2C_Start(0xB8);           /*send write address of AM2320*/
	I2C_Write(0x03);           /*send function code: read data reg*/
	I2C_Write(0x00);           /*send start address of data reg*/
	I2C_Write(0x04);            /*send number of registers to read*/
	I2C_Stop();                /*send STOP condition*/

	_delay_ms(15);               /*wait 15ms*/

	/*read & store humidity & temp bytes from sensor*/
	I2C_Start(0xB9);            /*send read address of AM2320*/
	//------------------------------------------------------------------
	for(int i=0;i < 7 ;i++){
		data_AM2320[i] = I2C_Read_Ack();
	}
	data_AM2320[7] = I2C_Read_Nack();
	I2C_Stop(); 

}
//========================================================================

unsigned int CRC16(unsigned char *ptr, unsigned char length)
{
	unsigned int crc = 0xFFFF;
	unsigned char s = 0x00;

	while(length--)
	{
		crc ^= *ptr++;
		for(s = 0; s < 8; s++)
		{
			if((crc & 0x01) != 0)
			{
				crc >>= 1;
				crc ^= 0xA001;
			}
			else
			{
				crc >>= 1;
			}
		}
	}

	return crc;
}
void Get_CRC(unsigned int *CRC_data)
{
	*CRC_data = ((unsigned int)((data_AM2320[7] << 8) | data_AM2320[6]));
}

#endif
