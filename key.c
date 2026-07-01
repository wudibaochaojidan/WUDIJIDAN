#include "key.h"

void KEY_Init(void)
{
	/*
	1. 时钟使能，要求使能模块 GPIOA 和 GPIOE
	*/
	RCC->APB2ENR |= (0x01 << 6) | (0x01 << 2);
	
	/*
	2. GPIO 引脚初始化配置
		PA0 对应 KEY_UP 工作模式为 下拉输入模式
		PE2 对应 KEY_2 工作模式为 上拉输入模式
		PE3 对应 KEY_1 工作模式为 上拉输入模式
		PE4 对应 KEY_0 工作模式为 上拉输入模式
		
	【注意】
		不要相信所谓的默认值数据，有可能当前引脚之前有其他
		操作，其他数据。需要在当前操作时，保证对应引脚状态
		引脚数据为当前可用状态
		【已知，可控】
		【数据类型一致化原则】
	*/
	GPIOA->CRL &= ~(0x0F);
	GPIOA->CRL |= (0x08);
	GPIOA->ODR &= ~(0x01);
	
	GPIOE->CRL &= ~(0x0FFF << 8);
	GPIOE->CRL |= (0x0888 << 8);
	GPIOE->ODR |= (0x07 << 2);	
}

KEY_Value KEY_Click(void)
{
	/*
	KEY_UP 按键按下判断
		1. KEY_UP 按下之后，提供给 PA0 电平为高电平信号
			对应 CPIOA->IDR 寄存器中 位[0] 为 1
		2. 利用 延时进行消抖操作，消抖之后再次判断
		3. 按键功能是将 LED0 进行亮灭控制
	*/
	if (0x01 & GPIOA->IDR)
	{
		// 消抖
		Delay_ms(20);
		
		if (0x01 & GPIOA->IDR)
		{	
			while (0x01 & GPIOA->IDR);
			return KEY_UP_VALUE;
		}
	}
	
	return NO_KEY_CLICK;
}


void Key_EXTI_Interrupt(void)
{
	/*
	【实现流程】
		1. AFIO 时钟使能，当前 EXTI 属于 GPIO 引脚复用功能
		2. AFIO 通过 EXTICR 寄存器配置 EXTI4 对应 PE4 引脚。
		3. EXTI 外部中断控制线, 对应 EXIT4 配置
			3.1 选择下降沿方式触发中断 --> FTSR
			3.2 使能当前中断           --> IMR
		4. 注册 EXTI4_IRQn 中断请求编号，并且设置对应的优先级
		5. 实现 EXTI4_IRQHandler 函数
	*/
	
	// 1. AFIO 时钟使能，当前 EXTI 属于 GPIO 引脚复用功能
	RCC->APB2ENR |= APB2_AFIO_CLOCK_ENABLE;
	
	// 2. AFIO 通过 EXTICR 寄存器配置 EXTI4 对应 PE4 引脚。
	AFIO->EXTICR[1] &= ~(0x0F);
	AFIO->EXTICR[1] |= (0x04);
	
	// 3. EXTI 外部中断控制线, 对应 EXIT4 配置 
	// 3.1 选择下降沿方式触发中断 --> FTSR
	EXTI->FTSR |= (0x01 << 4);
	// 3.2 使能当前中断           --> IMR
	EXTI->IMR |= (0x01 << 4);
	
	// 4. 注册 EXTI4_IRQn 中断请求编号，并且设置对应的优先级
	NVIC_EnableIRQ(EXTI4_IRQn);
	NVIC_SetPriority(EXTI4_IRQn, 1);
}

void EXTI4_IRQHandler(void)
{
	// 5. 实现 EXTI4_IRQHandler 函数
	
	// 5.1 判断当前中断是否触发
	if (EXTI->PR & (0x01 << 4))
	{
		// 5.2 清除当前中断标志位
		// 对应标志位写入 1 清除
		EXTI->PR |= (0x01 << 4);
		
		// LED0 亮灭翻转控制
		LED0_Toggle;
		
		USART1_SendString("KEY0 Clicked!\r\n");
	}
}
