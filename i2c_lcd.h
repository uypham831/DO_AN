#ifndef i2c_lcd
#define i2c_lcd
#include <avr/io.h>
#define F_CPU	16000000UL
#include <util/delay.h>
#include "I2C_Master.h"


#define PCF8574AT_WR	0x7E							//--- Slave Address is 7-Bit and Last Bit is either read or write
#define PCF8574AT_RD	0x7F

unsigned char lcd = 0x00;						//--- Declaring a variable as lcd for data operation

/* Function to Write data in PCF8574 */

void PCF8574_write(unsigned char x)
{
	I2C_Start(PCF8574AT_WR);
	I2C_Write(x);						//--- Data to Slave Device
	I2C_Stop();								//--- Stop Condition
}

/* Function to Write 4-bit data to LCD */

void I2C_LCD_4BIT(unsigned char x)
{
	unsigned char temp = 0x00;					//--- temp variable for data operation
	
	lcd &= 0x0F;								//--- Masking last four bit to prevent the RS, RW, EN, Backlight
	temp = (x & 0xF0);							//--- Masking higher 4-Bit of Data and send it LCD
	lcd |= temp;								//--- 4-Bit Data and LCD control Pin
	lcd |= (0x04);								//--- Latching Data to LCD EN = 1
	PCF8574_write(lcd);							//--- Send Data From PCF8574 to LCD PORT
	_delay_us(25);								//--- 1us Delay
	lcd &= ~(0x04);								//--- Latching Complete
	PCF8574_write(lcd);							//--- Send Data From PCF8574 to LCD PORT
	_delay_us(25);								//--- 5us Delay to Complete Latching
	
	
	temp = ((x & 0x0F)<<4);						//--- Masking Lower 4-Bit of Data and send it LCD
	lcd &= 0x0F;								//--- Masking last four bit to prevent the RS, RW, EN, Backlight
	lcd |= temp;								//--- 4-Bit Data and LCD control Pin
	lcd |= (0x04);								//--- Latching Data to LCD EN = 1
	PCF8574_write(lcd);							//--- Send Data From PCF8574 to LCD PORT
	_delay_us(25);								//--- 1us Delay
	lcd &= ~(0x04);								//--- Latching Complete
	PCF8574_write(lcd);							//--- Send Data From PCF8574 to LCD PORT
	_delay_us(25);								//--- 5us Delay to Complete Latching
	
}

/* Function to Write to LCD Command Register */

void I2C_LCD_CMD(unsigned char x)
{
	lcd = 0x08;									//--- Enable Backlight Pin
	lcd &= ~(0x01);								//--- Select Command Register By RS = 0
	PCF8574_write(lcd);							//--- Send Data From PCF8574 to LCD PORT
	_delay_us(25);
	I2C_LCD_4BIT(x);						//--- Function to Write 4-bit data to LCD
	
}

/* Function to Write to LCD Command Register */

void I2C_LCD_DWR(unsigned char x)
{
	lcd |= 0x09;								//--- Enable Backlight Pin & Select Data Register By RS = 1
	PCF8574_write(lcd);							//--- Send Data From PCF8574 to LCD PORT
	_delay_us(25);
	I2C_LCD_4BIT(x);						//--- Function to Write 4-bit data to LCD
}

/* Function to Send String of Data */

void I2C_LCD_STR(char *c)
{
	while (*c != '\0')							//--- Check Pointer for Null
	I2C_LCD_DWR(*c++);							//--- Send the String of Data
}

/* Function to Execute Clear LCD Command */

void I2C_LCD_CLR()
{
	I2C_LCD_CMD(0x01);
	_delay_us(50);
}

/* Function to Initialize LCD in 4-Bit Mode, Cursor Setting and Row Selection */

void I2C_LCD_INIT()
{
	lcd = 0x04;						//--- EN = 1 for 25us initialize Sequence
	PCF8574_write(lcd);
	_delay_us(25);
	
	I2C_LCD_CMD(0x03);				//--- Initialize Sequence
	I2C_LCD_CMD(0x02);				//--- Return to Home
	I2C_LCD_CMD(0x28);				//--- 4-Bit Mode 2 - Row Select
	I2C_LCD_CMD(0x0E);				//--- Cursor on, Blinking on
	I2C_LCD_CMD(0x01);				//--- Clear LCD
	I2C_LCD_CMD(0x06);				//--- Auto increment Cursor
	I2C_LCD_CMD(0x80);
}
void I2C_LCD_GOTO(unsigned char x, unsigned char y){
	unsigned char firstcharadr[] = {0x80, 0xC0, 0x94, 0xD4};
	I2C_LCD_CMD(firstcharadr[x-1] + y-1);
	_delay_us(100);
}
#endif