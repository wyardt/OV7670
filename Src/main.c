/**
  ******************************************************************************
  * @file    Templates/Src/main.c 
  * @author  MCD Application Team
  * @version V1.0.2
  * @date    17-February-2017
  * @brief   Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT(c) 2017 STMicroelectronics</center></h2>
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of STMicroelectronics nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "stm32412g_discovery.h"
#include "ov7670.h"

/** @addtogroup STM32F4xx_HAL_Examples
  * @{
  */

/** @addtogroup Templates
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
static void SystemClock_Config(void);
static void Error_Handler(void);

I2C_HandleTypeDef heval_I2c;
  
HAL_StatusTypeDef i2c_stat;
unsigned char regReadback[2];
uint16_t pic[240][320];
uint16_t i,j;
uint16_t pic_data;

GPIO_InitTypeDef gpio_camera;

GPIO_PinState Vsync;
uint16_t color_bar = 0;
const uint8_t ov7670_init_reg_tbl[][2]= 
{   
	/*ÒÔÏÂÎªOV7670 QVGA RGB565²ÎÊý  */
	{0x3a, 0x04},//dummy
	{0x40, 0xd0},//565   
	{0x12, 0x14},//QVGA,RGBÊä³ö

	//Êä³ö´°¿ÚÉèÖÃ
	{0x32, 0x80},//HREF control	bit[2:0] HREF start 3 LSB	 bit[5:3] HSTOP HREF end 3LSB
	{0x17, 0x16},//HSTART start high 8-bit MSB         
	{0x18, 0x04},//5 HSTOP end high 8-bit
	{0x19, 0x02},
	{0x1a, 0x7b},//0x7a,
	{0x03, 0x06},//0x0a,Ö¡ÊúÖ±·½Ïò¿ØÖÆ

	{0x0c, 0x00},
	{0x15, 0x00},//0x00
	{0x3e, 0x00},//10
	{0x70, 0x3a},
	{0x71, 0x35},//0x35
	{0x72, 0x11},
	{0x73, 0x00},//

	{0xa2, 0x02},//15
	{0x11, 0x81},//Ê±ÖÓ·ÖÆµÉèÖÃ,0,²»·ÖÆµ.
	{0x7a, 0x20},
	{0x7b, 0x1c},
	{0x7c, 0x28},

	{0x7d, 0x3c},//20
	{0x7e, 0x55},
	{0x7f, 0x68},
	{0x80, 0x76},
	{0x81, 0x80},

	{0x82, 0x88},
	{0x83, 0x8f},
	{0x84, 0x96},
	{0x85, 0xa3},
	{0x86, 0xaf},

	{0x87, 0xc4},//30
	{0x88, 0xd7},
	{0x89, 0xe8},
	{0x13, 0xe0},
	{0x00, 0x00},//AGC

	{0x10, 0x00},
	{0x0d, 0x00},//È«´°¿Ú£¬ Î»[5:4]: 01 °ë´°¿Ú£¬10 1/4´°¿Ú£¬11 1/4´°¿Ú 
	{0x14, 0x28},//0x38, limit the max gain
	{0xa5, 0x05},
	{0xab, 0x07},

	{0x24, 0x75},//40
	{0x25, 0x63},
	{0x26, 0xA5},
	{0x9f, 0x78},
	{0xa0, 0x68},

	{0xa1, 0x03},//0x0b,
	{0xa6, 0xdf},//0xd8,
	{0xa7, 0xdf},//0xd8,
	{0xa8, 0xf0},
	{0xa9, 0x90},

	{0xaa, 0x94},//50
	{0x13, 0xe5},
	{0x0e, 0x61},
	{0x0f, 0x4b},
	{0x16, 0x02},

	{0x1e, 0x27},//Í¼ÏñÊä³ö¾µÏñ¿ØÖÆ.0x07
	{0x21, 0x02},
	{0x22, 0x91},
	{0x29, 0x07},
	{0x33, 0x0b},

	{0x35, 0x0b},//60
	{0x37, 0x1d},
	{0x38, 0x71},
	{0x39, 0x2a},
	{0x3c, 0x78},

	{0x4d, 0x40},
	{0x4e, 0x20},
	{0x69, 0x00},
	{0x6b, 0x40},//PLL*4=48Mhz
	{0x74, 0x19},
	{0x8d, 0x4f},

	{0x8e, 0x00},//70
	{0x8f, 0x00},
	{0x90, 0x00},
	{0x91, 0x00},
	{0x92, 0x00},//0x19,//0x66

	{0x96, 0x00},
	{0x9a, 0x80},
	{0xb0, 0x84},
	{0xb1, 0x0c},
	{0xb2, 0x0e},

	{0xb3, 0x82},//80
	{0xb8, 0x0a},
	{0x43, 0x14},
	{0x44, 0xf0},
	{0x45, 0x34},

	{0x46, 0x58},
	{0x47, 0x28},
	{0x48, 0x3a},
	{0x59, 0x88},
	{0x5a, 0x88},

	{0x5b, 0x44},//90
	{0x5c, 0x67},
	{0x5d, 0x49},
	{0x5e, 0x0e},
	{0x64, 0x04},
	{0x65, 0x20},

	{0x66, 0x05},
	{0x94, 0x04},
	{0x95, 0x08},
	{0x6c, 0x0a},
	{0x6d, 0x55},


	{0x4f, 0x80},
	{0x50, 0x80},
	{0x51, 0x00},
	{0x52, 0x22},
	{0x53, 0x5e},
	{0x54, 0x80},

	//{0x54, 0x40},//110


	{0x09, 0x03},//Çý¶¯ÄÜÁ¦×î´ó

	{0x6e, 0x11},//100
	{0x6f, 0x9f},//0x9e for advance AWB
	{0x55, 0x00},//ÁÁ¶È
	{0x56, 0x40},//¶Ô±È¶È 0x40
	{0x57, 0x40},//0x40,  change according to Jim's request
