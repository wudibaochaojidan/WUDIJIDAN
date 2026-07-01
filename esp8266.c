#include "esp8266.h"

/**
 * @brief 通过 USART3 发送 AT 指令字符串到 ESP8266 模块
 * 
 * @param at_cmd  当前通过 USART3 模块发送到 ESP8266 的 AT 指令字符串，要求满足
 *                  AT 指令字符串格式要求。
 * @param ack     用于判断当前发送到 ESP8266 WiFi 模块对应 AT 指令是否执行成功，
 *                  当前 ack 对应应答字符串期望，例如 "OK", 特定回显内容字符 "+CWLAP"
 * @param timeout 当前 AT 等待时间
 * @return ESP8266 执行 AT 指令响应情况下成功，返回 0，否则返回 1。
 */
u8 USART3_Send_AT_Cmd_To_ESP8266(const char *at_cmd, const char *ack, u16 timeout)
{
    /*
    1. 通过 USART1 在 PC 串口调试工具中，查看当前发送的 AT 指令字符串信息
    */
    USART1_SendString("--------------------------------------\r\n");
    USART1_SendString("AT : ");
    USART1_SendString(at_cmd); // 数据从 MCU ---> PC 串口调试助手

    /*
    2. 利用 USART3 将 AT 指令发送到 ESP8266 模块
    */
    USART3_SendString(at_cmd); // 数据从 MCU ---> ESP8266 WiFi 模块

    while (timeout--)
    {
        /*
        如果当前 USART3_Data 数据缓冲结构体变量中的 flag 为 1 
        表示收到了 ESP8266 回显信息。
        */
        if (usart3_value.flag)
        {   
            /*
            char *strstr(const char *haystack, const char *needle)
            在字符串 *haystack* 中查找第一次出现字符串 *needle*（不包含空结束字符）的位置。
            */
            if (strstr((const char *)usart3_value.buffer, ack))
            {
                // 包括对应 ACK 返回 0
                return 0;
            }
            else
            {
                // 否则返回 1
                return 1;
            }
        }

        Delay_ms(1);
    }

    // 超时返回 1
    return 1;
}

/**
 * @brief ESP8266 WiFi 模块连接 AP 热点，要求
 *      1. 对应热点必须是 2.4GHz 频段，同时主要 AP 热点可连接数量参数
 *      2. 要求提供对应设备的 SSID 和 PSK，SSID 和 PSK 不可以有中文。
 * 
 * @param ssid 目标连接热点 AP 信息
 * @param psk  对应 AP 热点的密码
 */
void ESP8266_Connect_AP(const char *ssid, const char *psk)
{
    /*
    USART1 是 MCU 和 PC 串口调试助手通信模块
    USART3 是 MCU 和 ESP8266 WiFi 模块通信方式
    */
    USART1_SendString("ESP8266 WiFi Connecting......\r\n");

    // 1. 发送 AT 指令到 ESP8266，判断设备是否正常连接，是否正常工作。
    USART3_Send_AT_Cmd_To_ESP8266("AT\r\n", "OK", 2000);
    Delay_ms(2000);

    // 2. 发送 AT+CWMODE or AT+CWMODE_CUR or AT+CWMODE_DEF 设置 WiFi 工作模式
    // 当前所需工作模式为 STA 模式。
    USART3_Send_AT_Cmd_To_ESP8266("AT+CWMODE_CUR=1\r\n", "OK", 2000);
    Delay_ms(2000);

    // 3. 发送 AT+CWLAP 指令扫描当前可用 AP 热点
    USART3_Send_AT_Cmd_To_ESP8266("AT+CWLAP\r\n", "+CWLAP:", 2000);
    Delay_ms(3000);

    // 4. 发送 AT+CWJAP 指令连接目标 AP 热点
    char at_cmd[64] = "";
    sprintf(at_cmd, "AT+CWJAP=\"%s\",\"%s\"\r\n", ssid, psk);
    USART3_Send_AT_Cmd_To_ESP8266(at_cmd, "OK", 2000);
    Delay_ms(5000);

    // 5. 获取当前 ESP8266 WiFi 模块 IP 地址信息
    USART3_Send_AT_Cmd_To_ESP8266("AT+CIFSR\r\n", "OK", 2000);
    Delay_ms(3000);
}

/**
 * @brief 当前 ESP8266 通过 TCP 协议，根据对应的 IP 地址和端口号连接目标 MQTT 云平台服务器
 *    
 * @param protocol 目标网络连接协议，要求是字符串形式 "TCP" or "UDP"
 * @param ip_addr  目标服务器 IP 地址字符串 or 域名字符串
 * @param port     目标 MQTT 服务器提供服务器的端口
 */
u8 ESP8266_Connect_MQTT_Server(const char *protocol, const char *ip_addr, short port)
{
    char at_cmd[64] = "";

    /*
    AT 指令通过 TCP 协议连接目标服务器
    AT+CIPSTART="protocol_name","server_ip",server_port
    */
    sprintf(at_cmd, "AT+CIPSTART=\"%s\",\"%s\",%d\r\n", protocol, ip_addr, port);

    return USART3_Send_AT_Cmd_To_ESP8266(at_cmd, "OK", 2000);
}

/**
 * @brief 当前 ESP8266 模块开启透明传输模式，方便后续 ESP8266 直接将 MQTT 数据包内容
 *      不进行任何处理，直接发送到 MQTT 服务器。
 */
void ESP8266_Open_Transparent_Transmit_Mode(void) 
{
    /*
    开启透明传输模式
    */
    USART3_Send_AT_Cmd_To_ESP8266("AT+CIPMODE=1\r\n", "OK", 2000);
    Delay_ms(1000);

    /*
    开始利用透明传输模式发送数据，后续数据包内容不会增加任何的 ESP8266 处理。
    原文直接发送到目标服务器。
    */
    USART3_Send_AT_Cmd_To_ESP8266("AT+CIPSEND\r\n", ">", 2000);
}

/**
 * @brief 退出透传模式
 */
void ESP8266_Close_Transparent_Transmit_Mode(void) 
{
    /*
    ESP8266 模块退出透传模式 需要的 AT 指令是 "+++" 并且没有 "\r\n" 结尾
    */
    USART3_Send_AT_Cmd_To_ESP8266("+++", "", 2000);
}