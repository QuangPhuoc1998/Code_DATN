#ifndef _I2C_H_
#define _I2C_H_

void I2C_Init();								/* I2C initialize function */
uint8_t  I2C_Start(char write_address);			/* I2C start function */
uint8_t  I2C_Repeated_Start(char read_address);	/* I2C repeated start function */
void I2C_Stop();								/* I2C stop function */
void I2C_Start_Wait(char write_address);		/* I2C start wait function */
uint8_t  I2C_Write(char data);					/* I2C write function */
char I2C_Read_Ack();							/* I2C read ack function */
char I2C_Read_Nack();

void I2C_Slave_Init(uint8_t slave_address);		/* I2C slave initialize function with Slave address */
int8_t I2C_Slave_Listen();						/* I2C slave listen function */
int8_t I2C_Slave_Transmit(char data);			/* I2C slave transmit function */
char I2C_Slave_Receive();

#endif