#include "stm32f302x8.h"
#include "stm32f3xx_hal.h"
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include "demo_bsp.h"

#define LD2_Pin GPIO_PIN_13
#define LD2_GPIO_Port GPIOB

extern TIM_HandleTypeDef htim2;
extern UART_HandleTypeDef huart2;

const unsigned timer_irq = TIM2_IRQn;
void SystemClock_Config(void);
void _MX_GPIO_Init(void);
void _MX_USART2_UART_Init(void);
void _MX_TIM2_Init(void);
void Error_Handler(void);

void
led_init()
{
    ;
}

inline void
led_on()
{
    HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, SET);
}

inline void
led_off()
{
    HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, RESET);
}

inline void
timer_restart(void)
{
    __HAL_TIM_CLEAR_IT(&htim2, TIM_IT_UPDATE);
}

void
timer_init(void)
{
    _MX_TIM2_Init();
    __HAL_TIM_CLEAR_FLAG(&htim2, TIM_SR_UIF);
    HAL_TIM_Base_Start_IT(&htim2);
}

void
console_init(void)
{
    _MX_USART2_UART_Init();
}

void
core_init(void)
{
	HAL_Init();
	SystemClock_Config();
    _MX_GPIO_Init();

    NVIC_SetPriority(PendSV_IRQn, 0xFF);
    NVIC_SetPriority(SysTick_IRQn, 0xFF);
}

//
// Retargeting printf for the GNU Compiler Collection
//
int __io_putchar(int ch)
{
	return HAL_UART_Transmit(&huart2, (uint8_t*)&ch, 1, 1);
}

int __io_getchar(void)
{
	uint8_t ch = 0;
	__HAL_UART_CLEAR_OREFLAG(&huart2);
	do{
		HAL_UART_Receive(&huart2, &ch, 1, 0);
	}while(ch == 0);
	return ch;
}

//!
//! Unified fault handler
//!
void system_stop(void)
{
    for (;;);
}
