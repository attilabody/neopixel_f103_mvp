
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  ** This notice applies to any and all portions of this file
  * that are not between comment pairs USER CODE BEGIN and
  * USER CODE END. Other portions of this file, whether 
  * inserted by the user or by software development tools
  * are owned by their respective copyright owners.
  *
  * COPYRIGHT(c) 2018 STMicroelectronics
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
#include "gpio.h"

/* USER CODE BEGIN Includes */
#include "bitband.h"
/* USER CODE END Includes */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
/* Private variables ---------------------------------------------------------*/

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
static void LL_Init(void);
void SystemClock_Config(void);

/* USER CODE BEGIN PFP */
/* Private function prototypes -----------------------------------------------*/

/* USER CODE END PFP */

/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  *
  * @retval None
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration----------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  LL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  /* USER CODE BEGIN 2 */

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  asm volatile(
	"	movs r3, #1000	\n\t"
	"1:	subs r3, #1		\n\t"
	"	bne 1b			\n\t"
	: : : "r3", "cc"
  );

  asm volatile("\tmovs r3, #1000\n1:\tsubs r3, #1\n\tbne 1b\n\t" : : : "r3", "cc");

  GPIOB->ODR = 0;
  volatile uint32_t	v;
  uint8_t ledbytes[] = {0, 0xff, 0, 0xff, 0x55, 0xaa};

  __disable_irq();

#define W_1f 5
#define W_0f 0
#define W_0s 3

#define W_1af
#define W_1as ++v; ++v
#define W_0af ++v; ++v
#define W_0as ++v; ++v
/*
#define SHIFTBIT(x) \
	if(b&(1<<x)) { \
		GPIOB->BSRR = (1 << 11);	\
		for( v=W_1f; v != 0; --v); \
		W_1af; \
		GPIOB->BSRR = (1 << (16+11));	\
		W_1as; \
	} else { \
		GPIOB->BSRR = (1 << 11); \
		for( v=W_0f; v != 0; --v); \
		W_0af; \
		GPIOB->BSRR = (1 << (16+11)); \
		for( v=W_0s; v != 0; --v); \
		W_0as; \
	}
*/
#define SHIFTBIT(x) \
	if(b&(1<<x)) { \
		GPIOB->BSRR = (1 << 11);	\
		asm volatile("\tmov r3, #10\n1:\tsubs r3, #1\n\tbne 1b\n\t" : : : "r3", "cc"); \
		GPIOB->BSRR = (1 << (16+11));	\
		asm volatile("\tmov r3, #03\n1:\tsubs r3, #1\n\tbne 1b\n\t" : : : "r3", "cc"); \
	} else { \
		GPIOB->BSRR = (1 << 11); \
		asm volatile("\tmov r3, #03\n1:\tsubs r3, #1\n\tbne 1b\n\t" : : : "r3", "cc"); \
		GPIOB->BSRR = (1 << (16+11)); \
		asm volatile("\tmov r3, #10\n1:\tsubs r3, #1\n\tbne 1b\n\t" : : : "r3", "cc"); \
	}

__disable_irq();

  while(1)
  {
	uint8_t	*bufPtr = ledbytes;
	uint32_t	cnt = sizeof(ledbytes) + 1;

	while(--cnt) {
		uint32_t b = *bufPtr++;
		SHIFTBIT(7);
		SHIFTBIT(6);
		SHIFTBIT(5);
		SHIFTBIT(4);
		SHIFTBIT(3);
		SHIFTBIT(2);
		SHIFTBIT(1);

		if(b&1) {
			GPIOB->BSRR = 1 << 11;
			for( v=W_1f; v != 0; --v);
			W_1af;
			GPIOB->BSRR = 1 << (16+11);
			W_1as;
		} else {
			GPIOB->BSRR = 1 << 11;
			for( v=W_0f; v != 0; --v);
			W_0af;
			GPIOB->BSRR = 1 << (16+11);
			for( v=W_0s; v != 0; --v);
			++v; v=0;
		}
	}

  /* USER CODE END WHILE */

  /* USER CODE BEGIN 3 */
	for( v=100; v != 0; --v);		//long
  }
  /* USER CODE END 3 */

}
static void LL_Init(void)
{
  

  LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_AFIO);
  LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_PWR);

  NVIC_SetPriorityGrouping(NVIC_PRIORITYGROUP_4);

  /* System interrupt init*/
  /* MemoryManagement_IRQn interrupt configuration */
  NVIC_SetPriority(MemoryManagement_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(),0, 0));
  /* BusFault_IRQn interrupt configuration */
  NVIC_SetPriority(BusFault_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(),0, 0));
  /* UsageFault_IRQn interrupt configuration */
  NVIC_SetPriority(UsageFault_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(),0, 0));
  /* SVCall_IRQn interrupt configuration */
  NVIC_SetPriority(SVCall_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(),0, 0));
  /* DebugMonitor_IRQn interrupt configuration */
  NVIC_SetPriority(DebugMonitor_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(),0, 0));
  /* PendSV_IRQn interrupt configuration */
  NVIC_SetPriority(PendSV_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(),0, 0));
  /* SysTick_IRQn interrupt configuration */
  NVIC_SetPriority(SysTick_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(),0, 0));

    /**NOJTAG: JTAG-DP Disabled and SW-DP Enabled 
    */
  LL_GPIO_AF_Remap_SWJ_NOJTAG();

}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{

  LL_FLASH_SetLatency(LL_FLASH_LATENCY_2);

   if(LL_FLASH_GetLatency() != LL_FLASH_LATENCY_2)
  {
    Error_Handler();  
  }
  LL_RCC_HSE_Enable();

   /* Wait till HSE is ready */
  while(LL_RCC_HSE_IsReady() != 1)
  {
    
  }
  LL_RCC_PLL_ConfigDomain_SYS(LL_RCC_PLLSOURCE_HSE_DIV_1, LL_RCC_PLL_MUL_9);

  LL_RCC_PLL_Enable();

   /* Wait till PLL is ready */
  while(LL_RCC_PLL_IsReady() != 1)
  {
    
  }
  LL_RCC_SetAHBPrescaler(LL_RCC_SYSCLK_DIV_1);

  LL_RCC_SetAPB1Prescaler(LL_RCC_APB1_DIV_2);

  LL_RCC_SetAPB2Prescaler(LL_RCC_APB2_DIV_1);

  LL_RCC_SetSysClkSource(LL_RCC_SYS_CLKSOURCE_PLL);

   /* Wait till System clock is ready */
  while(LL_RCC_GetSysClkSource() != LL_RCC_SYS_CLKSOURCE_STATUS_PLL)
  {
  
  }
  LL_Init1msTick(72000000);

  LL_SYSTICK_SetClkSource(LL_SYSTICK_CLKSOURCE_HCLK);

  LL_SetSystemCoreClock(72000000);

  /* SysTick_IRQn interrupt configuration */
  NVIC_SetPriority(SysTick_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(),0, 0));
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @param  file: The file name as string.
  * @param  line: The line in file as a number.
  * @retval None
  */
void _Error_Handler(char *file, int line)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  while(1)
  {
  }
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
void assert_failed(uint8_t* file, uint32_t line)
{ 
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
