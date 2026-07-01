#include "usart3.h"

USART3_Data usart3_value = {0};

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
void USART3_Init(u32 brr)
{
    /*
    标准库实现模块配置流程
        1. 对应模块结构体变量定义
        2. 结构体变量参数赋值【配置参数打包】
        3. Init 函数初始化当前模块内容
    */
    // 1. RCC 时钟使能对应模块，需要使能模块有 GPIOB USART3
    // 标准库方式使能目标模块
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);

    /*
    2. GPIO 配置

    typedef struct
    {
        uint16_t GPIO_Pin;             // 引脚编号
        GPIOSpeed_TypeDef GPIO_Speed;  // 引脚速度，有且只针对于【输出模式】
        GPIOMode_TypeDef GPIO_Mode;    // GPIO 工作模式
    }  GPIO_InitTypeDef;
    */
    GPIO_InitTypeDef GPIO_InitStructure = {0};

    // PB11 USART3_RX 引脚初始化操作，对应工作模式为【浮空输入模式】
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;            // 引脚编号为 11 号
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; // 浮空输入模式
    /*
    void GPIO_Init(GPIO_TypeDef *GPIOx, GPIO_InitTypeDef *GPIO_InitStruct)
        GPIO_TypeDef *GPIOx 指定 GPIO 模块，对应 GPIOA GPIOB ...
        GPIO_InitTypeDef *GPIO_InitStruct 对应 GPIO 模块初始化操作参数结构体。
    */
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    // PB10 USART3_TX 引脚初始化操作，对应工作模式为【复用推挽输出模式，速度 2 MHz】
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;       // 引脚编号为 10 号
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;  // 复用推挽输出模式
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz; // 输出速率 2 MHz
    /*
    void GPIO_Init(GPIO_TypeDef *GPIOx, GPIO_InitTypeDef *GPIO_InitStruct)
        GPIO_TypeDef *GPIOx 指定 GPIO 模块，对应 GPIOA GPIOB ...
        GPIO_InitTypeDef *GPIO_InitStruct 对应 GPIO 模块初始化操作参数结构体。
    */
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    /*
    3. USART 模块配置
        3.1 8N1 模式 1 位起始位 8 位数据位 0 位校验位 1 位停止位
        3.2 波特率配置
        3.3 USART 发送使能和接收使能
    */

    /*
    typedef struct
    {
        // 当前 USART 工作对应波特率
        uint32_t USART_BaudRate;
        // 设置 USART 数据帧长度，对应 8 位 or 9 位
        uint16_t USART_WordLength;
        // 设置 USART 停止位，0.5, 1, 1.5, 2
        uint16_t USART_StopBits;
        // 设置校验位 0 位 or 1 位
        uint16_t USART_Parity;
        // 当前 USART 开放 Tx or Rx 模块
        uint16_t USART_Mode;
        // 触发方式
        uint16_t USART_HardwareFlowControl;
    } USART_InitTypeDef;
    */
    USART_InitTypeDef USART_InitStructure = {0};

    // 8N1 配置
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    // 波特率赋值
    USART_InitStructure.USART_BaudRate = brr;
    // USART 设置 TX 和 RX 同时工作
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;

    USART_Init(USART3, &USART_InitStructure);

    // 3.4 USART 中断模块使能和配置 Interrupt Config
    /*
    void USART_ITConfig(USART_TypeDef *USARTx, uint16_t USART_IT,
        FunctionalState NewState)
    USART 模块中断配置使能 or 失能配置
        USART_TypeDef *USARTx 对应 USART 模块，例如 USART1 USART2
        uint16_t USART_IT USART 对应中断名称宏定义
        FunctionalState NewState ENABLE 表示使能，DISENABLE 失能
    【重点】
        其他模块函数与之类似，只需要更换不同的模块名称即可，同时需要根据
        手册分析对应的中断名称需求
    */
    USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);
    USART_ITConfig(USART3, USART_IT_IDLE, ENABLE);

    NVIC_InitTypeDef NVIC_InitStructure = {0};

    NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;

    NVIC_Init(&NVIC_InitStructure);

    // 4. USART_Cmd 开启当前 USART3 模块
    USART_Cmd(USART3, ENABLE);
}

