/*
 * DS1307.c
 *
 * Created: 5/30/2019 9:06:01 AM
 * Author : Admin
 */ 
#define D4 eS_PORTD2
#define D5 eS_PORTD3
#define D6 eS_PORTD4
#define D7 eS_PORTD5
#define RS eS_PORTD0
#define EN eS_PORTD1

#include "main.h"
#include "I2C.h"
#include "lcd.h"

/*---*/
uint8_t i;
/*---*/
int DECIMALtoBCD(int DEC)
{
	int L, H;
	L=DEC%10; //make digit low
	H=DEC/10<<4; //make digit high
	return (H+L);
}
int BCDtoDECIMAL(int BCD)
{
	int L, H;
	L=BCD & 0x0F; //ones
	H=(BCD>>4) * 10;//tens
	return (H+L);
}
int main(void)
{
	uint8_t ucTemp;
	/*---port---*/
	DDRD = 0xFF;
	DDRA = 0x00;
	/*--- Init ---*/
	Lcd4_Init();
	I2C_Init();
	/*--- Begin ---*/
	Lcd4_Write_String("Temp: ");
	
	I2C_Start_Wait(0x90);

	I2C_Stop();
	
	I2C_Start_Wait(0x90);
	I2C_Write(0x00);
	I2C_Repeated_Start(0x91);
	ucTemp = I2C_Read_Ack();
	I2C_Stop();

	
    while (1) 
    {
		Lcd4_Set_Cursor(1, 6);
		Lcd4_Write_Char(ucTemp%10+48);
		_delay_ms(1000);
    }
}

