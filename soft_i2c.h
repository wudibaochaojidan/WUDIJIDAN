#ifndef _SOFT_I2C_H
#define _SOFT_I2C_H

#include "common.h"
#include "delay.h"

// 软件 I2C 引脚对应 GPIO 组
#define I2C_GPIO_GROUP GPIOB
// 软件 I2C SCL 时钟线对应引脚
#define I2C_SCL_PIN GPIO_Pin_6
// 软件 I2C SDA 数据线对应引脚
#define I2C_SDA_PIN GPIO_Pin_7

// I2C 延时函数控制周期为 5 us
#define I2C_DELAY_US (5)

typedef enum
{
    ACK = 0,
    NACK = 1
} ACK_TYPE;

/**
 * @brief 【软件 I2C】GPIO 引脚初始化函数
 *      PB6 ==> I2C_SCL 时钟线，默认工作模式为【通用开漏模式】
 *      PB7 ==> I2C_SDA 数据线，默认工作模式为【通用开漏模式】
 */
void I2C_GPIO_Init(void);

/**
 * @brief 【软件 I2C】控制 SCL 时钟线电平高低
 *
 * @param flag 用户提供 1，对应引脚为高电平，0 对应低电平
 */
void SCL_Set(u8 flag);

/**
 * @brief 【软件 I2C】控制 SDA 数据线电平高低
 *
 * @param flag 用户提供 1，对应引脚为高电平，0 对应低电平
 */
void SDA_Set(u8 flag);

/**
 * @brief 【软件 I2C】时钟周期控制延时函数
 */
void I2C_Delay(void);

/**
 * @brief 【软件 I2C】SDA 输出模式配置，对应所需模式为【通用开漏输出模式，速率 2 MHz】
 */
void SDA_Output_Mode(void);

/**
 * @brief 【软件 I2C】SDA 输入模式配置，对应所需模式为【浮空输入模式】
 */
void SDA_Input_Mode(void);

/**
 * @brief 【软件 I2C 核心函数】I2C 起始信号函数
 *      1. SCL 时钟线处于稳定的高电平状态
 *      2. SDA 数据线在 SCL 时钟线高电平状态下，完成了一次由高电平到低电平的跳变过程
 *      3. 并且 SCL 后续进入周期性高电平低电平变化，提供整个数据传递过程的参考周期
 */
void I2C_Start(void);

/**
 * @brief 【软件 I2C 核心函数】I2C 停止信号函数
 *      1. SCL 时钟线处于稳定的高电平状态
 *      2. SDA 数据线在 SCL 时钟线高电平状态下，完成了一次由低电平到高电平的跳变过程
 *      3. 后续 SDA 和 SCL 保持高电平状态，进入==> I2C 信号
 */
void I2C_Stop(void);

/**
 * @brief 【软件 I2C 核心函数】SDA 数据线读取电平情况下，主要用于满足
 *      1. MCU 主机读取从机发送应答信息 ACK or NACK
 *      2. MCU 读取从机发送的数据信息
 *      读取 SDA 数据线电平情况下，高电平对应数据 1，低电平对应数据 0
 *
 * @return 返回值对应 SDA 数据线电平情况，高电平返回 1，低电平返回 0
 */
u8 SDA_Read(void);

/**
 * @brief 【软件 I2C 核心函数】主机发送一个字节数据到从机设备，对应功能有
 *          1. 发送 7 位设备地址 + 读写标志位
 *          2. 目标写入数据 or 读取数据的从机寄存器地址
 *          3. 主机发送给从机进行处理的数据内容。
 *         核心内容
 *          1. SCL 高电平周期内容进行数据发送，SDA 需要在 SCL 高电平之前根据当前
 *              数据内容，调整电平高低，同时需要保证 SCL 低电平之前不会有任何变动
 *          2. 数据内容采用高位先发原则(MSB)
 *          3. 对 从机应答进行处理，SDA 转换工作模式，读取电平情况。
 * 
 * @param byte 利用 I2C 发送的一字节数据内容
 * @return 从机应答成功，返回 ACK(0)，否则 NACK(1)
 */
u8 I2C_SendByte(u8 byte);

/**
 * @brief 【软件 I2C 核心函数】主机读取从机发送的一个字节数据内容
 *      1. 要求 SDA 对应 GPIO 工作模式为浮空输入模式
 *      2. SDA 数据线电平情况完全交给从机控制，从机会再主机提供的 SCL 时钟
 *          参考信号的高电平周期内容将数据发送到 SDA 数据线
 *      3. 在 SCL 高电平周期内容读取 SDA 电平情况，从而获取从机发送数据。
 *
 * @param ack 主机给予从机应答信号，ACK(0) 告知从机继续发送数据 NACK(1)
 *              告知从机停止发送数据
 * @return 读取到的字节数据内容。
 */
u8 I2C_ReadByte(ACK_TYPE ack);

#endif
