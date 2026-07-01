#ifndef _EEPROM_AT24C02_H
#define _EEPROM_AT24C02_H

#include "common.h"
#include "soft_i2c.h"

/*
I2C 读写标志位枚举类型
*/
typedef enum
{
    WRITE_FLAG,
    READ_FLAG,
} I2C_RW_FLAG;

// I2C 指定指令对应的尝试重复次数
#define I2C_RETRY_COUNT (3)

/**
 * @brief 写入一个字节数据到指定设备地址 EERPOM AT24C02 芯片中，要求用户
- *      提供目标写入数据寄存器地址和 1 字节数据内容
 * 
 * @param eeprom_addr AT24C02 EEPROM 存储芯片 I2C 设备地址 
 * @param reg_addr    目标写入数据的寄存器地址，范围是 0x00 ~ 0xFF
 * @param data        目标写入 1 字节数据内容
 * @return 写入数据成功返回 0，否则返回 1
 */
u8 EEPROM_WriteByte(u8 eeprom_addr, u8 reg_addr, u8 data);

/**
 * @brief 读取 EEPROM AT24C02 指定寄存器地址对应 1 字节数据内容，数据存储
 *      到 data 指针存储对应变量中。
 * 
 * @param eeprom_addr AT24C02 EEPROM 存储芯片 I2C 设备地址 
 * @param reg_addr    目标读取数据的寄存器地址，范围是 0x00 ~ 0xFF
 * @param data        读取数据存储对应变量指针变量
 * @return 读取数据成功，返回 0，否则返回 1
 */
u8 EEPROM_ReadByte(u8 eeprom_addr, u8 reg_addr, u8 *data);

#endif
