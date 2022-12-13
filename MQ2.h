#ifndef MQ2
#define MQ2
#include <util/delay.h>
#include "ADC.h"
#include "i2c_lcd.h"
#define RL_VALUE		(10)     //điện trở tải trên board MQ2
#define RO_CLEAN_AIR_FACTOR (9.83)  //RS/RO trên biểu đồ dữ liệu trong datasheet
#define LPG (0)         // Gas identity no.
#define SMOKE (1)
	
float Sensor_Resistor(int adc_value);
float Sensor_Calibration();
float Read_Sensor();
int Get_PPM_Sel(float RS_R0, int Sel);
int  Get_PPM(float RS_R0, float *curve);	
char Res[16], lpg[16], smoke[16];
char buffer_MQ[20];
void MQ2_init();
void MQ2_Display();

volatile float Ro=10;			//giá trị ban đầu Ro
float LPG_Curve[3] = {2.3,0.20,-0.45}; //format:{ x, y, Hso góc}; đường LPG
float Smoke_Curve[3] ={2.3,0.53,-0.43}; //format:{ x, y, Hso góc}; đường Smoke
int LPG_i,SMK_i;
	
	void MQ2_init(){
		I2C_LCD_CMD(0x94);
		I2C_LCD_STR("Chuan hoa R0...");
		Ro = Sensor_Calibration();
		dtostrf(Ro, 6, 2, Res);
		I2C_LCD_CMD(0x94);
		I2C_LCD_STR("Chuan hoa xong...");
		I2C_LCD_CMD(0xd4);
		sprintf(buffer_MQ, "R0=");
		strcat(buffer_MQ, Res);
		strcat(buffer_MQ, "KOhm");
		I2C_LCD_STR(buffer_MQ);
		_delay_ms(3000);
		I2C_LCD_CLR();
	}
	void MQ2_Display(){
		LPG_i = Get_PPM_Sel(Read_Sensor()/Ro,LPG);
		itoa(LPG_i, lpg, 10);
		sprintf(buffer_MQ,"LPG=");
		strcat(buffer_MQ,lpg);
		I2C_LCD_GOTO(4,1);
		I2C_LCD_STR(buffer_MQ);
		SMK_i = Get_PPM_Sel(Read_Sensor()/Ro,SMOKE);
		itoa(SMK_i, smoke, 10);
		sprintf(buffer_MQ,"SMO=");
		strcat(buffer_MQ,smoke);
		I2C_LCD_GOTO(4,11);
		I2C_LCD_STR(buffer_MQ);
		if(Get_PPM_Sel(Read_Sensor()/Ro,LPG)==0){
			I2C_LCD_GOTO(4,6);
			I2C_LCD_STR("     ");
			I2C_LCD_GOTO(4,16);
			I2C_LCD_STR("     ");
		}
		
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
#endif