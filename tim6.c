#include "tim6.h"

void TIM6_Init(u16 psc, u16 arr)
{
	/*
	【TIM6 基础定时器开发流程】
		1. RCC 时钟使能
		2. TIM6 PSC + ARR 配置
		3. TIM6 定时器配置周期性 or 一次性工作模式
		4. 使能 TIM6 中断
		5. 注册 TIM6_IRQn 中断
		6. 启动 TIM6 定时器
		7. 利用 TIM6_IRQHandler 作为定时器任务函数。
	*/
	// 1. RCC 时钟使能
	RCC->APB1ENR |= APB1_TIM6_CLOCK_ENABLE;
	
	// 【重点补充】保证当前 TIM6 定时器处于关闭状态。
	TIM6->CR1 &= ~(0x01);
	
	// 2. TIM6 PSC + ARR 配置
	/*
	【重点】
		1. 根据 PSC 寄存器要求，提供给 PSC 的实际数据为用户提供参数 - 1
		2. ARR 数据为用户提供参数 - 1， 需要考虑 ARR 重新赋值 CNT 操作时间。
	*/
	TIM6->PSC = psc - 1;
	TIM6->ARR = arr - 1;
	
	// 3. TIM6 定时器配置周期性 or 一次性工作模式
	TIM6->CR1 &= ~(0x01 << 3); // 当前定时器为周期性工作定时器
	
	// 4. 使能 TIM6 中断
	TIM6->DIER |= (0x01);
	
	// 5. 注册 TIM6_IRQn 中断
	NVIC_EnableIRQ(TIM6_IRQn);
	NVIC_SetPriority(TIM6_IRQn, 1);
	
	// 6. 启动 TIM6 定时器
	TIM6->CR1 |= (0x01);
}

// 7. 利用 TIM6_IRQHandler 作为定时器任务函数。
void TIM6_IRQHandler(void)
{
	/*
	基础定时器 TIM6 SR 寄存器有且只有 1 位数据有效
	如果触发中断 TIM6->SR 对应数据为 0x01，满足 if 
	条件执行。
	*/
	if (TIM6->SR)
	{
		// 清除中断标志位
		TIM6->SR = 0;
		
		LED0_Toggle;
		LED1_Toggle;
		
		USART1_SendString("TIM6 Update Interrupt\r\n");
	}
}






