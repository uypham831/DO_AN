/*
 * I2C_Master.c
 *
 * Created: 10/7/2022 9:12:21 PM
 * Author : __DELL__Minh UY
 */
#define F_CPU	16000000UL 

#include "I2C_Master.h"
#include "i2c_lcd.h"
#include "DS1307.h"
#include "AM2320.h"
#include "UART.h"
#include "esp8266.h"
#include "MQ2.h"

volatile char timer1;
volatile char temp_h;
char* days[7]= {"Sun","Mon","Tue","Wed","Thu","Fri","Sat"};
char buffer[20];
int ring;
volatile	int i_reciev;
 float _humidity,_temperature;
volatile char KH_BD=0;
volatile char KH=0;
char second_temp=0;

void baodong(){
	TIMSK0 = (1 << TOIE0) ; //enable interrupt
	TCNT0 = 0x06;
	TCCR0B = (1 << CS00)|(1 << CS02);
	//sbi(PORTB,PINB0);
	ring = 0;
}

ISR(INT1_vect){
	TIMSK0 = (1 << TOIE0) ; //enable interrupt
	TCNT0 = 0x06;
	TCCR0B = (1 << CS00)|(1 << CS02);
	sbi(PORTB,PINB0);
	ring = 0;
}

ISR(TIMER0_OVF_vect)
{
	if(ring < 400) {
		tbi(PORTB,PINB0);
		tbi(PORTD,PIND6);
		TCNT0 = 0x06;
		ring++;
	}else if(ring >= 400){
		ring = 0;
		cbi(PORTB,PINB0);
		cbi(PORTD,PIND6);
		TCCR0B &= 0xF8;
		}
}
ISR(INT0_vect) {		// CB chuyenr ?ong

	sbi(PORTD,PIND5);
	TCCR1B = (1<<CS12); //??t pre-scalar 256
	TCNT1H = 0x0B;	//set 1s
	TCNT1L = 0xDC;
	TIMSK1 = (1 << TOIE1) ; //enable interrupt
	timer1 = 0;
	cbi(EIMSK,INT0);
}
ISR (TIMER1_OVF_vect)    // Timer1 ISR
{
	timer1++;
	if(timer1 == 15){
		cbi(PORTD,PIND5);
		if(KH == 1){
			baodong();
		}
		TCCR1B &= 0xF8;
		sbi(EIMSK,INT0);
		cbi(PORTD,PIND5);
	} else {
		TCNT1H = 0x0B;
		TCNT1L = 0xDC;
	}
}
ISR(USART_RX_vect)
{
	unsigned char pc;
	pc=UDR0;//Gan ki tu vua nhan duoc vao bien tam
	switch(i_reciev){
		case 0:
		if(pc == '8')i_reciev++;
		break;
		case 1:
		if(pc == '3')i_reciev++;
		break;
		case 2:
		switch(pc){
			//---------bat ki tu bat--------//
			case 'b':
			i_reciev=0;
			if((PORTC & (1<<PORTC2)) == 0)sbi(PORTC,PINC2);
			break;
			case 't':
			i_reciev=0;
			if((PORTC & (1<<PORTC2)) != 0)cbi(PORTC,PINC2);
			break;
			default: break;
		}
		default: break;
		
	}
}

