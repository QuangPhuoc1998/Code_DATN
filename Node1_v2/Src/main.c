#include "main.h"
#include "stdio.h"
#include "i2c_lcd.h"
#include "stdbool.h"
#include <string.h>
#include <stdlib.h>

#define DS1307_ADRESS (0x68<<1)
#define LM75_ADRESS (0x48<<1)
#define GY30_ADRESS (0x23<<1)
#define QMC5883_ADRESS (0x0D<<1)

#define D_1P 0
#define D_15P 1
#define D_30P 2
#define D_1H 3

/*--- global variable ---*/
char Rx_Data[2], Rx_Buffer[50];
uint8_t Rx_Indx = 0;
char buffer_to_send[64];
uint8_t I2C_Buffer[8];

volatile uint8_t cout_hs = 0;
volatile uint32_t HS1101_sum;

volatile uint16_t temperature;
volatile uint16_t sun;
volatile uint32_t HS1101 = 0;
volatile uint16_t re_HS1101 = 0;
volatile int32_t HX711_raw;
volatile uint8_t set_duty = D_1P;
volatile uint8_t time[6];

volatile uint32_t HX711_Sum = 0;
volatile uint8_t HX711_count = 0;

uint8_t page = 1;
volatile bool send_done = false;
/*---*/
volatile uint32_t timer2 = 0;
/*--- struct ---*/
typedef struct
{
	uint8_t sec;
	uint8_t min;
	uint8_t hour;
	uint8_t day;
	uint8_t date;
	uint8_t month;
	uint8_t year;
}DS1307_t;
typedef struct
{
	uint8_t TempH;
	uint8_t TempL;
	uint8_t Config;
	uint8_t Thyst;
	uint8_t Tset;
}LM75_t;
typedef struct
{
	uint8_t power_down;
	uint8_t power_on;
	uint8_t reset;
	uint8_t Conti_H_Mode1;
	uint8_t Conti_H_Mode2;
	uint8_t Conti_L_Mode;
	uint8_t One_H_mode1;
	uint8_t One_H_mode2;
	uint8_t One_L_mode;
}GY30_t;
/*---*/
volatile DS1307_t DS1307;
volatile LM75_t LM75;
volatile GY30_t GY30;
/*-----------------------*/

I2C_HandleTypeDef hi2c1;
I2C_HandleTypeDef hi2c2;

TIM_HandleTypeDef htim1;
TIM_HandleTypeDef htim2;
TIM_HandleTypeDef htim3;

UART_HandleTypeDef huart1;

void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_I2C1_Init(void);
static void MX_I2C2_Init(void);
static void MX_TIM1_Init(void);
static void MX_TIM2_Init(void);
static void MX_TIM3_Init(void);
static void MX_USART1_UART_Init(void);

