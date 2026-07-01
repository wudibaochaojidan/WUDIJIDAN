#ifndef _COMMON_H
#define _COMMON_H

#include "stm32f10x.h"

/*
时钟使能宏定义，用于使能 APB1 APB2 外设。
*/
#define APB2_AFIO_CLOCK_ENABLE   (0x01)
#define APB2_IOPA_CLOCK_ENABLE   (0x01 << 2)
#define APB2_IOPB_CLOCK_ENABLE   (0x01 << 3)
#define APB2_IOPC_CLOCK_ENABLE   (0x01 << 4)
#define APB2_IOPD_CLOCK_ENABLE   (0x01 << 5)
#define APB2_IOPE_CLOCK_ENABLE   (0x01 << 6)
#define APB2_IOPF_CLOCK_ENABLE   (0x01 << 7)
#define APB2_IOPG_CLOCK_ENABLE   (0x01 << 8)
#define APB2_USART1_CLOCK_ENABLE (0x01 << 14)


#define APB1_TIM2_CLOCK_ENABLE (0x01)
#define APB1_TIM3_CLOCK_ENABLE (0x01 << 1)
#define APB1_TIM4_CLOCK_ENABLE (0x01 << 2)
#define APB1_TIM5_CLOCK_ENABLE (0x01 << 3)
#define APB1_TIM6_CLOCK_ENABLE (0x01 << 4)
#define APB1_TIM7_CLOCK_ENABLE (0x01 << 5)


/*
GPIO 工作模式宏定义
*/
// 通用推挽输出模式不同速率二进制宏定义
#define Push_Pull_Output_Speed_2MHz  (0x02)  // 0010
#define Push_Pull_Output_Speed_10MHz (0x01)  // 0001
#define Push_Pull_Output_Speed_50MHz (0x03)  // 0011

// 复用推挽输出模式不同速率二进制宏定义
#define AF_Push_Pull_Output_Speed_2MHz  (0x0A)  // 1010
#define AF_Push_Pull_Output_Speed_10MHz (0x09)  // 1001
#define AF_Push_Pull_Output_Speed_50MHz (0x0B)  // 1011

// 上拉 or 下拉 GPIO 输入模式二进制宏定义
#define Pull_Up_Or_Pull_Dowm_Input_Mode (0x08) // 1000

// 浮空输入 GPIO 输入模式二进制宏定义
#define Floating_Input_Mode (0x04) // 0100


#endif

