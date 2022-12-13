#ifndef esp8266
#define esp8266
#include "UART.h"
#include <util/delay.h>
char *SSID="Uyyy";                  //my wifi
char *PASS="minhuy83";                //my wifi
char *WEBSITE="myhomemonitoring831.000webhostapp.com";  //my web

void HTTP_connect()
{
	UART_chuoi("AT\r\n");//reset command beffor
	_delay_ms(500);
	UART_chuoi("ATE0\r\n");_delay_ms(500);
	UART_chuoi("AT+CWMODE=1\r\n");_delay_ms(500);
	UART_chuoi("AT+CIPMUX=0\r\n");_delay_ms(500);
	//UART_chuoi("AT+CIPSTART=\"TCP\",\"145.14.145.187\",80\r\n");//ket noi toi website https://myhomemonitoring831.000webhostapp.com/
	_delay_ms(1500);
}
void init_esp()
{
	UART_chuoi("AT\r\n");_delay_ms(500);
	UART_chuoi("ATE0\r\n");_delay_ms(500);
	UART_chuoi("AT+CWMODE=1\r\n");_delay_ms(500);
	UART_chuoi("AT+CIPMUX=0\r\n");_delay_ms(500);
	UART_chuoi("AT+CWJAP=\"Uyyy\",\"minhuy83\"\r\n");_delay_ms(2000);

}
void GET_StatusLED()
{
	int length;
	char data[90];
	char tmp_send[90];
	sprintf(data,"GET /kiemtra.php HTTP/1.1\r\nHost: myhomemonitoring831.000webhostapp.com\r\n\r\n");
	sprintf(tmp_send,"AT+CIPSTART=\"TCP\",\"145.14.145.187\",80\r\n");
	UART_chuoi(tmp_send);_delay_ms(400);  //wait connect
	length=strlen(data);
	sprintf(tmp_send,"AT+CIPSEND=%i\r\n",length);
	UART_chuoi(tmp_send);_delay_ms(100);
	UART_chuoi(data);
	_delay_ms(1500);
	UART_chuoi("AT+CIPCLOSE\r\n");_delay_ms(200);
}
void POST_dataCB(int LPG_i,int SMK_i,float _humidity, float _temperature){
	int length;
	char data[90];
	char tmp_send[90];
	sprintf(data,"GET /server.php?lpg=%i&smk=%i&hum=%.2f&temp=%.2f HTTP/1.1\r\nHost: myhomemonitoring831.000webhostapp.com\r\n\r\n",LPG_i,SMK_i,_humidity,_temperature);
	sprintf(tmp_send,"AT+CIPSTART=\"TCP\",\"145.14.145.187\",80\r\n");
	UART_chuoi(tmp_send);_delay_ms(400);  //wait connect
	length=strlen(data);
	sprintf(tmp_send,"AT+CIPSEND=%i\r\n",length);
	UART_chuoi(tmp_send);_delay_ms(100);
	UART_chuoi(data);
	_delay_ms(1500);
	UART_chuoi("AT+CIPCLOSE\r\n");_delay_ms(200);
}
#endif