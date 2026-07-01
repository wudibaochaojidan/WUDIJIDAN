#include "beep.h"

/**
 * @brief Beep 模块初始化函数，需要将 Beep 对应 PB8 引脚初始化为 
 *       【通用推挽输出模式，最大速率为 2 MHz】
 */
void Beep_Init(void) 
{
	// 1. RCC 时钟使能，需要通过 APB2 分支，完成 GPIOB 时钟使能。
	RCC->APB2ENR |= 0x01 << 3;
	
	// 2. GPIOB 组中对应 PB8 引脚配置工作模式为【通用推挽输出模式，最大速度为 2 MHz】
	GPIOB->CRH &= ~(0x0F);
	GPIOB->CRH |= 0x02;
	
	// 3. PB8 默认电平为低电平，Beep 默认关闭模式
	GPIOB->ODR &= ~(0x01 << 8);
}	


/**
 * @brief Beep 蜂鸣器控制函数
 *
 * @param flag 提供的实际参数为 
 *			1， Beep 模块工作，
 *			0, Beep 模块停止工作
 */
void Beep_Ctrl(u8 flag) 
{
	if (flag)
	{
		// PB8 引脚高电平，Beep 工作
		GPIOB->ODR |= (0x01 << 8);
	}
	else
	{
		// PB8 引脚低电平，Beep 关闭
		GPIOB->ODR &= ~(0x01 << 8);
	}
}
