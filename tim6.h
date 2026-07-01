#ifndef _TIM6_H
#define _TIM6_H

#include "common.h"

#include "led.h"
#include "usart1.h"

/** 
 * @brief TIM6 基础定时器初始化操作，要求提供的参数有
 * 		1. PSC 预分频器倍数
 *      2. ARR 自动重装载寄存器
 *      例如 MCU 时钟频率为 72 MHz，提供的 PSC 为 72，ARR 为 1000
 *			当前定时器工作周期时间为 1 ms
 *
 * @param psc 预分频器倍数
 * @param arr 自动重装载寄存器
 */
void TIM6_Init(u16 psc, u16 arr);

#endif

