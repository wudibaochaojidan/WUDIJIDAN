#include "soft_i2c.h"

/**
 * @brief 【软件 I2C】GPIO 引脚初始化函数
 *      PB6 ==> I2C_SCL 时钟线，默认工作模式为【通用开漏模式】
 *      PB7 ==> I2C_SDA 数据线，默认工作模式为【通用开漏模式】
 */
void I2C_GPIO_Init(void)
{
    /*
    1. 时钟使能，需要使能模块对应 GPIOB 模块
    */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

    /*
    2. 配置 GPIOB 中 PB6 和 PB7 都是默认【通用开漏模式】，速率 2 MHz
    */
    GPIO_InitTypeDef GPIO_InitStructure = {0};

    GPIO_InitStructure.GPIO_Pin = I2C_SCL_PIN | I2C_SDA_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD; // 通用开漏模式
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz; // 速率要求 2 MHz 满足 I2C 设备使用

    GPIO_Init(I2C_GPIO_GROUP, &GPIO_InitStructure);

    /*
    3. 【重点】默认 SCL 和 SDA 电平都为高电平
        当前两个引脚对应都是【通用开漏模式】，N-MOS 截止状态下，对应
        SCL 和 SDA 默认电平都是高电平。
        GPIO 控制，输出高电平即可【ODR = 1】
    */
    SDA_Set(1);
    SCL_Set(1);
}

/**
 * @brief 控制 SCL 时钟线电平高低
 *
 * @param flag 用户提供 1，对应引脚为高电平，0 对应低电平
 */
void SCL_Set(u8 flag)
{
    if (flag)
    {
        // 设置当前 GPIO 引脚输出高电平信号
        GPIO_SetBits(I2C_GPIO_GROUP, I2C_SCL_PIN);
    }
    else
    {
        // 设置当前 GPIO 引脚输出低电平信号
        GPIO_ResetBits(I2C_GPIO_GROUP, I2C_SCL_PIN);
    }
}

/**
 * @brief 控制 SDA 数据线电平高低
 *
 * @param flag 用户提供 1，对应引脚为高电平，0 对应低电平
 */
void SDA_Set(u8 flag)
{
    if (flag)
    {
        GPIO_SetBits(I2C_GPIO_GROUP, I2C_SDA_PIN);
    }
    else
    {
        GPIO_ResetBits(I2C_GPIO_GROUP, I2C_SDA_PIN);
    }
}

/**
 * @brief 【软件 I2C】时钟周期控制延时函数
 */
void I2C_Delay(void)
{
    Delay_us(I2C_DELAY_US);
}

/**
 * @brief 【软件 I2C】SDA 输出模式配置，对应所需模式为【通用开漏输出模式，速率 2 MHz】
 */
void SDA_Output_Mode(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    GPIO_InitStruct.GPIO_Pin = I2C_SDA_PIN;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_OD;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_2MHz;

    GPIO_Init(I2C_GPIO_GROUP, &GPIO_InitStruct);
}

/**
 * @brief 【软件 I2C】SDA 输入模式配置，对应所需模式为【浮空输入模式】
 */
void SDA_Input_Mode(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    GPIO_InitStruct.GPIO_Pin = I2C_SDA_PIN;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;

    GPIO_Init(I2C_GPIO_GROUP, &GPIO_InitStruct);
}

/**
 * @brief 【软件 I2C 核心函数】I2C 起始信号函数
 *      1. SCL 时钟线处于稳定的高电平状态
 *      2. SDA 数据线在 SCL 时钟线高电平状态下，完成了一次由高电平到低电平的跳变过程
 *      3. 并且 SCL 后续进入周期性高电平低电平变化，提供整个数据传递过程的参考周期
 */
void I2C_Start(void)
{
    // 1. 明确当前 I2C SDA 数据线为输出模式，有可能数从机应答失败，从新开始的 I2C_Start 信号
    // 此时读取从机应答信号过程是 SDA 输入模式。
    SDA_Output_Mode();

    // 2. 保证 SDA 和 SCL 处于【高电平状态】 ==> 空闲状态
    SDA_Set(1);
    SCL_Set(1);
    I2C_Delay();

    // 3. SDA 拉低，同时 SCL 保持高电平状态，满足起始信号【SDA 高到低跳变】
    SDA_Set(0);
    I2C_Delay();

    // 4. SCL 拉低，进入明确的 SCL 时钟周期过程，进入嘀嗒！！！
    SCL_Set(0);
    I2C_Delay();
}

/**
 * @brief 【软件 I2C 核心函数】I2C 停止信号函数
 *      1. SCL 时钟线处于稳定的高电平状态
 *      2. SDA 数据线在 SCL 时钟线高电平状态下，完成了一次由低电平到高电平的跳变过程
 *      3. 后续 SDA 和 SCL 保持高电平状态，进入==> I2C 信号
 */
void I2C_Stop(void)
{
    // 1. 明确当前 I2C SDA 数据线为输出模式，有可能数从机应答失败，从新开始的 I2C_Start 信号
    // 此时读取从机应答信号过程是 SDA 输入模式。
    SDA_Output_Mode();

    // 2. SDA 和 SCL 进入低电平模式，无效时钟周期状态。
    SCL_Set(0);
    SDA_Set(0);
    I2C_Delay();

    // 3. 时钟线拉高，进入高电平模式，有效周期
    SCL_Set(1);
    I2C_Delay();

    // 4. SDA 数据线拉高，在时钟线高电平周期内完成低电平到高电平跳变
    SDA_Set(1);
    I2C_Delay();

    /*
    如果没有后续功能！！！此时 SCL 和 SDA 同处于高电平状态 ==> 【I2C 空闲状态】
    */
}

