#ifndef _TIM2_H
#define _TIM2_H

#include "common.h"

#include "usart1.h"
#include "led.h"

/**
 * @brief 通用定时器基本定时功能实现案例，采用中断方式完成定时器任务。
 * 
 * @param psc 预分频器倍数
 * @param arr 自动重装载寄存器
 */
void TIM2_Init(u16 psc, u16 arr);

#endif
