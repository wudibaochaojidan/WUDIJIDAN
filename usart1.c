#include "usart1.h"

void USART1_Init(u32 brr)
{
	/*
	1. 时钟使能 
		需要使能 USART1 和 GPIOA ，同属 APB2 分支
	*/
	RCC->APB2ENR |= APB2_IOPA_CLOCK_ENABLE | APB2_USART1_CLOCK_ENABLE;
	
	/*
	2. GPIO 配置
		PA9 USART1 TX ==> 【复用推挽输出模式，速度 2MHz】
		PA10 USART1 RX ==> 【浮空输入模式】
	*/
	GPIOA->CRH &= ~(0xFF << 4);
	
	GPIOA->CRH |= (AF_Push_Pull_Output_Speed_2MHz << 4);
	GPIOA->CRH |= (Floating_Input_Mode << 8);
	
	/*
	3. USART1 配置
		3.1 NRZ 8N1 数据格式
			1 位起始位，8 位数据位，0 位校验位，1 位停止位
		3.2 波特率计算
		3.3 TX RX 使能
		3.4 USART1 开启
	*/	
	// 3.1 8N1 配置
	// 【8】保证 USART1 CR1 寄存器 M 标志位必须是 0 
	// 明确 NRZ 数据组成 1 位起始位，8 位数据位，n 位校验位，n 位停止位
	USART1->CR1 &= ~(0x01 << 12);
	
	// 【N】保证 USART1 CR1 寄存器 PCE 标志位必须是 0 
	// 不使用 USART 数据校验位信息
	USART1->CR1 &= ~(0x01 << 10);
	
	// 【1】保证 USART1 CR2 寄存器 STOP[位13:12] 标志位必须是 00
	// 确定当时 NRZ 数据帧停止位 1 位
	USART1->CR2 &= ~(0x03 << 12);
	
	// 3.2 波特率计算
	float USART_DIV = USART1_SYSTEM_CLOCK / 16 / brr;
	u16 DIV_Mantissa = (u16)USART_DIV;
	u16 DIV_Fraction = (u16)((USART_DIV - DIV_Mantissa) * 16);
	
	// 波特率寄存器填充控制
	USART1->BRR = (DIV_Mantissa << 4) | DIV_Fraction;
	
	/*
	【USART1 中断注册补充】
		需要在 USART1 开启 TX 和 RX 之前完成中断配置。
	*/
	/*
	【USART1 中断处理 1】
		USART1 CR1 寄存器开启 
			RXNEIE 接收缓冲区非空中断使能
			IDLEIE 数据总线空闲中断使能
	*/
	USART1->CR1 |= (0x01 << 5); // 使能 RXNE 中断
	USART1->CR1 |= (0x01 << 4); // 使能 IDLE 中断
	
	/*	
	【USART1 中断处理 2】
		NVIC 注册当前 USART1 对应的 IRQn 中断请求编号
		并且设置当前中断的优先级。
	*/
	// NVIC_EnableIRQ(IRQn_Type IRQn) NVIC 注册指定中断请求编号
	NVIC_EnableIRQ(USART1_IRQn);
	// TODO NVIC_SetPriorityIRQn_Type IRQn， uint32_t priority); 设置中断优先级
	NVIC_SetPriority(USART1_IRQn, 1);
	
	// 3.3 TX RX 使能
	USART1->CR1 |= (0x01 << 3); // 使能 USART1 TX
	USART1->CR1 |= (0x01 << 2); // 使能 USART1 RX
	
	// 3.4 USART1 开启
	USART1->CR1 |= (0x01 << 13); // 使能 USART1 工作
}

/*
【USART1 中断处理 3】
	实现 USART1 中断请求处理函数
*/
USART1_Data usart1_value = {0};

void USART1_IRQHandler(void)
{
	u8 temp;
	/*
	【注意】
		无论是触发 RXNE 中断还是 IDLE 中断，都是当前 USART1_IRQHandler 函数处理
		需要根据 USART1 SR 状态寄存器中的 RXNE 标志位 or IDLE 标志位判断当前触发
		中断是哪一个，进行针对性处理。
	
	【需要完成的内容】
		1. 当前数据已处理之后，需要清除缓冲区中的数据内容
		2. IDLE 中断处理
		3. RXNE 中断处理
	*/
	// 1. 当前数据已处理之后，需要清除缓冲区中的数据内容
	if (usart1_value.flag)
	{
		/*
		当前 flag 为 1 表示数据已满 or 数据接收完成，并且
		数据已经处理。
		*/
		memset(&usart1_value, 0, sizeof(USART1_Data));
	}
	
	// 2. IDLE 中断处理
	if (USART1->SR & (0x01 << 4))
	{
		/*
		IDLE 中断触发，表示数据接收完成，需要进行的处理有
			1. 清除 IDLE 中断标志位
			2. 数据接收完成标志位 flag 赋值为 1
			3. 【数据处理】
				目前临时将数据发送到 PC 串口调试工具
		*/
		// 1. 清除 IDLE 中断标志位 【软件序列】读 USART_SR 读 USART_DR
		temp = USART1->SR;
		temp = USART1->DR;
		
		// 2. 数据接收完成标志位 flag 赋值为 1
		usart1_value.flag = 1;
		
		// 3. 【数据处理】
		// 		目前临时将数据发送到 PC 串口调试工具
		USART1_SendString("IDLE : ");
		USART1_SendBuffer(usart1_value.buffer, usart1_value.count);
		USART1_SendString("\r\n");
	}
	
	// 3. RXNE 中断处理
	if (USART1->SR & (0x01 << 5))
	{
		/*
		RNXE 中断触发，表示有数据进入到当前 USART1 DR 寄存器，需要进行存储操作
		数据存储到 USART1_Data 结构体缓冲区数组中。
			1. 数据存储到 USART1_Data 结构体
			2. 判断当前底层接收数据缓冲是否已满，如果已满进行【临时处理】展示数据，同时
				将 flag 标志位赋值为 1
		*/
		// 1. 数据存储到 USART1_Data 结构体
		usart1_value.buffer[usart1_value.count++] = USART1->DR;
		
		// 2. 判断当前底层接收数据缓冲是否已满，
		if (USART1_DATA_BUFFER_SIZE == usart1_value.count)
		{
			// 数据缓冲区已满，下一次触发中断需要将原数据内容清空
			usart1_value.flag = 1;
			// 【临时数据处理】目前临时将数据发送到 PC 串口调试工具
			USART1_SendString("RXNE : ");
			USART1_SendBuffer(usart1_value.buffer, usart1_value.count);
			USART1_SendString("\r\n");
		}
	}	
}

void USART1_SendByte(u8 byte) 
{
	/*
	再每一次数据发送之前，需要确定上一次数据发送是否完成
	当前 while 循环判读中
		1. 【读取 USART1->SR】 寄存器数据内容，和 0x01 << 6 判断
		2. 如果当前数据尚未发送完成， TC 标志位为 0，while 循环继续
		3. 如果数据发送完成，TC 标志位为 1，while 循环终止。
	*/
	while (!((0x01 << 6) & USART1->SR)) {}

	/*
	【核心】
	【写入数据到 USART1->DR】 寄存器，进行数据发送操作
	*/
	USART1->DR = byte;
}

void USART1_SendBuffer(const u8 *buffer, u16 len) 
{
	while (len--)
	{
		USART1_SendByte(*buffer);
		buffer += 1;
	}
}

void USART1_SendString(const char *str) 
{
	while (*str)
	{
		USART1_SendByte(*str);
		str += 1;
	}
}




