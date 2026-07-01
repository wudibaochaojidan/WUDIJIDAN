#ifndef _KEY_H
#define _KEY_H

#include "common.h"

#include "delay.h"
#include "led.h"
#include "usart1.h"

/*
按照状态枚举/数据枚举
*/
typedef enum
{
	KEY_UP_VALUE = 0,
	KEY2_VALUE,
	KEY1_VALUE,
	KEY0_VALUE,
	NO_KEY_CLICK
} KEY_Value;


/**
 * @brief 按键模块初始化函数，需要完成的内容如下
 *		KEY_UP 对应 PA0 引脚，要求 PA0 引脚工作模式【下拉输入模式】
 *		KEY_2 对应 PE2 引脚，要求 PE2 引脚工作模式【上拉输入模式】
 *		KEY_1 对应 PE3 引脚，要求 PE3 引脚工作模式【上拉输入模式】
 *		KEY_0 对应 PE4 引脚，要求 PE4 引脚工作模式【上拉输入模式】
 */
void KEY_Init(void);

/**
 * @brief 返回当前 KEY 模块中，哪一个按键被按下，返回值是对应按键数据枚举
 * 
 * @return 返回值对应 KEY_Value 枚举类型
 *			KEY_UP 按键按下返回 	      KEY_UP_VALEU	
 *			KEY2 按键按下返回 	      KEY2_VALEU	
 *			KEY1 按键按下返回 	      KEY1_VALEU	
 *			KEY0 按键按下返回 	      KEY0_VALEU	
 *			Have No Key Click Return  NO_KEY_CLICK
 */
KEY_Value KEY_Click(void);

/**
* @brief 按键外部中断控制线配置函数，需要完成
*		KEY_UP ==> PA0 ==> EXIT0 ==> 上升沿触发中断		
*		KEY2 ==> PE2 ==> EXIT2 ==> 下降沿触发中断		
*		KEY1 ==> PE3 ==> EXIT3 ==> 下降沿触发中断		
*		KEY0 ==> PE4 ==> EXIT4 ==> 下降沿触发中断【已完成】	
*/ 
void Key_EXTI_Interrupt(void);

#endif 
