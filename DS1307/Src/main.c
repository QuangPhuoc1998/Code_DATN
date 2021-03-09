#include "main.h"

I2C_HandleTypeDef hi2c1;
I2C_HandleTypeDef hi2c2;

#define DS1307_ADRESS (0x68<<1)
#define DS1307_REG_TIME 0x00

typedef struct
{
	uint8_t sec;
	uint8_t min;
	uint8_t hour;
	uint8_t day;
	uint8_t date;
	uint8_t month;
	uint8_t year;
	
	uint8_t I2C_Buffer[8];
}DS1307_t;
DS1307_t DS1307;

void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_I2C1_Init(void);
static void MX_I2C2_Init(void);

int RTC_BCB2DEC(int c);
int RTC_DEC2BCB(int c);
void I2C_WriteBuffer(I2C_HandleTypeDef hi, uint8_t DEV_ADDR, uint8_t sizebuf);
void I2C_ReadBuffer(I2C_HandleTypeDef hi, uint8_t DEV_ADDR, uint8_t sizebuf); 
void RTC_GetTime(void);
void RTC_SetTime(uint8_t hour, uint8_t min, uint8_t sec, uint8_t date, uint8_t month, uint8_t year);
int main(void)
{
  HAL_Init();
  SystemClock_Config();

  MX_GPIO_Init();
  MX_I2C1_Init();
  MX_I2C2_Init();
  while (1)
  {
		HAL_Delay(1000);
		RTC_GetTime();
		HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13);
  }
}

int RTC_BCB2DEC(int BCD)
{
	int L, H;
	L=BCD & 0x0F; //ones
	H=(BCD>>4) * 10;//tens
	return (H+L);
}
int RTC_DEC2BCB(int DEC)
{
	int L, H;
	L=DEC%10; //make digit low
	H=DEC/10<<4; //make digit high
	return (H+L);
}
void I2C_WriteBuffer(I2C_HandleTypeDef hi, uint8_t DEV_ADDR, uint8_t sizebuf)
{
	while(HAL_I2C_Master_Transmit(&hi, (uint16_t) DEV_ADDR, (uint8_t*) &DS1307.I2C_Buffer, (uint16_t) sizebuf, (uint32_t)1000))
	{
		if(HAL_I2C_GetError(&hi) != HAL_I2C_ERROR_AF)
		{
			HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13);
		}
	}
}
void I2C_ReadBuffer(I2C_HandleTypeDef hi, uint8_t DEV_ADDR, uint8_t sizebuf)
{
	while(HAL_I2C_Master_Receive(&hi, (uint16_t) DEV_ADDR, (uint8_t*) &DS1307.I2C_Buffer, (uint16_t) sizebuf, (uint32_t)1000))
	{
		if(HAL_I2C_GetError(&hi) != HAL_I2C_ERROR_AF)
		{
			HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13);
		}
	}
}
void RTC_GetTime(void)
{
	DS1307.I2C_Buffer[0] = 0x00;
	I2C_WriteBuffer(hi2c1,(uint16_t)DS1307_ADRESS, 1);
	while(HAL_I2C_GetState(&hi2c1) != HAL_I2C_STATE_READY);
	I2C_ReadBuffer(hi2c1, (uint16_t)DS1307_ADRESS, 7);
	
	DS1307.sec = RTC_BCB2DEC(DS1307.I2C_Buffer[0]);
	DS1307.min = RTC_BCB2DEC(DS1307.I2C_Buffer[1]);
	DS1307.hour = RTC_BCB2DEC(DS1307.I2C_Buffer[2]);
	DS1307.day = RTC_BCB2DEC(DS1307.I2C_Buffer[3]);
	DS1307.date = RTC_BCB2DEC(DS1307.I2C_Buffer[4]);
	DS1307.month = RTC_BCB2DEC(DS1307.I2C_Buffer[5]);
	DS1307.year = RTC_BCB2DEC(DS1307.I2C_Buffer[6]);
}
void RTC_SetTime(uint8_t hour, uint8_t min, uint8_t sec, uint8_t date, uint8_t month, uint8_t year)
{
	DS1307.I2C_Buffer[0] = 0x00;
	DS1307.I2C_Buffer[1] = RTC_DEC2BCB(sec);
	DS1307.I2C_Buffer[2] = RTC_DEC2BCB(min);
	DS1307.I2C_Buffer[3] = RTC_DEC2BCB(hour);
	DS1307.I2C_Buffer[5] = RTC_DEC2BCB(date);
	DS1307.I2C_Buffer[6] = RTC_DEC2BCB(month);
	DS1307.I2C_Buffer[7] = RTC_DEC2BCB(year);
	
	I2C_WriteBuffer(hi2c1,(uint16_t)DS1307_ADRESS, 8);
	HAL_Delay(100);
}
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
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_SET);

  /*Configure GPIO pin : PC13 */
  GPIO_InitStruct.Pin = GPIO_PIN_13;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

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