/*--- My function ---*/
int RTC_BCB2DEC(int c);
int RTC_DEC2BCB(int c);
void I2C_WriteBuffer(I2C_HandleTypeDef hi, uint8_t DEV_ADDR, uint8_t sizebuf);
void I2C_ReadBuffer(I2C_HandleTypeDef hi, uint8_t DEV_ADDR, uint8_t sizebuf);
void RTC_GetTime(void);
void RTC_SetTime(uint8_t hour, uint8_t min, uint8_t sec, uint8_t date, uint8_t month, uint8_t year);
void RTC_SetIRQ(void);
uint16_t LM75_GetValue(void);
uint16_t GY30_GetValue(void);
int32_t HX711_GetValue(void);
void delay_us (uint16_t us);
void page1(void);
void page2(void);
void page3(void);
/*-------------------*/
int main(void)
{
  HAL_Init();
  SystemClock_Config();

  MX_GPIO_Init();
  MX_I2C1_Init();
  MX_I2C2_Init();
  MX_TIM1_Init();
  MX_TIM2_Init();
	MX_TIM3_Init();
  MX_USART1_UART_Init();
	
	//RTC_SetTime(0,0,0,0,0,0);
	//RTC_SetIRQ();
	lcd_init();
	HAL_Delay(500);
	lcd_send_string("Ready");
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_11|GPIO_PIN_12, GPIO_PIN_RESET);
	HAL_TIM_IC_Start_IT(&htim1,TIM_CHANNEL_1);
	HAL_UART_Receive_IT(&huart1, (uint8_t*) Rx_Data, 1);
	HAL_Delay(500);
	
	
  while (1)
  {
		lcd_init();
		page1();
  }
}
/*--- page 1 ---*/
void page1(void)
{
	lcd_init();
	lcd_goto_XY(1,0); 
	lcd_send_string("Temp:         ");
	lcd_goto_XY(2,0);
	lcd_send_string("Humi:         ");
	while(1)
	{
		
		lcd_goto_XY(1,6);
		lcd_send_data(temperature/100+48);
		lcd_send_data(temperature%100/10+48);
		lcd_send_data('.');
		lcd_send_data(temperature%10+48);
		lcd_goto_XY(1,11);
		lcd_send_string("oC  ");
		
		lcd_goto_XY(2,6);
		lcd_send_data(HS1101/100+48);
		lcd_send_data(HS1101%100/10+48);
		lcd_send_data(HS1101%10+48);
		lcd_goto_XY(2,10);
		lcd_send_string("%  ");
		
		if(HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_6)== 0)
		{
			while(HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_6)== 0);
			if(3 == page); else page++;
			switch (page)
				{
					case 1: 
					page1();
					break;
					case 2: 
					page2();
					break;
					case 3: 
					page3();
					default: break;
				}
		}
		if(HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_7)== 0)
		{
			while(HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_7)== 0);
			if(1 == page); else page--;
			switch (page)
				{
					case 1: 
					page1();
					break;
					case 2: 
					page2();
					break;
					case 3: 
					page3();
					default: break;
				}
		}
		
		if(HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_4) == 0)
			{
			while(HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_4)== 0);

			sprintf(buffer_to_send,"1,%d,%d,%d,%d,%d,%dW", temperature, HS1101, sun, HX711_raw,DS1307.hour,0);
			for(int i = 0; buffer_to_send[i] != 0; i++)
			{
				printf("%c",buffer_to_send[i]);
				HAL_Delay(10);
			}
		}
			
		HX711_Sum += ((HX711_GetValue()-8288900)/502)-75;
		if(HX711_count == 5)
		{
			HX711_raw = HX711_Sum/5;
			if(HX711_raw > 1000) HX711_raw = 0;
			HX711_Sum = 0;
			HX711_count = 0;
		}		
		HX711_count++;
		//HX711_raw = ((HX711_GetValue()-8288900)/502)-66;
		//HX711_raw = HX711_GetValue();
		HAL_Delay(100);
	}
}
/*--- page 2 ---*/
void page2(void)
{
	lcd_init();
	lcd_goto_XY(1,0);
	lcd_send_string("Sun:        ");
	lcd_goto_XY(2,0);
	lcd_send_string("Rain:       ");
	while(1)
	{
		
		lcd_goto_XY(1,6);
		lcd_send_data(sun/10000+48);
		lcd_send_data(sun%10000/1000+48);
		lcd_send_data(sun%1000/100+48);
		lcd_send_data(sun%100/10+48);
		lcd_send_data(sun%10+48);
		lcd_goto_XY(1,12);
		lcd_send_string("Lux  ");
		
		lcd_goto_XY(2,6);
		lcd_send_data(HX711_raw/1000000+48);
		lcd_send_data(HX711_raw%1000000/100000+48);
		lcd_send_data(HX711_raw%100000/10000+48);
		lcd_send_data(HX711_raw%10000/1000+48);
		lcd_send_data(HX711_raw%1000/100+48);
		lcd_send_data(HX711_raw%100/10+48);
		lcd_send_data(HX711_raw%10+48);
		
		if(HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_6)== 0)
		{
			while(HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_6)== 0);
			if(3 == page); else page++;
			switch (page)
				{
					case 1: 
					page1();
					break;
					case 2: 
					page2();
					break;
					case 3: 
					page3();
					default: break;
				}
		}
		if(HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_7)== 0)
		{
			while(HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_7)== 0);
			if(1 == page); else page--;
			switch (page)
				{
					case 1: 
					page1();
					break;
					case 2: 
					page2();
					break;
					case 3: 
					page3();
					default: break;
				}
		}
		
		if(HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_4) == 0)
			{
			while(HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_4)== 0);

			sprintf(buffer_to_send,"1,%d,%d,%d,%d,%d,%dW", temperature, HS1101, sun, HX711_raw,DS1307.hour,0);
			for(int i = 0; buffer_to_send[i] != 0; i++)
			{
				printf("%c",buffer_to_send[i]);
				HAL_Delay(10);
			}
		}
			
		HX711_Sum += ((HX711_GetValue()-8288900)/502)-75;
		if(HX711_count == 5)
		{
			HX711_raw = HX711_Sum/5;
			if(HX711_raw > 1000) HX711_raw = 0;
			HX711_Sum = 0;
			HX711_count = 0;
		}		
		HX711_count++;
		
		//HX711_raw = ((HX711_GetValue()-8288900)/502)-66;
		//HX711_raw = HX711_GetValue();
		
		HAL_Delay(100);
	}
}
/*--- page 3 ---*/
void page3(void)
{
	lcd_init();
	lcd_goto_XY(1,0);
	lcd_send_string("Duty:            ");
	lcd_goto_XY(2,0);
	lcd_send_string("                 ");

	switch (set_duty)
	{
		case D_1P:
		lcd_goto_XY(1,6);
		lcd_send_string("1 minute ");
		break;
		case D_15P:
		lcd_goto_XY(1,6);
		lcd_send_string("15 minute");
		break;
		case D_30P:
		lcd_goto_XY(1,6);
		lcd_send_string("30 minute ");
		break;
		case D_1H:
		lcd_goto_XY(1,6);
		lcd_send_string("1 hour    ");
		break;
	}
	
	while(1)
	{
		if(HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_5) == 0)
			{
			while(HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_5)== 0);
			if(set_duty == D_1H) set_duty = D_1P; else set_duty++;
		}

		switch (set_duty)
		{
				case D_1P:
				lcd_goto_XY(1,6);
				lcd_send_string("1 minute ");
				break;
				case D_15P:
				lcd_goto_XY(1,6);
				lcd_send_string("15 minute");
				break;
				case D_30P:
				lcd_goto_XY(1,6);
				lcd_send_string("30 minute ");
				break;
				case D_1H:
				lcd_goto_XY(1,6);
				lcd_send_string("1 hour    ");
				break;
		}
		
		lcd_goto_XY(2,4);
		lcd_send_data(DS1307.hour/10+48);
		lcd_send_data(DS1307.hour%10+48);
		lcd_send_data(':');
		lcd_send_data(DS1307.min/10+48);
		lcd_send_data(DS1307.min%10+48);
		lcd_send_data(':');
		lcd_send_data(DS1307.sec/10+48);
		lcd_send_data(DS1307.sec%10+48);
		
		
		if(HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_6)== 0)
		{
			while(HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_6)== 0);
			if(3 == page); else page++;
			switch (page)
				{
					case 1: 
					page1();
					break;
					case 2: 
					page2();
					break;
					case 3: 
					page3();
					default: break;
				}
		}
		if(HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_7)== 0)
		{
			while(HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_7)== 0);
			if(1 == page); else page--;
			switch (page)
				{
					case 1: 
					page1();
					break;
					case 2: 
					page2();
					break;
					case 3: 
					page3();
					default: break;
				}
		}
		
		if(HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_4) == 0)
			{
			while(HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_4)== 0);

			sprintf(buffer_to_send,"1,%d,%d,%d,%d,%d,%dW", temperature, HS1101, sun, HX711_raw,DS1307.hour,0);
			for(int i = 0; buffer_to_send[i] != 0; i++)
			{
				printf("%c",buffer_to_send[i]);
				HAL_Delay(10);
			}
		}
			
		HX711_Sum += ((HX711_GetValue()-8288900)/502)-75;
		if(HX711_count == 5)
		{
			HX711_raw = HX711_Sum/5;
			if(HX711_raw > 1000) HX711_raw = 0;
			HX711_Sum = 0;
			HX711_count = 0;
		}		
		HX711_count++;
		
		//HX711_raw = ((HX711_GetValue()-8288900)/502)-66;
		//HX711_raw = HX711_GetValue();
		
		HAL_Delay(100);
	}
}
/*-timer 2 interrupt-*/
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	if(htim->Instance == TIM2)
	{
		if(timer2 == 10 && send_done == false)
		{
			sprintf(buffer_to_send,"1,%d,%d,%d,%d,%d,%dW", temperature, HS1101, sun, HX711_raw,DS1307.hour,0);
			for(int i = 0; buffer_to_send[i] != 0; i++)
			{
				printf("%c",buffer_to_send[i]);
				HAL_Delay(10);
			}
			timer2 = 0;	
		}
		else if(send_done == true)
		{
			HAL_TIM_Base_Stop_IT(&htim2);
			timer2 = 0;	
			send_done = false;
		}
		else
		{
			timer2++;
		}
	}
}
/*--- HS1101 in ---*/
void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
{
	if(htim -> Instance == htim1.Instance)
	{

		//HS1101 = __HAL_TIM_GET_COMPARE(&htim1,TIM_CHANNEL_1)-100;
		
		if(cout_hs == 200)
		{
			HS1101_sum /=200; 
			HS1101 = 55+(6000-HS1101_sum)*0.0762 + 0.001*(temperature/10-28);
			if(HS1101 > 100) HS1101 =  re_HS1101; else re_HS1101 = HS1101;
			cout_hs = 0;
		}
		else
		{
			HS1101_sum += 1000000/(__HAL_TIM_GET_COMPARE(&htim1,TIM_CHANNEL_1));
			cout_hs++;
		}
		
		/*
		//HS1101 = (__HAL_TIM_GET_COMPARE(&htim1,TIM_CHANNEL_1));
		HS1101 = 1000000/(__HAL_TIM_GET_COMPARE(&htim1,TIM_CHANNEL_1));
		HS1101 = 59+(6660-HS1101)*0.0762 + 0.001*(temperature/10-28);
		if(HS1101 > 100) HS1101 =  re_HS1101;
	  else re_HS1101 = HS1101;
		*/
	}
		htim1.Instance -> CNT = 0;	
}
/*--- interrupt GPIO ---*/
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
		if(GPIO_Pin == GPIO_PIN_5)
		{
			RTC_GetTime();
			temperature = LM75_GetValue();
			sun = GY30_GetValue();
			if(set_duty == D_1P)
			{
				if(DS1307.sec == 0)
				{
					sprintf(buffer_to_send,"1,%d,%d,%d,%d,%d,%dW", temperature, HS1101, sun, HX711_raw,DS1307.hour,0);
					for(int i = 0; buffer_to_send[i] != 0; i++)
					{
						printf("%c",buffer_to_send[i]);
						HAL_Delay(10);
					}
					__HAL_TIM_SET_COUNTER(&htim2,0);
					HAL_TIM_Base_Start_IT(&htim2);
				}
			}
			else
			{
				/*--- 15P ---*/
				if(set_duty == D_15P)
				{
					if((DS1307.min == 0 || DS1307.min == 15 || DS1307.min == 30 || DS1307.min == 45) && DS1307.sec == 0)
					{
						sprintf(buffer_to_send,"1,%d,%d,%d,%d,%d,%dW", temperature, HS1101, sun, HX711_raw,DS1307.hour,0);
						for(int i = 0; buffer_to_send[i] != 0; i++)
						{
							printf("%c",buffer_to_send[i]);
							HAL_Delay(10);
						}
						__HAL_TIM_SET_COUNTER(&htim2,0);
						HAL_TIM_Base_Start_IT(&htim2);
					}
				}
				/*--- 30P ---*/
				if(set_duty == D_30P)
				{
					if((DS1307.min == 0 || DS1307.min == 30) && DS1307.sec == 0)
					{
						sprintf(buffer_to_send,"1,%d,%d,%d,%d,%d,%dW", temperature, HS1101, sun, HX711_raw,DS1307.hour,0);
						for(int i = 0; buffer_to_send[i] != 0; i++)
						{
							printf("%c",buffer_to_send[i]);
							HAL_Delay(10);
						}
						__HAL_TIM_SET_COUNTER(&htim2,0);
						HAL_TIM_Base_Start_IT(&htim2);
					}
				}
				/*--- 1H ---*/
				if(set_duty == D_1H)
				{
					if(DS1307.min == 0 && DS1307.sec == 0)
					{
						sprintf(buffer_to_send,"1,%d,%d,%d,%d,%d,%dW", temperature, HS1101, sun, HX711_raw,DS1307.hour,0);
						for(int i = 0; buffer_to_send[i] != 0; i++)
						{
							printf("%c",buffer_to_send[i]);
							HAL_Delay(10);
						}
						__HAL_TIM_SET_COUNTER(&htim2,0);
						HAL_TIM_Base_Start_IT(&htim2);
					}
				}
			}
			HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13);
		}				
}
/*--- BCB2DEC ---*/
int RTC_BCB2DEC(int BCD)
{
	int L, H;
	L=BCD & 0x0F; //ones
	H=(BCD>>4) * 10;//tens
	return (H+L);
}
/*--- DEC2BCB ---*/
int RTC_DEC2BCB(int DEC)
{
	int L, H;
	L=DEC%10; //make digit low
	H=DEC/10<<4; //make digit high
	return (H+L);
}
/*--- Write REG ---*/
void I2C_WriteBuffer(I2C_HandleTypeDef hi, uint8_t DEV_ADDR, uint8_t sizebuf)
{
	while(HAL_I2C_Master_Transmit(&hi, (uint16_t) DEV_ADDR, (uint8_t*) &I2C_Buffer, (uint16_t) sizebuf, (uint32_t)1000))
	{
		if(HAL_I2C_GetError(&hi) != HAL_I2C_ERROR_AF)
		{
			HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13);
		}
	}
}
/*--- Read REG ---*/
void I2C_ReadBuffer(I2C_HandleTypeDef hi, uint8_t DEV_ADDR, uint8_t sizebuf)
{
	while(HAL_I2C_Master_Receive(&hi, (uint16_t) DEV_ADDR, (uint8_t*) &I2C_Buffer, (uint16_t) sizebuf, (uint32_t)1000))
	{
		if(HAL_I2C_GetError(&hi) != HAL_I2C_ERROR_AF)
		{
			HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13);
		}
	}
}
/*--- DS1307 Get time ---*/
void RTC_GetTime(void)
{
	I2C_Buffer[0] = 0x00;
	I2C_WriteBuffer(hi2c1,(uint16_t)DS1307_ADRESS, 1);
	while(HAL_I2C_GetState(&hi2c1) != HAL_I2C_STATE_READY);
	I2C_ReadBuffer(hi2c1, (uint16_t)DS1307_ADRESS, 7);
	
	DS1307.sec = RTC_BCB2DEC(I2C_Buffer[0]);
	DS1307.min = RTC_BCB2DEC(I2C_Buffer[1]);
	DS1307.hour = RTC_BCB2DEC(I2C_Buffer[2]);
	DS1307.day = RTC_BCB2DEC(I2C_Buffer[3]);
	DS1307.date = RTC_BCB2DEC(I2C_Buffer[4]);
	DS1307.month = RTC_BCB2DEC(I2C_Buffer[5]);
	DS1307.year = RTC_BCB2DEC(I2C_Buffer[6]);
}
/*--- DS1307 Set time ---*/
void RTC_SetTime(uint8_t hour, uint8_t min, uint8_t sec, uint8_t date, uint8_t month, uint8_t year)
{
	I2C_Buffer[0] = 0x00;
	I2C_Buffer[1] = RTC_DEC2BCB(sec);
	I2C_Buffer[2] = RTC_DEC2BCB(min);
	I2C_Buffer[3] = RTC_DEC2BCB(hour);
	I2C_Buffer[5] = RTC_DEC2BCB(date);
	I2C_Buffer[6] = RTC_DEC2BCB(month);
	I2C_Buffer[7] = RTC_DEC2BCB(year);
	
	I2C_WriteBuffer(hi2c1,(uint16_t)DS1307_ADRESS, 8);
	HAL_Delay(100);
}
/*--- DS1307 SET IRQ ---*/
void RTC_SetIRQ(void)
{
	I2C_Buffer[0] = 0x07;
	I2C_Buffer[1] = 0x10;
	I2C_WriteBuffer(hi2c1,(uint16_t)DS1307_ADRESS, 2);
	while(HAL_I2C_GetState(&hi2c1) != HAL_I2C_STATE_READY);
}
/*--- LM75 get temperature ---*/
uint16_t LM75_GetValue(void)
{
	uint16_t temp;
	I2C_Buffer[0] = 0x00;
	I2C_WriteBuffer(hi2c1,(uint16_t)LM75_ADRESS, 1);
	while(HAL_I2C_GetState(&hi2c1) != HAL_I2C_STATE_READY);
	I2C_ReadBuffer(hi2c1, (uint16_t)LM75_ADRESS, 2);
	
	LM75.TempH = I2C_Buffer[0];
	LM75.TempL = I2C_Buffer[1];
	temp = ((LM75.TempH << 8 ) + LM75.TempL) >> 7;
	temp = ((temp & 0xFE) >> 1) * 10 + (temp & 0x01) * 5;
	return temp;
}
/*--- GY 30 get value ---*/
uint16_t GY30_GetValue(void)
{
	I2C_Buffer[0] = 0x10;
	I2C_WriteBuffer(hi2c1,(uint16_t)GY30_ADRESS, 1);
	while(HAL_I2C_GetState(&hi2c1) != HAL_I2C_STATE_READY);
	I2C_ReadBuffer(hi2c1, (uint16_t)GY30_ADRESS, 2);
	
	GY30.Conti_H_Mode1 = I2C_Buffer[0];
	GY30.Conti_H_Mode2 = I2C_Buffer[1];

	return ((GY30.Conti_H_Mode1<<8)+GY30.Conti_H_Mode2)/1.2;
}
/*--- Delay US ---*/
void delay_us (uint16_t us)
{
	HAL_TIM_Base_Start(&htim3);
	__HAL_TIM_SET_COUNTER(&htim3,0);  // set the counter value a 0
	while (__HAL_TIM_GET_COUNTER(&htim3) < us);  // wait for the counter to reach the us input in the parameter
	HAL_TIM_Base_Stop(&htim3);
}
/*--- HX711 Get Value ---*/
int32_t HX711_GetValue(void)
{
	uint32_t data = 0;
  uint32_t  startTime = HAL_GetTick();
  while(HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_8) == GPIO_PIN_SET)
  {
    if(HAL_GetTick() - startTime > 150)
      return 0;
  }
  for(int8_t i=0; i<24 ; i++)
  {
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_9, GPIO_PIN_SET);   
    delay_us(1);
    data = data << 1;    
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_9, GPIO_PIN_RESET);
    delay_us(1);
    if(HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_8) == GPIO_PIN_SET)
      data ++;
  }
  data = data ^ 0x800000; 
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_9, GPIO_PIN_SET);   
  delay_us(1);
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_9, GPIO_PIN_RESET);
  delay_us(1);
  return data;   
}
/**-------------Ham nhan bluetooth-----------------*/
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	uint8_t i;
	if(huart -> Instance == huart1.Instance)
	{
		if(Rx_Indx == 0) for(i=0;i<50;i++) Rx_Buffer[i]=0;
		if(Rx_Data[0] != 'W') 
		{
			Rx_Buffer[Rx_Indx++] = Rx_Data[0];
		}
    else
		{ 
			Rx_Indx = 0;
			/*--- code here ---*/
			if(Rx_Buffer[0] == '1')
			{
				if(Rx_Buffer[1] == 'D')
				{
					if(Rx_Buffer[2] == '0') set_duty = D_1P;
					else if(Rx_Buffer[2] == '1') set_duty = D_15P;
					else if(Rx_Buffer[2] == '2') set_duty = D_30P;
					else if(Rx_Buffer[2] == '3') set_duty = D_1H;
				}
				/*------*/
				if(Rx_Buffer[1] == 'T')
				{
					i = 0;
					char* token = strtok(Rx_Buffer, ",");
					while (token != NULL) 
					{
						token = strtok(NULL, ",");
						time[i] = atoi(token);
						i++;
					}
					RTC_SetTime(time[0],time[1],time[2],time[3],time[4],time[5]);
				}
				/*------*/
				if(Rx_Buffer[1] == 'O')
				{
					send_done = true;
				}
			}
			/*-----------------*/
		}
    HAL_UART_Receive_IT(&huart1, (uint8_t*) Rx_Data, 1);		
	}
}
/*-------------------------*/
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief I2C1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C1_Init(void)
{

  /* USER CODE BEGIN I2C1_Init 0 */

  /* USER CODE END I2C1_Init 0 */

  /* USER CODE BEGIN I2C1_Init 1 */

  /* USER CODE END I2C1_Init 1 */
  hi2c1.Instance = I2C1;
  hi2c1.Init.ClockSpeed = 100000;
  hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_2;
  hi2c1.Init.OwnAddress1 = 0;
  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c1.Init.OwnAddress2 = 0;
  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C1_Init 2 */

  /* USER CODE END I2C1_Init 2 */

}