///////////////////////////////////////////////////////////////////////
//ÒÔÏÂ²¿·Ö´úÂëÓÉ¿ªÔ´µç×ÓÍøÍøÓÑ:duanzhang512 Ìá³ö
//Ìí¼Ó´Ë²¿·Ö´úÂë½«¿ÉÒÔ»ñµÃ¸üºÃµÄ³ÉÏñÐ§¹û,µ«ÊÇ×îÏÂÃæÒ»ÐÐ»áÓÐÀ¶É«µÄ¶¶¶¯.
//Èç²»ÏëÒª,¿ÉÒÔÆÁ±Î´Ë²¿·Ö´úÂë.È»ºó½«:OV7670_Window_Set(12,176,240,320);
//¸ÄÎª:OV7670_Window_Set(12,174,240,320);,¼´¿ÉÈ¥µô×îÏÂÒ»ÐÐµÄÀ¶É«¶¶¶¯
	{0x6a, 0x40},
	{0x01, 0x40},
	{0x02, 0x40},
	{0x13, 0xe7},
	{0x15, 0x00},  
	
		
	{0x58, 0x9e},
	
	{0x41, 0x08},
	{0x3f, 0x00},
	{0x75, 0x05},
	{0x76, 0xe1},
	{0x4c, 0x00},
	{0x77, 0x01},
	{0x3d, 0xc2},	
	{0x4b, 0x09},
	{0xc9, 0x60},
	{0x41, 0x38},
	
	{0x34, 0x11},
	{0x3b, 0x02}, 

	{0xa4, 0x89},
	{0x96, 0x00},
	{0x97, 0x30},
	{0x98, 0x20},
	{0x99, 0x30},
	{0x9a, 0x84},
	{0x9b, 0x29},
	{0x9c, 0x03},
	{0x9d, 0x4c},
	{0x9e, 0x3f},
	{0x78, 0x04},
	
	{0x79, 0x01},
	{0xc8, 0xf0},
	{0x79, 0x0f},
	{0xc8, 0x00},
	{0x79, 0x10},
	{0xc8, 0x7e},
	{0x79, 0x0a},
	{0xc8, 0x80},
	{0x79, 0x0b},
	{0xc8, 0x01},
	{0x79, 0x0c},
	{0xc8, 0x0f},
	{0x79, 0x0d},
	{0xc8, 0x20},
	{0x79, 0x09},
	{0xc8, 0x80},
	{0x79, 0x02},
	{0xc8, 0xc0},
	{0x79, 0x03},
	{0xc8, 0x40},
	{0x79, 0x05},
	{0xc8, 0x30},
	{0x79, 0x26}, 
	{0x09, 0x00},
