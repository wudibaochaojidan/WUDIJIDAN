#include "led.h"

void LED_Init(void) 
{
	// 1. RCC 时钟使能操作，需要使能模块 GPIOB 和 GPIOE
	RCC->APB2ENR |= (0x01 << 6) | (0x01 << 3);
	
	/*
	2. PB5 和 PE5 两个 GPIO 引脚配置为
		【通用推挽输出模式，速率 2 MHz】
	*/
	GPIOB->CRL &= ~(0x0F << 20);
	GPIOB->CRL |= (0x02 << 20);
	
	GPIOE->CRL &= ~(0x0F << 20);
	GPIOE->CRL |= (0x02 << 20);
	
	/*
	3. 默认当前 LED 处于熄灭状态，要求 PB5 和 PE5 输出高电平信号
	*/
	GPIOB->ODR |= (0x01 << 5);
	GPIOE->ODR |= (0x01 << 5);
}

void LED0_Ctrl(u8 flag) 
{
	if (flag)
	{	
		// LED0 点亮
		GPIOB->ODR &= ~(0x01 << 5);
	}
	else	
	{
		// LED0 熄灭
		GPIOB->ODR |= (0x01 << 5);
	}
}

void LED1_Ctrl(u8 flag) 
{
	if (flag)
	{	
		// LED1 点亮
		GPIOE->ODR &= ~(0x01 << 5);
	}
	else	
	{
		// LED1 熄灭
		GPIOE->ODR |= (0x01 << 5);
	}
}
