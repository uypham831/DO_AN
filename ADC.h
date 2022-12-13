#include <avr/io.h>
#include <util/delay.h>

void ADC_init(){
	//tạo chân PC0 input analog
	DDRC &= ~(1<<PINC0);
	//kich hoạt ADC module, đặt prescalar là 128 ==> 16M/128 = 125K
	ADCSRA |= (1<<ADEN) | (1<<ADPS2) | (1<<ADPS1) | (1<<ADPS0);
	//đặt điện áp tham chiếu và chọn kênh ADC
	ADMUX = 0b01000000;     // đặt REFS1 = 0 |REFS0 = 1 (Vref = AVCC) | ADLAR = 0(chỉnh format thanh ghi data |  ADC0
}
int ADC_Read(char channel)
{
	
	ADMUX=ADMUX|(channel & 0x0f);	//chọn kênh ADC để đọc

	ADCSRA |= (1<<ADSC);		// bắt đầu
	while((ADCSRA&(1<<ADIF))==0);	//chờ cờ ngắt ADC kết thúc
	ADCSRA |= (1<<ADIF);
	_delay_us(500);
	return ADCW;				
}