///////////////////////////////////////////////////////////////////////
	
}; 


/********************************* LINK CAMERA ********************************/


/* Private functions ---------------------------------------------------------*/

void RCK(void)
{
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_5, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_5, GPIO_PIN_SET);
}

void WRST(void)
{
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_11, GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_11, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_11, GPIO_PIN_SET);
}

void RRST(void)
{
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_13, GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_13, GPIO_PIN_RESET);
	RCK();
  RCK();
  RCK();
  RCK();
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_13, GPIO_PIN_SET);
}

void WEN(void)
{
	HAL_GPIO_WritePin(GPIOG, GPIO_PIN_10, GPIO_PIN_SET);
}

void WDISEN(void)
{
	HAL_GPIO_WritePin(GPIOG, GPIO_PIN_10, GPIO_PIN_RESET);
}

void Init_OV7670(void)
{
	uint16_t i;
	
	for(i=0;i<sizeof(ov7670_init_reg_tbl)/sizeof(ov7670_init_reg_tbl[0]);i++)
	{
	 	wrOV7670Reg(ov7670_init_reg_tbl[i][0],ov7670_init_reg_tbl[i][1]);
	}
}

static void EXTI0_IRQHandler_Config(void)
{
  GPIO_InitTypeDef   GPIO_InitStructure;

  /* Enable GPIOA clock */
  __HAL_RCC_GPIOA_CLK_ENABLE();

  /* Configure PA.00 pin as input floating */
  GPIO_InitStructure.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStructure.Pull = GPIO_NOPULL;
  GPIO_InitStructure.Pin = GPIO_PIN_7;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStructure);

  /* Enable and set EXTI line 0 Interrupt to the lowest priority */
  HAL_NVIC_SetPriority(EXTI9_5_IRQn, 2, 0);
  HAL_NVIC_EnableIRQ(EXTI9_5_IRQn);
}

uint16_t state = 0;
/**
  * @brief EXTI line detection callbacks
  * @param GPIO_Pin: Specifies the pins connected EXTI line
  * @retval None
  */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
  if (GPIO_Pin == GPIO_PIN_7)
  {
  	if (state == 0)
  	{
    	WRST();
  		WEN();
  		BSP_LED_Toggle(LED1);
  		state = 1;
  	}
		else if (state == 1)
		{
			state = 2;
	  	WDISEN();
	  	RRST();
	  	
			for (i = 0; i < 240; i++)
			{
				for (j = 0; j < 320; j++)
				{
					RCK();
					pic[i][j] = (uint16_t)((((uint16_t)(GPIOC->IDR))&0x00ff)<<8);//HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_All);
					//pic_data = (GPIOC->IDR)&0xff;//HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_All);
					RCK();
					pic[i][j] = pic[i][j] | (uint16_t)((uint16_t)(GPIOC->IDR)&0x00ff);//HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_All);
					//pic_data = (pic_data << 8) | (GPIOC->IDR)&0xff;//HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_All);
					//ST7789H2_WritePixel(i, j, pic_data);
				}
			}
  	}
  }
}


/**
  * @brief  Main program
  * @param  None
  * @retval None
  */
