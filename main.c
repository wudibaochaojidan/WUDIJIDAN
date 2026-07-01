#include "stm32f10x.h"

#include "led.h"
#include "delay.h"
#include "key.h"
#include "usart1.h"
#include "tim6.h"
#include "tim2.h"
#include "tim3_ch2_pb5_pwm.h"
#include "usart1.h"
#include "usart3.h"
#include "esp8266.h"
#include "mqtt.h"
#include "soft_i2c.h"
#include "eeprom_at24c02.h"

#define EEPROM_AT24C02_ADDR (0x50) // 101 0000

int main(void)
{
	LED_Init();
	// 初始化 USART1 模块，建立 PC 和 MCU 通道，波特率要求 9600
	USART1_Init(9600);
	// 初始化 I2C GPIO 引脚
	I2C_GPIO_Init();

	// 写入数据到 EEPROM 中
	if (EEPROM_WriteByte(EEPROM_AT24C02_ADDR, 0x10, 'A'))
	{
		USART1_SendString("Write Data To EEPROM AT24C02 Failed!\r\n");
	}
	else
	{
		USART1_SendString("Write Data To EEPROM AT24C02 Successed!\r\n");
	}

	Delay_ms(2000);

	// 读取 AT24C02 设备数据
	u8 data = 0;

	if (!EEPROM_ReadByte(EEPROM_AT24C02_ADDR, 0x10, &data))
	{
		USART1_SendString("Read Data From  EEPROM AT24C02 Successed\r\n");
		USART1_SendString("Data : ");
		USART1_SendByte(data);
		USART1_SendString("\r\n");
	}
	else 
	{
		USART1_SendString("Read Data From EEPROM AT24C02 Failed\r\n");
	}

	while (1)
	{
		LED0_Toggle;
		LED1_Toggle;

		Delay_ms(5000);
	}
}
