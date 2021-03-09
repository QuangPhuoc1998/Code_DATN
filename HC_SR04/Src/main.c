#include "main.h"
#include "stdio.h"
#include "stdbool.h"
#include <string.h>
#include <stdlib.h>

#define D_1P 0
#define D_15P 1
#define D_30P 2
#define D_1H 3
#define DS1307_ADRESS (0x68<<1)

volatile uint32_t local_time, sensor_time;
volatile uint32_t distance;
volatile uint8_t time[6];
volatile uint8_t re_min = 0;
volatile uint8_t re_hour = 0;

I2C_HandleTypeDef hi2c1;
TIM_HandleTypeDef htim1;
TIM_HandleTypeDef htim2;
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
/*---*/
volatile DS1307_t DS1307;

UART_HandleTypeDef huart1;

/*--- global variable ---*/
char Rx_Data[2], Rx_Buffer[50];
uint8_t Rx_Indx = 0;
char buffer_to_send[64];
uint8_t I2C_Buffer[8];
volatile uint8_t set_duty = D_1P;

void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_TIM1_Init(void);
static void MX_TIM2_Init(void);
static void MX_I2C1_Init(void);
static void MX_USART1_UART_Init(void);

void delay_us (uint16_t us);
uint32_t hcsr04_read (void);
void RTC_GetTime(void);
void RTC_SetTime(uint8_t hour, uint8_t min, uint8_t sec, uint8_t date, uint8_t month, uint8_t year);
void RTC_SetIRQ(void);
int main(void)
{
  HAL_Init();
  SystemClock_Config();
  MX_GPIO_Init();
  MX_TIM1_Init();
  MX_TIM2_Init();
	MX_I2C1_Init();
	MX_USART1_UART_Init();
	
	//RTC_SetIRQ();
	//HAL_TIM_Base_Start(&htim2);
	
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_11|GPIO_PIN_12, GPIO_PIN_RESET);
	HAL_UART_Receive_IT(&huart1, (uint8_t*) Rx_Data, 1);
	
  while (1)
  {
		 RTC_GetTime();
		 //sensor_time = hcsr04_read();
		 //distance = sensor_time * .034/2;
		 if(HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_4) == 0)
			{
				while(HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_4)== 0);
				HAL_GPIO_WritePin(GPIOC, GPIO_PIN_14,GPIO_PIN_RESET);
				
				sprintf(buffer_to_send,"2,%d,%d,0W",distance,DS1307.hour);
				for(int i = 0; buffer_to_send[i] != 0; i++)
				{
					printf("%c",buffer_to_send[i]);
					HAL_Delay(10);
				}
				
				HAL_GPIO_WritePin(GPIOC, GPIO_PIN_14,GPIO_PIN_SET);
			}
			if(HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_5)== 0)
			{
				while(HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_5)== 0);
				HAL_GPIO_WritePin(GPIOC, GPIO_PIN_15,GPIO_PIN_RESET);
				
				sprintf(buffer_to_send,"2,%d,%d,0W",distance,DS1307.hour);
				for(int i = 0; buffer_to_send[i] != 0; i++)
				{
					printf("%c",buffer_to_send[i]);
					HAL_Delay(10);
				}
				
				HAL_GPIO_WritePin(GPIOC, GPIO_PIN_15,GPIO_PIN_SET);
			}
			
			if(set_duty == D_1P)
			{
				if(DS1307.sec == 0 && DS1307.min != re_min)
				{
					sprintf(buffer_to_send,"2,%d,%d,0W",distance,DS1307.hour);
					for(int i = 0; buffer_to_send[i] != 0; i++)
					{
						printf("%c",buffer_to_send[i]);
						HAL_Delay(10);
					}
					re_min = DS1307.min;
				}
			}
			else
			{
				/*--- 15P ---*/
				if(set_duty == D_15P)
				{
					if((DS1307.min == 0 || DS1307.min == 15 || DS1307.min == 30 || DS1307.min == 45) && DS1307.sec == 0 && DS1307.min != re_min)
					{
						sprintf(buffer_to_send,"2,%d,%d,0W",distance,DS1307.hour);
						for(int i = 0; buffer_to_send[i] != 0; i++)
						{
							printf("%c",buffer_to_send[i]);
							HAL_Delay(10);
						}
						re_min = DS1307.min;
					}
				}
				/*--- 30P ---*/
				if(set_duty == D_30P)
				{
					if((DS1307.min == 0 || DS1307.min == 30) && DS1307.sec == 0 && DS1307.min != re_min)
					{
						sprintf(buffer_to_send,"2,%d,%d,0W",distance,DS1307.hour);
						for(int i = 0; buffer_to_send[i] != 0; i++)
						{
							printf("%c",buffer_to_send[i]);
							HAL_Delay(10);
						}
						re_min = DS1307.min;
					}
				}
				/*--- 1H ---*/
				if(set_duty == D_1H)
				{
					if(DS1307.min == 0 && DS1307.sec == 0 && DS1307.hour != re_hour)
					{
						sprintf(buffer_to_send,"2,%d,%d,0W",distance,DS1307.hour);
						for(int i = 0; buffer_to_send[i] != 0; i++)
						{
							printf("%c",buffer_to_send[i]);
							HAL_Delay(10);
						}
						re_hour = DS1307.hour;
					}
				}
			}
			HAL_Delay(200);
  }
}
void delay_us (uint16_t us)
{
	__HAL_TIM_SET_COUNTER(&htim2,0);  // set the counter value a 0
	while (__HAL_TIM_GET_COUNTER(&htim2) < us);  // wait for the counter to reach the us input in the parameter
}
uint32_t hcsr04_read (void)
{
 local_time=0;
 HAL_GPIO_WritePin(GPIOA, GPIO_PIN_0, GPIO_PIN_RESET);  // pull the TRIG pin HIGH
 delay_us(2);  // wait for 2 us


 HAL_GPIO_WritePin(GPIOB, GPIO_PIN_3, GPIO_PIN_SET);  // pull the TRIG pin HIGH
 delay_us(10);  // wait for 10 us
 HAL_GPIO_WritePin(GPIOB, GPIO_PIN_3, GPIO_PIN_RESET);  // pull the TRIG pin low

 // read the time for which the pin is high

 while (!(HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_4)));  // wait for the ECHO pin to go high
 while (HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_4))    // while the pin is high
  {
  local_time++;   // measure time for which the pin is high
  delay_us (1);
  }
 return local_time;
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
			HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_14);
			/*--- code here ---*/
			if(Rx_Buffer[0] == '2')
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
			}
			/*-----------------*/
		}
    HAL_UART_Receive_IT(&huart1, (uint8_t*) Rx_Data, 1);		
	}
}
/*------*/
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
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim1, &sMasterConfig) != HAL_OK)
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

  /* USER CODE BEGIN TIM2_Init 1 */

  /* USER CODE END TIM2_Init 1 */
  htim2.Instance = TIM2;
  htim2.Init.Prescaler = 71;
  htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim2.Init.Period = 65535;
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
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM2_Init 2 */

  /* USER CODE END TIM2_Init 2 */

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
  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_14|GPIO_PIN_15, GPIO_PIN_SET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(trig_GPIO_Port, trig_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_11|GPIO_PIN_12, GPIO_PIN_SET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(trig__GPIO_Port, trig__Pin, GPIO_PIN_RESET);

  /*Configure GPIO pins : PC13 PC14 PC15 */
  GPIO_InitStruct.Pin = GPIO_PIN_13|GPIO_PIN_14|GPIO_PIN_15;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pins : trig_Pin PA11 */
  GPIO_InitStruct.Pin = trig_Pin|GPIO_PIN_11;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : echo_Pin PA4 PA5 */
  GPIO_InitStruct.Pin = echo_Pin|GPIO_PIN_4|GPIO_PIN_5;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pin : PA12 */
  GPIO_InitStruct.Pin = GPIO_PIN_12;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pin : trig__Pin */
  GPIO_InitStruct.Pin = trig__Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(trig__GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : echo__Pin */
  GPIO_InitStruct.Pin = echo__Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(echo__GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : PB5 */
  GPIO_InitStruct.Pin = GPIO_PIN_5;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(EXTI9_5_IRQn, 0, 0);
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
