#include "eeprom_at24c02.h"

/**
 * @brief 写入一个字节数据到指定设备地址 EERPOM AT24C02 芯片中，要求用户
 *      提供目标写入数据寄存器地址和 1 字节数据内容
 *
 * @param eeprom_addr AT24C02 EEPROM 存储芯片 I2C 设备地址
 * @param reg_addr    目标写入数据的寄存器地址，范围是 0x00 ~ 0xFF
 * @param data        目标写入 1 字节数据内容
 * @return 写入数据成功返回 0，否则返回 1
 */
u8 EEPROM_WriteByte(u8 eeprom_addr, u8 reg_addr, u8 data)
{
    // 1. 当前 I2C 写入数据尝试次数
    u8 retry_count = I2C_RETRY_COUNT;

    // 2. I2C 写入数据超时时间判断, 为 10 ms
    u8 timeout = 10;

    /*
    【注意】
        I2C 写入数据帧任何一个操作从机应答位 NACK，都需要立刻停止 I2C 传递 ==> I2C_Stop,
        从 I2C_Start 从新开始完整数据帧传递。
    */
    while (retry_count--)
    {
        /*
        【I2C 写入数据帧内容】
            1. I2C 起始信号
            2. I2C 目标设备地址 + 写入标志位，处理应答信息
            3. I2C 目标写入数据寄存器地址，处理应答信息
            4. I2C 目标写入 1 字节数据内容，处理应答信息
            5. I2C 停止信号
        */

        /*
        【I2C 写入数据帧内容】
            1. I2C 起始信号
        */
        I2C_Start();

        /*
        【I2C 写入数据帧内容】
            2. I2C 目标设备地址 + 写入标志位，处理应答信息
        */
        if (I2C_SendByte(eeprom_addr << 1 | WRITE_FLAG))
        {
            I2C_Stop();
            continue;
        }

        /*
        【I2C 写入数据帧内容】
            3. I2C 目标写入数据寄存器地址，处理应答信息
        */
        if (I2C_SendByte(reg_addr))
        {
            I2C_Stop();
            continue;
        }

        /*
        【I2C 写入数据帧内容】
            4. I2C 目标写入 1 字节数据内容，处理应答信息
        */
        if (I2C_SendByte(data))
        {
            I2C_Stop();
            continue;
        }

        /*
        【I2C 写入数据帧内容】
            5. I2C 停止信号
        */
        I2C_Stop();

        /*
        如果当前 I2C 整个写入数据帧发送完成！！！并且从机应答没有任何的问题。表示 I2C 数据
        发送成功！！！此时 EEPROM 会进入【处理数据阶段】。

        【EEPROM 进入冥想模式】EEPROM AT24C02 芯片不会应答任何的外部请求！！！利用特征，通过
        循环方式发送 EEPROM AT24C02 设备地址 + 读写标志位信息，判断设备是否有应答。
        如果在正常的时间范围内容，EEPROM AT24C02 设备有应答信息，表示写入数据成功
        如果超时(10 ms) 表示本次写入数据失败！！！
        */
        while (timeout--)
        {
            // 发送 I2C 起始信号
            I2C_Start();
            /*
            I2C_SendByte 发送设备地址 + 写入标志位，
            如果从机应答 NACK(1) 表示当前从机还在处理数据中
            如果从机应答 ACK(0) 表示当前从机处理数据完成
            */
            if (!I2C_SendByte(eeprom_addr << 1 | WRITE_FLAG))
            {
                // I2C 数据总线空闲状态！！！
                I2C_Stop();

                // 写入数据成功！！！
                return 0;
            }

            I2C_Stop();

            Delay_ms(1);
        }

        /*
        如果当前处理数据阶段判断 while 循环因为 timeout 为 0 退出循环，表示 EEPROM 数据
        写入失败！！！需要重新尝试，此时需要 timeout 重新赋值为 10
        */
        timeout = 10;
    }

    return 1;
}

/**
 * @brief 读取 EEPROM AT24C02 指定寄存器地址对应 1 字节数据内容，数据存储
 *      到 data 指针存储对应变量中。
 *
 * @param eeprom_addr AT24C02 EEPROM 存储芯片 I2C 设备地址
 * @param reg_addr    目标读取数据的寄存器地址，范围是 0x00 ~ 0xFF
 * @param data        读取数据存储对应变量指针变量
 * @return 读取数据成功，返回 0，否则返回 1
 */
u8 EEPROM_ReadByte(u8 eeprom_addr, u8 reg_addr, u8 *data)
{
    // 1. 当前 I2C 写入数据尝试次数
    u8 retry_count = I2C_RETRY_COUNT;

    u8 flag = 0;

    /*
    【I2C 读取数据帧内容】
        1. I2C 起始信号
        2. I2C 发送设备地址 + 写入标志位信息，找到目标设备，同时处理从机应答数据
        3. I2C 发送目标读取寄存器地址，同时处理从机应答数据
        4. I2C 起始信号
        5. I2C 发送设备地址 + 读取标志位信息，找到目标设备，同时处理从机应答数据
        6. 主机 I2C 读取从机发送数据内容，同时根据【主机应答信息】告知从机是否继续发送。
        7. I2C 停止信号
    */
    while (retry_count--)
    {
        flag = 0;

        /*
        【I2C 读取数据帧内容】
            1. I2C 起始信号
        */
        I2C_Start();

        /*
        【I2C 读取数据帧内容】
            2. I2C 发送设备地址 + 写入标志位信息，找到目标设备，同时处理从机应答数据
        */
        if (I2C_SendByte(eeprom_addr << 1 | WRITE_FLAG))
        {
            I2C_Stop();
            flag = 1;
            continue;
        }

        /*
        【I2C 读取数据帧内容】
            3. I2C 发送目标读取寄存器地址，同时处理从机应答数据
        */
        if (I2C_SendByte(reg_addr))
        {
            I2C_Stop();
            flag = 1;
            continue;
        }

        /*
        【I2C 读取数据帧内容】
            4. I2C 起始信号
        */
        I2C_Start();

        /*
        【I2C 读取数据帧内容】
            5. I2C 发送设备地址 + 读取标志位信息，找到目标设备，同时处理从机应答数据
        */
        if (I2C_SendByte(eeprom_addr << 1 | READ_FLAG))
        {
            I2C_Stop();
            flag = 1;
            continue;
        }

        /*
        【I2C 读取数据帧内容】
            6. 主机 I2C 读取从机发送数据内容，同时根据【主机应答信息】告知从机是否继续发送
            本次读取 1 字节数据内容，当前主机应答信号选择 NACK(1)，告知从机不再进行数据发送
        */
        *data = I2C_ReadByte(1);

        /*
        【I2C 读取数据帧内容】
            7. I2C 停止信号
        */
        I2C_Stop();
    }

    return flag;
}
