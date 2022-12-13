#include "MQ2.h"
void MQ2_init(){
	I2C_LCD_CMD(0x94);
	I2C_LCD_STR("Calibrating...");
	Ro = Sensor_Calibration();
	dtostrf(Ro, 6, 2, Res);
	I2C_LCD_CMD(0x94);
	I2C_LCD_STR("Calibration done...");
	I2C_LCD_CMD(0xd4);
	sprintf(buffer_MQ, "R0=");
	strcat(buffer_MQ, Res);
	strcat(buffer_MQ, "KOhm");
	I2C_LCD_STR(buffer_MQ);
	_delay_ms(3000);
	I2C_LCD_CLR();
}
void MQ2_Display(){
	itoa(Get_PPM_Sel(Read_Sensor()/Ro,LPG), lpg, 10);
	sprintf(buffer_MQ,"LPG=");
	strcat(buffer_MQ,lpg);
	strcat(buffer_MQ,lpg);
	I2C_LCD_GOTO(4,1);
	I2C_LCD_STR(buffer_MQ);
	itoa(Get_PPM_Sel(Read_Sensor()/Ro,SMOKE), smoke, 10);
	sprintf(buffer_MQ,"SMOKE=");
	strcat(buffer_MQ,smoke);
	strcat(buffer_MQ,smoke);
	I2C_LCD_GOTO(4,10);
	I2C_LCD_STR(buffer_MQ);
}
float Sensor_Resistor(int adc_value){
	return ( ((float)RL_VALUE*(1023-adc_value)/adc_value));   // Tính điện trở cảm biến RS
}
float Sensor_Calibration(){
	int i;                                   // Giả định trong không khí sạch
	float val=0;
	for (i=0;i<50;i++){                   //Tính giá trị trung bình
		val += Sensor_Resistor(ADC_Read(0));
	_delay_ms(500);}
	val = val/50;
	val = val/RO_CLEAN_AIR_FACTOR;           //chia cho hằng số RO_CLEAN_AIR_FACTOR	để được R0
	return val;
}
float Read_Sensor(){
	int i;
	float rs=0;
	for (i=0;i<5;i++) {                                 // lấy giá trị trung bình
		rs += Sensor_Resistor(ADC_Read(0));   // RS thay đổi theo môi trường nhất định
	_delay_ms(50);}
	rs = rs/5;
	return rs;
}
int Get_PPM_Sel(float RS_R0, int Sel){
	if ( Sel == LPG ) {
		return Get_PPM(RS_R0,LPG_Curve);
	}
	else if( Sel == SMOKE ) {
		return Get_PPM(RS_R0,Smoke_Curve);
	}

	return 0;
}
int  Get_PPM(float RS_R0, float *curve){
	return (pow(10,( ((log(RS_R0)-curve[1])/curve[2]) + curve[0])));
}