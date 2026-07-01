#ifndef _LED_H
#define _LED_H

#include "stm32f10x.h"


#define LED0_Toggle GPIOB->ODR ^= (0x01 << 5)
#define LED1_Toggle GPIOE->ODR ^= (0x01 << 5)

/**
 * @brief LED 模块初始化操作，需要完成的初始化内容包括
 *		包括 DS0 和 DS1 两个 LED 灯
 */
void LED_Init(void);

/**
 * @brief LED0 亮灭控制，用户根据 flag 参数控制 LED 亮灭
 *
 * @param flag 用户提供 1，当前 LED 点亮，用户提供 0，LED 熄灭
 */
void LED0_Ctrl(u8 flag);

/**
 * @brief LED1 亮灭控制，用户根据 flag 参数控制 LED 亮灭
 *
 * @param flag 用户提供 1，当前 LED 点亮，用户提供 0，LED 熄灭
 */
void LED1_Ctrl(u8 flag);

#endif

