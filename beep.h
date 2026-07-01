#ifndef _BEEP_H
#define _BEEP_H

#include "stm32f10x.h"
/*
Beep 模块头文件
	引脚和模块对应关系
	PB8 --> Beep
	
*/
#define BEEP_Toggle (GPIOB->ODR ^= (0x01 << 8))

/**
 * @brief Beep 模块初始化函数，需要将 Beep 对应 PB8 引脚初始化为 
 *       【通用推挽输出模式，最大速率为 2 MHz】
 */
void Beep_Init(void);

/**
 * @brief Beep 蜂鸣器控制函数
 *
 * @param flag 提供的实际参数为 
 *			1， Beep 模块工作，
 *			0, Beep 模块停止工作
 */
void Beep_Ctrl(u8 flag);

#endif