int main(void)
{
  static unsigned int temp7670 = 0;
  uint8_t ID_reg[] = {0x0a,0xff,0xff,0xff};
  uint8_t recv[5] = {0, 0, 0, 0, 0};
  uint16_t value,val,val1,val2;

  /* STM32F4xx HAL library initialization:
       - Configure the Flash prefetch, Flash preread and Buffer caches
       - Systick timer is configured by default as source of time base, but user 
             can eventually implement his proper time base source (a general purpose 
             timer for example or other time source), keeping in mind that Time base 
             duration should be kept 1ms since PPP_TIMEOUT_VALUEs are defined and 
             handled in milliseconds basis.
       - Low Level Initialization
     */
  HAL_Init();

  /* Configure the System clock to have a frequency of 100 MHz */
  SystemClock_Config();
  //CLK_init_ON();
  HAL_RCC_MCOConfig(RCC_MCO1, RCC_MCO1SOURCE_PLLCLK, RCC_MCODIV_5);
  BSP_PB_Init(BUTTON_WAKEUP, BUTTON_MODE_GPIO);
  
  
	heval_I2c.Instance = I2C1;
	heval_I2c.Init.ClockSpeed      = 100000;
	heval_I2c.Init.DutyCycle       = I2C_DUTYCYCLE_2;
	heval_I2c.Init.OwnAddress1     = 0;
	heval_I2c.Init.AddressingMode  = I2C_ADDRESSINGMODE_7BIT;
	heval_I2c.Init.DualAddressMode = I2C_DUALADDRESS_DISABLED;
	heval_I2c.Init.OwnAddress2     = 0;
	heval_I2c.Init.GeneralCallMode = I2C_GENERALCALL_DISABLED;
	heval_I2c.Init.NoStretchMode   = I2C_NOSTRETCH_DISABLED;  
 
	/* Init the I2C */
	i2c_stat = HAL_I2C_Init(&heval_I2c);    
  //CAMERA_IO_Init();
  //CAMERA_IO_Write(0x42, ID_reg, 1);
  //err_code = HAL_I2C_Master_Transmit(&heval_I2c, 0x42>>1, ID_reg, 1, 100);
  //__HAL_RCC_I2C1_FORCE_RESET();
  //__HAL_RCC_I2C1_RELEASE_RESET();
  //I2Cx_MspInit();
  //I2Cx_Init();
  //err_code = HAL_I2C_Master_Receive(&heval_I2c, 0x43>>1, ID_reg, 1, 100);
  //HAL_I2C_Mem_Write(&heval_I2c, 0x42, (uint16_t)ID_reg, 1, &ID_reg, 1, 100);
  
  //temp7670 = CAMERA_IO_Read(0x42, 0x0a);
  //while(1!=OV7670_init())
  //	;
  BSP_LED_Init(LED1);
  BSP_LED_Init(LED2);
  BSP_LED_Init(LED3);
  BSP_LED_Off(LED1);
  BSP_LED_Off(LED2);
  BSP_LED_Off(LED3);

  BSP_LCD_Init();
	BSP_LCD_Clear(0);
#if 0
	for (i = 0; i < 320; i++)
	{
		for (j = 0; j < 480; j++)
		{
			pic[i][j] = 0xf;
		}
	}
#endif
#if 1
	//__HAL_RCC_GPIOA_CLK_ENABLE();
	__HAL_RCC_GPIOB_CLK_ENABLE();
	__HAL_RCC_GPIOG_CLK_ENABLE();
	__HAL_RCC_GPIOC_CLK_ENABLE();
	
	gpio_camera.Pin = GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3|GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7;
	gpio_camera.Pull = GPIO_NOPULL;
	gpio_camera.Mode = GPIO_MODE_INPUT;
	gpio_camera.Speed = GPIO_SPEED_HIGH;
	HAL_GPIO_Init(GPIOC, &gpio_camera);	//pic data input

	//gpio_camera.Pin = GPIO_PIN_7;
	//HAL_GPIO_Init(GPIOB, &gpio_camera); //control signal input
	EXTI0_IRQHandler_Config();
	
	gpio_camera.Pull = GPIO_PULLUP;
	gpio_camera.Pin = GPIO_PIN_11|GPIO_PIN_12|GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_3;
	gpio_camera.Mode = GPIO_MODE_OUTPUT_PP;
	HAL_GPIO_Init(GPIOB, &gpio_camera);	//control signal output

	gpio_camera.Pin = GPIO_PIN_13;
	HAL_GPIO_Init(GPIOA, &gpio_camera);
	
	gpio_camera.Pull = GPIO_NOPULL;
	gpio_camera.Pin = GPIO_PIN_10;
	HAL_GPIO_Init(GPIOG, &gpio_camera);	//control signal output
	
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_3, GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_3, GPIO_PIN_RESET);
	HAL_Delay(1);
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_3, GPIO_PIN_SET);
#endif
	
	HAL_I2C_Master_Transmit(&heval_I2c, 0x42, ID_reg, 1, 100);
	HAL_I2C_Master_Transmit(&heval_I2c, 0x42, ID_reg, 2, 100);