int main(void)
{
	I2C_Init();				
	I2C_LCD_INIT();
	UART_Init();
	init_esp();
	ADC_init(); 
	MQ2_init();
	//set chân PD2 input, cam bien chuyen dong
	cbi(DDRD,PIND2);
	cbi(PORTD,PIND2);	// tri-z
	//set chân PD3 input, cam bien chuyen dong
	cbi(DDRD,PIND3);
	cbi(PORTD,PIND3);
	sbi(DDRD,PIND5);
	//set chân PD4 input, sw che do bao dong
	cbi(DDRD,PIND4);
	cbi(PORTD,PIND4);
	//dieu khien coi
	sbi(DDRB,PINB0);
	cbi(PORTB,PINB0);
	//dieu khien den canh bao do
	sbi(DDRD,PIND7);
	cbi(PORTD,PIND7);
	//dieu khien den canh bao vang
	sbi(DDRD,PIND6);
	cbi(PORTD,PIND6);
	//dieu khien dèn
	sbi(DDRC,PINC2);
	cbi(PORTC,PINC2);
	//set ngat ngoài INT0; rising edge
	sbi(EICRA,ISC01);	
	sbi(EICRA,ISC00);
	sbi(EIMSK,INT0) ;
	//INT1 => Gas+nhietdo: falling edge
	sbi(EICRA,ISC11);
	cbi(EICRA,ISC10);
	sbi(EIMSK,INT1) ;
	sei();
	/*
	char _hour = 12;
	char _minute = 4;
	char _second = 45;
	char _day = 1;
	char _date = 11;
	char _month = 12;
	char _year = 22;
	RTC_Clock_Write(dec2bcd(_hour),dec2bcd(_minute),dec2bcd(_second));
	RTC_Calendar_Write(dec2bcd(_day), dec2bcd(_date), dec2bcd(_month), dec2bcd(_year));
	*/
	while (1)
	{
		
	MQ2_Display();
    GET_StatusLED();
	RTC_Read_Clock(0);	// Read clock with second add. i.e location is 0 
	
	if(KH_BD != 1){
		if((PIND & (1<<PIND4)) != 0){
			second_temp = second;
			sbi(PORTC,PINC2);
			KH_BD = 1;
		}
	}
	if((PIND & (1<<PIND4)) == 0){
		cbi(PORTC,PINC2);
		cbi(PORTB,PINB0);
		KH_BD = 0;
		KH = 0;
		second_temp = second;
	}
	if((second - second_temp) >= 10){
		second_temp = second;
		KH = 1;
	}
	
	if(temp_m != minute)
	{
		minute = temp_m;
		sprintf(buffer, "%02x:%02x", (hour & 0b00111111), minute);
		I2C_LCD_CMD(0x80);
		I2C_LCD_STR(buffer);
	}
	if(temp_h != hour)
	{
		temp_h = hour;
		RTC_Read_Calendar(3);	// Read calendar with day address i.e location is 3
		sprintf(buffer, "%02x/%02x/%02x %3s", date, month, year,days[day-1]);
		I2C_LCD_CMD(0xC0);
		I2C_LCD_STR(buffer);
	}

	AM2320_readBytes();
	Get_CRC(&CRC_temp);
	CRC_data = CRC16(data_AM2320, 6);
	if(CRC_temp == CRC_data){
		  int humidity = ((data_AM2320[2] << 8) | data_AM2320[3]);
		  _humidity =  humidity / 10.0;
		  dtostrf(_humidity, 4, 1, buffer);
		  strcat(buffer, "%");
		  I2C_LCD_GOTO(3,16);
		  I2C_LCD_STR(buffer);
		  I2C_LCD_GOTO(3,12);
		  I2C_LCD_STR("Hum=");
		  int temperature = ((data_AM2320[4] & 0x7F) << 8) | data_AM2320[5];
		  if(temperature >= 450){
			  baodong();
			  tbi(PORTD,PIND7);
			  }else cbi(PORTD,PIND7);
		  if ((data_AM2320[2] & 0x80) >> 8 == 1) {       // negative temperature
			  _temperature = (temperature / 10.0) * -1;    // devide data by 10 according to the datasheet
			  dtostrf(_temperature, 4, 1, buffer);
			  strcat(buffer, "°C");
			  I2C_LCD_GOTO(3,1);
			  I2C_LCD_STR("Tem=");
			  I2C_LCD_GOTO(3,5);
			  I2C_LCD_STR(buffer);
		  } else {                                  // positive temperature
			  _temperature = temperature / 10.0;           // devide data by 10 according to the datasheet
			  dtostrf(_temperature, 4, 1, buffer);
			  strcat(buffer, "°C");
			  I2C_LCD_GOTO(3,1);
			  I2C_LCD_STR("Tem=");
			  I2C_LCD_GOTO(3,5);
			  I2C_LCD_STR(buffer);
			}
			} else {
			I2C_LCD_GOTO(3,5);
			I2C_LCD_STR(" --.- ");
			I2C_LCD_GOTO(3,12);
			I2C_LCD_STR(" --.- ");
		}
	}
}