/**
 * @brief SDA 数据线读取电平情况下，主要用于满足
 *      1. MCU 主机读取从机发送应答信息 ACK or NACK
 *      2. MCU 读取从机发送的数据信息
 *      读取 SDA 数据线电平情况下，高电平对应数据 1，低电平对应数据 0
 *
 * @return 返回值对应 SDA 数据线电平情况，高电平返回 1，低电平返回 0
 */
u8 SDA_Read(void)
{
    return GPIO_ReadInputDataBit(I2C_GPIO_GROUP, I2C_SDA_PIN);
}

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
u8 I2C_SendByte(u8 byte)
{
    // 1. 保证当前 SDA 工作模式为输出模式。
    SDA_Output_Mode();

    // 2. 必要变量准备
    u8 i = 0;
    u8 ack = 0;

    /*
    【数据发送】
    3. 利用循环，对当前需要发送的数据 byte 进行逐一字节高位先发处理
    */
    for (i = 0; i < 8; i++)
    {
        /*
        3.1 SCL 拉低电平，进入时钟无效周期状态，保证当前 SDA 可以执行相关的调整操作
        防止出现触发其他 I2C 信号
        */
        SCL_Set(0);
        I2C_Delay();

        /*
        3.2 进入到 SCL 低电平周期内容，根据当前数据内容，采用高位先发原则 MSB 方式
        对 SDA 数据线进行电平调整
        */
        if (byte & 0x80)
        {
            SDA_Set(1);
        }
        else
        {
            SDA_Set(0);
        }
        I2C_Delay();

        /*
        3.3 进入到 SCL 高电平周期内，开始进行利用 I2C 协议方式进行数据传递。
        */
        SCL_Set(1);
        I2C_Delay();

        /*
        3.4 byte 发送最高位之后，进行【左移操作】
        */
        byte <<= 1;
    }

    /*
    4. 上述数据发送完成之后，SCL 处于高电平状态，需要保证 I2C SCL 时钟周期的
    完整性，进入到主机读取从机应答信息之前，需要完成上一次 SCL 周期控制
    */
    SCL_Set(0);
    I2C_Delay();

    /*
    【从机应答处理】流程
    */
    /*
    【从机应答处理】
        1. 保证 SDA 数据线为输入模式。
    */
    SDA_Input_Mode();
    I2C_Delay();

    /*
    【从机应答处理】
        2. SCL 拉高，告知从机可以进行数据传递。I2C 协议规定，都是在时钟线
        高电平周期内进行数据传递。

        高电平信号是告知从机可以进行数据发送
        延时操作是给予从机发送数据的必要时间
    */
    SCL_Set(1);
    I2C_Delay();

    /*
    【从机应答处理】
        3. SCL 拉高之后，从机将对应的响应信号通过 SDA 传递到 MCU。需要
            在 SCL 高电平周期内，读取 SDA 电平情况，判断从机应答

        此时读取 SDA 数据线电平情况，通用处于 SCL 高电平周期内。
    */
    ack = SDA_Read();

    /*
    【从机应答处理】
        4. 保证时钟 SCL 周期完整性，需要进行拉低电平处理
    */
    SCL_Set(0);
    I2C_Delay();

    return ack;
}

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
u8 I2C_ReadByte(ACK_TYPE ack)
{
    // 1. SDA 对应 GPIO 为输入模式
    SDA_Input_Mode();

    // 2. 必要变量准备
    u8 i = 0;
    u8 data = 0;

    /*
    3. 利用 for 循环读取数据内容
    */
    for (i = 0; i < 8; i++)
    {
        /*
        3.1 SCL 时钟线拉低操作，进入正常 SCL 周期控制
        */
        SCL_Set(0);
        I2C_Delay();

        /*
        3.2 SCL 时钟线拉高，告知从机在 SCL 高电平进行数据传递
        */
        SCL_Set(1);
        I2C_Delay();

        /*
        3.3 data 存储数据变量左移 1 位，二进制操作会再低位补 0
            SDA_Read() 读取 SDA 数据线电平情况，此时电平由从机设备控制
            SDA_Read() 读取到高电平，返回 1，if 分支执行，data 位或 1
            SDA_Read() 读取到低电平，返回 0，if 无法执行
        */
        data <<= 1;
        if (SDA_Read())
        {
            data |= 1;
        }
    }

    /*
    以上操作读取数据完成之后，SCL 处于高电平状态，为了保证时钟周期完成。拉低电平
    */
    SCL_Set(0);
    I2C_Delay();

    /*
    【主机发送应答信息给予从机处理】
        1. 将 SDA 数据线输入模式转换为输出模式，用于发送应答信息
    */
    SDA_Output_Mode();

    /*
    【主机发送应答信息给予从机处理】
        2. 根据用户给定的参数，将 SDA 数据线电平情况进行调整。
            ACK(0) ==> SDA 低电平
            NACK(1) ==> SDA 高电平
    */
    SDA_Set(ack);
    I2C_Delay();

    /*
    【主机发送应答信息给予从机处理】
        3. 时钟线拉高，将当时 SDA 数据对应的应答信号明确告知从机
    */
    SCL_Set(1);
    I2C_Delay();

    /*
    【主机发送应答信息给予从机处理】
        4. 保证时钟周期的完整性，将 SCL 进行拉低处理
    */
    SCL_Set(0);

    return data;
}
