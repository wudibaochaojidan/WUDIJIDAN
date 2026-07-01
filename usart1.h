#ifndef _USART1_H
#define _USART1_H

#include "common.h"

#define USART1_SYSTEM_CLOCK (72000000)

#define USART1_DATA_BUFFER_SIZE (256)

/*
USART1 用于存储数据的临时缓冲区结构体类型。
*/
typedef struct usart1_data
{
	// 【数据临时存储缓冲区数组】，总容量 USART1_DATA_BUFFER_SIZE
	u8 buffer[USART1_DATA_BUFFER_SIZE];
	/*
	【标志位】
		0 当前数据接收尚未完成
		1 当前数据接收已完成，获取底层数据接收缓冲区已满。
	接收完成标记为 IDLE 中断触发。
	*/
	u8 flag;
	// 当前 USART1_Data 底层缓冲区 buffer 存储的【有效元素个数】。
	u16 count;
} USART1_Data;

// 声明外部文件可用的全局变量
extern USART1_Data usart1_value;

/**
* @brief USART1 模块初始化函数，要求用户提供必要的波特率参数
*
* @param brr 当前 USART1 设置期望波特率
*/
void USART1_Init(u32 brr);

/**
 * @brief MCU 通过 USART1 模块发送一个字节数据
 * 
 * @param byte 需要发送的数据，数据大小为 1 个字节
 */
void USART1_SendByte(u8 byte);

/**
 * @brief MCU 通过 USART1 模块数据缓冲区，需要提供对应数据
 *			空间首地址 + 数据字节数
 * 
 * @param buffer 发送数据内容空间首地址
 * @param len    发送的数据有效字节
 */
void USART1_SendBuffer(const u8 *buffer, u16 len);

/**
 * @brief MCU 通过 USART1 模块发送字符串数据
 * 
 * @param str 需要发送的字符串数据内容
 */
void USART1_SendString(const char *str);

#endif