/**
  * @brief I2C2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C2_Init(void)
{

  /* USER CODE BEGIN I2C2_Init 0 */

  /* USER CODE END I2C2_Init 0 */

  /* USER CODE BEGIN I2C2_Init 1 */

  /* USER CODE END I2C2_Init 1 */
  hi2c2.Instance = I2C2;
  hi2c2.Init.ClockSpeed = 100000;
  hi2c2.Init.DutyCycle = I2C_DUTYCYCLE_2;
  hi2c2.Init.OwnAddress1 = 0;
  hi2c2.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c2.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c2.Init.OwnAddress2 = 0;
  hi2c2.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c2.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C2_Init 2 */

  /* USER CODE END I2C2_Init 2 */

}

/**
  * @brief TIM1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM1_Init(void)
{

  /* USER CODE BEGIN TIM1_Init 0 */

  /* USER CODE END TIM1_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_IC_InitTypeDef sConfigIC = {0};

  /* USER CODE BEGIN TIM1_Init 1 */

  /* USER CODE END TIM1_Init 1 */
  htim1.Instance = TIM1;
  htim1.Init.Prescaler = 71;
  htim1.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim1.Init.Period = 65535;
  htim1.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim1.Init.RepetitionCounter = 0;
  htim1.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim1) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim1, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_IC_Init(&htim1) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim1, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigIC.ICPolarity = TIM_INPUTCHANNELPOLARITY_RISING;
  sConfigIC.ICSelection = TIM_ICSELECTION_DIRECTTI;
  sConfigIC.ICPrescaler = TIM_ICPSC_DIV1;
  sConfigIC.ICFilter = 0;
  if (HAL_TIM_IC_ConfigChannel(&htim1, &sConfigIC, TIM_CHANNEL_1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM1_Init 2 */

  /* USER CODE END TIM1_Init 2 */

}

/**
  * @brief TIM2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM2_Init(void)
{

  /* USER CODE BEGIN TIM2_Init 0 */

  /* USER CODE END TIM2_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_IC_InitTypeDef sConfigIC = {0};

  /* USER CODE BEGIN TIM2_Init 1 */

  /* USER CODE END TIM2_Init 1 */
  htim2.Instance = TIM2;
  htim2.Init.Prescaler = 7199;
  htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim2.Init.Period = 10000;
  htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim2) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim2, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_IC_Init(&htim2) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigIC.ICPolarity = TIM_INPUTCHANNELPOLARITY_RISING;
  sConfigIC.ICSelection = TIM_ICSELECTION_DIRECTTI;
  sConfigIC.ICPrescaler = TIM_ICPSC_DIV1;
  sConfigIC.ICFilter = 0;
  if (HAL_TIM_IC_ConfigChannel(&htim2, &sConfigIC, TIM_CHANNEL_3) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_IC_ConfigChannel(&htim2, &sConfigIC, TIM_CHANNEL_4) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM2_Init 2 */

  /* USER CODE END TIM2_Init 2 */

}