/*
和 USART1 一致，都是需要处理
    1. IDLE 数据总线空闲中断
    2. RXNE 数据接收缓冲非空中断

【重点注意小心谨慎】不允许使用标准库中断清除方式，
【有且只允许采用寄存器方式清除中断】
*/
void USART3_IRQHandler(void)
{
    /*
    缓冲区标志位 flag 为 1 需要清除当前缓冲区数据内容。
    */
    if (usart3_value.flag)
    {
        memset(&usart3_value, 0, sizeof(USART1_Data));
    }
    u8 temp;
    /*
    2. IDLE 中断处理
        ITStatus USART_GetITStatus(USART_TypeDef *USARTx, uint16_t USART_IT)
        判断当前指定 USART 模块中断是否触发            USART_TypeDef *USARTx USART 模块
            uint16_t USART_IT USART 中断标志位
    */
    if (USART_GetITStatus(USART3, USART_IT_IDLE))
    {
        // 2.1 清除中断标志位【软件序列】(先读USART_SR，然后读USART_DR)。
        temp = USART3->SR;
        temp = USART3->DR;

        // 2.2 缓冲区底层数组 flag 标志位赋值为 1
        usart3_value.flag = 1;

        /*
        2.3 【数据临时处理】将 USART3 接收到的数据内容，通过 USART1 模块发送到
            PC 串口调试助手
        */
        // USART1_SendString("USART3 : ");
        USART1_SendBuffer(usart3_value.buffer, usart3_value.count);
        USART1_SendString("\r\n");
    }

    /*
    3. RXNE 中断处理
    */
    if (USART_GetITStatus(USART3, USART_IT_RXNE))
    {
        // 3.1 清除中断内容，【读取 USART->DR】
        /*
        uint16_t USART_ReceiveData(USART_TypeDef *USARTx)
            读取指定 USART 模块数据内容
        */
        usart3_value.buffer[usart3_value.count++] = USART_ReceiveData(USART3);

        // 2. 判断当前底层接收数据缓冲是否已满，
        if (USART3_DATA_BUFFER_SIZE == usart3_value.count)
        {
            // 数据缓冲区已满，下一次触发中断需要将原数据内容清空
            usart3_value.flag = 1;

            // 【临时数据处理】目前临时将数据发送到 PC 串口调试工具
            // USART1_SendString("USART3 : ");
            USART1_SendBuffer(usart3_value.buffer, usart3_value.count);
            USART1_SendString("\r\n");
        }
    }
}

/**
 * @brief 通过 USART3 模块发送一个字节数据
 *
 * @param byte 发送的字节数据内容
 */
void USART3_SendByte(u8 byte)
{
    /*
    FlagStatus USART_GetFlagStatus(USART_TypeDef *USARTx, uint16_t USART_FLAG);
        获取指定串口模块对应标志位状态，返回结果为 0 or 1
        typedef enum {RESET = 0, SET = !RESET} FlagStatus, ITStatus;
    USART_TypeDef *USARTx 串口模块名称，
    uint16_t USART_FLAG 对应标志位名称
    */
    while (!USART_GetFlagStatus(USART3, USART_FLAG_TC))
        ;

    /*
    void USART_SendData(USART_TypeDef *USARTx, uint16_t Data)
        串口数据发送函数，需要指定 USART 模块和对应数据
        USART_TypeDef *USARTx 串口模块，例如 USART1 USART2 USART3
        uint16_t Data 1 字节数据内容
    */
    USART_SendData(USART3, byte);
}

/**
 * @brief 通过 USART3 模块发送字节数据缓冲区
 *
 * @param buffer 发送字节缓冲区内存空间首地址
 * @param len    数据缓冲区字节个数
 */
void USART3_SendBuffer(u8 *buffer, u16 len)
{
    while (len--)
    {
        USART3_SendByte(*buffer);
        buffer++;
    }
}

/**
 * @brief 通过 USART3 模块发送字符串数据内容
 *
 * @param str 需要发送的字符串数据
 */
void USART3_SendString(const char *str)
{
    while (*str)
    {
        USART3_SendByte(*str);
        str++;
    }
}