#if 1
	wrOV7670Reg(0x12, 0x80);
	HAL_Delay(10);
	Init_OV7670();
	OV7670_config_window(184,12,320,240);// set 240*320
#else	
	OV7670_init();
	//Cmos7670_init();
	OV7670_config_window(184,12,320,240);// set 240*320
	if (1 != OV7670_init())
	{
		BSP_LED_On(LED1);
		while (1)
		;
	}
#endif
	while (1)
	{
		if (state == 2)
		{
			BSP_LCD_DrawRGBImage(0, 0, 320, 240, pic);
			state = 0;
		}
		if (BSP_PB_GetState(BUTTON_WAKEUP) == GPIO_PIN_SET)
		{
			if (color_bar == 0)
			{
				wrOV7670Reg(0x71, 0x80);
				color_bar = 1;
			}
			else
			{
				color_bar = 0;
				wrOV7670Reg(0x71, 0x35);
			}
		}
	}
}

/**
  * @brief  System Clock Configuration
  *         The system Clock is configured as follow : 
  *            System Clock source            = PLL (HSE)
  *            SYSCLK(Hz)                     = 100000000
  *            HCLK(Hz)                       = 100000000
  *            AHB Prescaler                  = 1
  *            APB1 Prescaler                 = 2
  *            APB2 Prescaler                 = 1
  *            HSE Frequency(Hz)              = 8000000
  *            PLL_M                          = 8
  *            PLL_N                          = 200
  *            PLL_P                          = 2
  *            PLL_Q                          = 7
  *            PLL_R                          = 2
  *            VDD(V)                         = 3.3
  *            Main regulator output voltage  = Scale1 mode
  *            Flash Latency(WS)              = 3
  * @param  None
  * @retval None
  */
static void SystemClock_Config(void)
{
  RCC_ClkInitTypeDef RCC_ClkInitStruct;
  RCC_OscInitTypeDef RCC_OscInitStruct;
  
  HAL_StatusTypeDef ret = HAL_OK;

  /* Enable Power Control clock */
  __HAL_RCC_PWR_CLK_ENABLE();

  /* The voltage scaling allows optimizing the power consumption when the device is 
     clocked below the maximum system frequency, to update the voltage scaling value 
     regarding system frequency refer to product datasheet.  */
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /* Enable HSE Oscillator and activate PLL with HSE as source */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 8;
  RCC_OscInitStruct.PLL.PLLN = 200;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 7;
  RCC_OscInitStruct.PLL.PLLR = 2;
  ret = HAL_RCC_OscConfig(&RCC_OscInitStruct);
  
  if(ret != HAL_OK)
  {
    Error_Handler();
  }

  /* Select PLL as system clock source and configure the HCLK, PCLK1 and PCLK2 
     clocks dividers */
  RCC_ClkInitStruct.ClockType = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2);
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;
  ret = HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_3);
  if(ret != HAL_OK)
  {
    Error_Handler();
  }
  
}

/**
  * @brief  This function is executed in case of error occurrence.
  * @param  None
  * @retval None
  */
static void Error_Handler(void)
{
  /* User may add here some code to deal with this error */
  while(1)
  {
  }
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{ 
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
}
#endif

/**
  * @}
  */ 

/**
  * @}
  */ 

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