/**
  * @brief TIM3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM3_Init(void)
{

  /* USER CODE BEGIN TIM3_Init 0 */

  /* USER CODE END TIM3_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM3_Init 1 */

  /* USER CODE END TIM3_Init 1 */
  htim3.Instance = TIM3;
  htim3.Init.Prescaler = 71;
  htim3.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim3.Init.Period = 65535;
  htim3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim3.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim3) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim3, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim3, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM3_Init 2 */

  /* USER CODE END TIM3_Init 2 */

}

/**
  * @brief USART1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART1_UART_Init(void)
{

  /* USER CODE BEGIN USART1_Init 0 */

  /* USER CODE END USART1_Init 0 */

  /* USER CODE BEGIN USART1_Init 1 */

  /* USER CODE END USART1_Init 1 */
  huart1.Instance = USART1;
  huart1.Init.BaudRate = 9600;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART1_Init 2 */

  /* USER CODE END USART1_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, BUZZ_Pin|LED3_Pin|LED2_Pin|LED1_Pin, GPIO_PIN_SET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, M1_Pin|M0_Pin, GPIO_PIN_SET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(CLK_GPIO_Port, CLK_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : PC13 */
  GPIO_InitStruct.Pin = GPIO_PIN_13;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pins : PA4 PA5 PA6 PA7 */
  GPIO_InitStruct.Pin = GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : BUZZ_Pin LED3_Pin LED2_Pin LED1_Pin 
                           CLK_Pin */
  GPIO_InitStruct.Pin = BUZZ_Pin|LED3_Pin|LED2_Pin|LED1_Pin 
                          |CLK_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pins : M1_Pin M0_Pin */
  GPIO_InitStruct.Pin = M1_Pin|M0_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pin : SQ_Pin */
  GPIO_InitStruct.Pin = SQ_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(SQ_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : DATA_Pin */
  GPIO_InitStruct.Pin = DATA_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(DATA_GPIO_Port, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(EXTI9_5_IRQn, 4, 0);
  HAL_NVIC_EnableIRQ(EXTI9_5_IRQn);

}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */

  /* USER CODE END Error_Handler_Debug */
}
#ifdef __GNUC__
	#define PUTCHAR_PROTOTYPE int __io_purchar(int ch)
#else
	#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif	
	
PUTCHAR_PROTOTYPE
{
	HAL_UART_Transmit(&huart1, (uint8_t *)&ch, 1, 0xFFFF);
	return ch;
}
#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{ 
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
