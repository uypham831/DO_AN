
#ifndef UART
#define UART
#define BAUD_RATE_115200_BPS  16
#include <avr/io.h>
#include <string.h>
#include <avr/interrupt.h>
void UART_Init()
{
	unsigned int ubrr = BAUD_RATE_115200_BPS;
	UBRR0H = (ubrr>>8);
	UBRR0L = (ubrr);
	UCSR0A = 0x02;		//Double the USART Transmission Speed
	UCSR0C = (1<<UCSZ01)|(1<<UCSZ00);	//8 bitdata,1 stop, 0 parity
	UCSR0B = (1<<RXEN0)|(1<<TXEN0)|(1<<RXCIE0);	//enable transmit, receive, ISR Re
}
void UART_gui(char data)
{
	while (!(UCSR0A & (1<<UDRE0)));
	UDR0 = data;
}
unsigned char UART_nhan()
{
	while (!(UCSR0A & (1<<RXC0)));
	return UDR0;
}
void UART_chuoi(char* s)
{
	// transmit character until NULL is reached
	while(*s)
	 {
		 UART_gui(*s);
		 s++;
	 }
}
#endif