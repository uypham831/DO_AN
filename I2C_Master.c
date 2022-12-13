#include "I2C_Master.h"							
volatile uint8_t status;		

void I2C_Init()			//khoi dong I2C								
{
	TWBR = BITRATE(TWSR = 0x00);		//set bit rate value, 100Khz
}


uint8_t I2C_Start(char slave_write_address)						// ham bat dau I2C + address slave
{
									
	TWCR = (1<<TWSTA)|(1<<TWEN)|(1<<TWINT);					// kinh hoat TWI, INTerrupt
	while (!(TWCR & (1<<TWINT)));							// ch? bit finish
	status = TWSR & 0xF8;									//doc gia tri status reg
	if (status != 0x08)										//kiem tra qua trinh star
	return 0;												//tra ve 0 if fail
	TWDR = slave_write_address;								// gui SLA+W 
	TWCR = (1<<TWEN)|(1<<TWINT);							// EN TWI, clear INT
	while (!(TWCR & (1<<TWINT)));							
	status = TWSR & 0xF8;									// Read TWI status register
	if (status == 0x18)										// Check SLA+W transmitted & ack received or not?
	return 1;												
	if (status == 0x20)										// Check SLA+W transmitted & nack received or not? 
	return 2;											
	else
	return 3;												// Else return 3 => SLA+W failed
}

uint8_t I2C_Repeated_Start(char slave_read_address)			// I2C repeated start function
{
										
	TWCR = (1<<TWSTA)|(1<<TWEN)|(1<<TWINT);					
	while (!(TWCR & (1<<TWINT)));							
	status = TWSR & 0xF8;									
	if (status != 0x10)									
	return 0;												
	TWDR = slave_read_address;							
	TWCR = (1<<TWEN)|(1<<TWINT);							
	while (!(TWCR & (1<<TWINT)));							
	status = TWSR & 0xF8;									
	if (status == 0x40)										
	return 1;												
	if (status == 0x20)										
	return 2;												
	else
	return 3;												
}

void I2C_Stop()												
{
	TWCR=(1<<TWSTO)|(1<<TWINT)|(1<<TWEN);					
	while(TWCR & (1<<TWSTO));								
}
void I2C_Stop_()												
{
	TWCR=(1<<TWSTO)|(1<<TWINT)|(1<<TWEN);			
	while(TWCR & (1<<TWSTO));								
}


void I2C_Start_Wait(char slave_write_address)				
{
										
	while (1)
	{
		TWCR = (1<<TWSTA)|(1<<TWEN)|(1<<TWINT);		
		while (!(TWCR & (1<<TWINT)));						
		status = TWSR & 0xF8;								
		if (status != 0x08)							
		continue;											
		TWDR = slave_write_address;							
		TWCR = (1<<TWEN)|(1<<TWINT);						
		while (!(TWCR & (1<<TWINT)));					
		status = TWSR & 0xF8;							
		if (status != 0x18 )								
		{
			I2C_Stop();									
			continue;										
		}
		break;										
	}
}

uint8_t I2C_Write(char data)								
{
											
	TWDR = data;											
	TWCR = (1<<TWEN)|(1<<TWINT);							
	while (!(TWCR & (1<<TWINT)));							
	status = TWSR & 0xF8;									
	if (status == 0x28)										
	return 0;											
	if (status == 0x30)										
	return 1;												
	else
	return 2;												
}

char I2C_Read_Ack()										
{
	TWCR=(1<<TWEN)|(1<<TWINT)|(1<<TWEA);					
	while (!(TWCR & (1<<TWINT)));							
	return TWDR;											
}

char I2C_Read_Nack()										
{
	TWCR=(1<<TWEN)|(1<<TWINT);							
	while (!(TWCR & (1<<TWINT)));							
	return TWDR;	
											
}

