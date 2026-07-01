#ifndef _DELAY_H
#define _DELAY_H

#include "stm32f10x.h"

/**
 * @brief 延时控制函数，单位 us 
 *
 * @param us 参数数据类型为 u32(unsigned int) 类型，对应
 *			当前延时控制的微秒数
 */
void Delay_us(u32 us);

/**
 * @brief 延时控制函数，单位 ms
 *
 * @param ms 参数数据类型为 u32(unsigned int) 类型，对应
 *			当前延时控制的毫秒数
 */
void Delay_ms(u32 ms);

#endif
