#ifndef _USART3_H
#define _USART3_H

#include "common.h"
#include "usart1.h"

// USART3 数据接收缓冲区数组底层字节数
#define USART3_DATA_BUFFER_SIZE (256)

// USART3 数据接收缓冲区结构体
typedef struct usart3_data
{
    // 最大存储 256 字节数据缓冲区数组
    u8 buffer[USART3_DATA_BUFFER_SIZE];
    // 缓冲区数组有效字节个数
    u16 count;
    // 数据接收完成 or 数据缓冲区已满标记
    u8 flag;
} USART3_Data;

extern USART3_Data usart3_value;

/**
 * @brief USART3 初始化函数，需要用户提供对应波特率参数
 *        串口配置核心内容
 *             1. NRZ 数据格式 8N1
 *             2. 数据发送和数据接收对应波特率
 *             3. USART3 对应中断使能 (IDLE RXNE) 和注册
 *             4. 使能 USART3 TX 和 RX 功能
 *             5. USART3 模块开启
 *       需要配置 USART3 模块对应 GPIO 引脚工作模式
 *           PB11 USART3_RX ==> 【浮空输入模式】
 *           PB10 USART3_TX ==> 【复用推挽输出模式 速度 2 MHz】
 * 
 * @param brr 用户提供的 USART3 工作波特率
 */
void USART3_Init(u32 brr);

/**
 * @brief 通过 USART3 模块发送一个字节数据
 * 
 * @param byte 发送的字节数据内容
 */
void USART3_SendByte(u8 byte);

/**
 * @brief 通过 USART3 模块发送字节数据缓冲区
 * 
 * @param buffer 发送字节缓冲区内存空间首地址
 * @param len    数据缓冲区字节个数
 */
void USART3_SendBuffer(u8 *buffer, u16 len);

/**
 * @brief 通过 USART3 模块发送字符串数据内容
 * 
 * @param str 需要发送的字符串数据
 */
void USART3_SendString(const char *str);

#endif

