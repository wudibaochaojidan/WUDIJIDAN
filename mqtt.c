#include "mqtt.h"

/**
 * @brief STM32 通过 ESP8266 发送 MQTT 协议对应的 CONNECT 报文数据包
 *      建立本地设备和云端设备连接。所需提供的参数有本机客户端名称，连接使用
 *      的用户名和密码
 *
 * @param client_name 本机客户端名称
 * @param username    连接目标 MQTT 云平台设备对应的用户名
 * @param password    连接目标 MQTT 云平台对应的密码
 * @param keep_alive  MQTT 客户端和 MQTT 服务器最大消息间隔
 */
void MQTT_Connect_Package_Send(const char *client_name,
                               const char *username,
                               const char *password,
                               u16 keep_alive)
{
    /*
    MQTT CONNECT 数据包组成
        【1. 固定头 Fixed Header】
        【2. 可变头 Variable Header】
        【3. 有效载荷 Payload】
    */

    // 1. 数据包采用 u8 类型数组组装。
    u8 Connect_Package_Data[MQTT_PACKAGE_SIZE] = "";

    // 2. 计算相关的数据长度和必要变量
    u16 client_len = strlen(client_name);
    u16 username_len = strlen(username);
    u16 password_len = strlen(password);
    u16 mqtt_len = strlen("MQTT");

    // 3. 相关协议头字节数
    /*
    3.1 CONNECT 数据包可变头字节数
        2 + N MQTT 字符串数据，对应协议字符串 "MQTT"
        1 对应协议级别
        1 对应连接标志位参数
        2 对应 Keep Alive 时间
    */
    u16 variable_header_len = 2 + mqtt_len + 1 + 1 + 2;
    /*
    3.2 CONNECT 数据包有效载荷字节数
        2 + N MQTT 字符串数据，对应协议客户端名称字符串
        2 + N MQTT 字符串数据，对应协议用户名字符串
        2 + N MQTT 字符串数据，对应协议密码字符串
    */
    u16 payload_len = 2 + client_len + 2 + username_len + 2 + password_len;

    u8 pos = 0;
    /*
    【核心步骤一】 MQTT CONNECT 固定头 Fixed Header 组装
        1.1 报文类型 + 报文参数标志位 ==> 0x10
        1.2 数据包剩余长度 ==> variable_header_len + payload_len;
    */
    Connect_Package_Data[pos++] = 0x10;
    Connect_Package_Data[pos++] = variable_header_len + payload_len;

    /*
    【核心步骤二】 MQTT CONNECT 可变头 Variable Header 组装
        2.1 2 + N MQTT 字符串数据，对应 "MQTT" 协议名称字符串【6 字节】
        2.2 协议等级 【1 字节】
        2.3 连接标志位参数 【1 字节】
        2.4 Keep Alive 时间 【2 字节】
    */
    /*
    2 + N MQTT 字符串格式组包赋值方式
    */
    Connect_Package_Data[pos++] = mqtt_len / 256; // mqtt_len >> 8;
    Connect_Package_Data[pos++] = mqtt_len % 256; // mqtt_len & (0x00FF);
    memcpy(Connect_Package_Data + pos, "MQTT", mqtt_len);
    pos += mqtt_len;

    Connect_Package_Data[pos++] = 0x04; // MQTT 3.1.1 版本
    Connect_Package_Data[pos++] = 0xC2; // 连接标志位参数，对应 1100 0010

    // Keep Alive 时间
    Connect_Package_Data[pos++] = keep_alive / 256; // keep_alive >> 8;
    Connect_Package_Data[pos++] = keep_alive % 256; // keep_alive & (0x00FF);

    /*
   【核心步骤三】 MQTT CONNECT 有效载荷 Payload 组装
       3.1 2 + N MQTT 字符串格式 ==> 客户端名称
       3.2 2 + N MQTT 字符串格式 ==> 用户名
       3.3 2 + N MQTT 字符串格式 ==> 密码
    */
    // 3.1 客户端名称
    Connect_Package_Data[pos++] = client_len / 256; // client_len >> 8
    Connect_Package_Data[pos++] = client_len % 256; // client_len & (0x00FF);
    memcpy(Connect_Package_Data + pos, client_name, client_len);
    pos += client_len;

    // 3.2 用户名
    Connect_Package_Data[pos++] = username_len / 256; // client_len >> 8
    Connect_Package_Data[pos++] = username_len % 256; // client_len & (0x00FF);
    memcpy(Connect_Package_Data + pos, username, username_len);
    pos += username_len;

    // 3.3 密码
    Connect_Package_Data[pos++] = password_len / 256; // client_len >> 8
    Connect_Package_Data[pos++] = password_len % 256; // client_len & (0x00FF);
    memcpy(Connect_Package_Data + pos, password, password_len);
    pos += password_len;

    /*
    MCU 将数据包提交给 ESP8266 发送到云平台，连接 MQTT 云平台控制端
    */
    USART3_SendBuffer(Connect_Package_Data, pos);
}

/**
 * @brief STM32 通过 ESP8266 发布数据到 ThingsCloud 云平台，数据包格式按照
 *          PUBLISH 报文要求完成。要求用户提供对应上报信息主题 topic_name 和
 *          有效载荷字符串
 *
 * @param topic_name 上报信息主题字符串，当前 ThingsCloud 要求为 "attributes"
 * @param payload    JSON 格式字符串有效载荷内容
 */
void MQTT_Publish_Package_Send(const char *topic_name, const char *payload)
{
    /*
    MQTT PUBLISH 数据包组成
        【1. 固定头 Fixed Header】
        【2. 可变头 Variable Header】
        【3. 有效载荷 Payload】
    */
    // 1. 数据包采用 u8 类型数组组装
    u8 Publish_Package_Data[MQTT_PACKAGE_SIZE] = "";

    // 2. 计算相关数据长度
    u16 topic_len = strlen(topic_name);
    u16 payload_len = strlen(payload);

    // 3. 可变头字节数
    u16 variable_header_len = 2 + topic_len;

    u8 pos = 0;
    /*
    【核心步骤一】 MQTT PUBLISH 固定头 Fixed Header 组装
        1.1 报文类型 + 报文参数标志位 ==> 0x30
        1.2 数据包剩余长度 ==> variable_header_len + payload_len;
    */
    Publish_Package_Data[pos++] = 0x30;
    Publish_Package_Data[pos++] = variable_header_len + payload_len;

    /*
    【核心步骤二】 MQTT PUBLISH 可变头 Variable Header 组装
        2 + N MQTT 字符串数据，对应上报信息属性字符串 topic_name
    */
    Publish_Package_Data[pos++] = topic_len >> 8;
    Publish_Package_Data[pos++] = topic_len & 0xFF;
    memcpy(Publish_Package_Data + pos, topic_name, topic_len);
    pos += topic_len;

    /*
   【核心步骤三】 MQTT PUBLISH 有效载荷 Payload 组装
      有效载荷字符串直接填充
    */
    memcpy(Publish_Package_Data + pos, payload, payload_len);
    pos += payload_len;

    /*
    MCU 将数据包提交给 ESP8266 发送到云平台，上报数据内容
    */
    USART3_SendBuffer(Publish_Package_Data, pos);
}

/**
 * @brief STM32 通过 ESP8266 发布订阅主题数据包到 ThingsCloud 云平台，用于订阅
 *          MQTT 服务器云平台下发属性主题，可以接收到 ThingsCloud 下发指令
 *
 * @param topic_name 订阅主题对应字符串
 * @param qos        当前订阅主题对应的 QoS 等级，有 0 1 2。需要根据服务器要求提供
 */
void MQTT_Subscribe_Package_Send(const char *topic_name, u8 qos)
{
    /*
    MQTT SUBSCRIBE 数据包组成
        【1. 固定头 Fixed Header】
        【2. 可变头 Variable Header】
        【3. 有效载荷 Payload】
    */
    // 1. 数据包采用 u8 类型数组组装
    u8 Subscribe_Package_Data[MQTT_PACKAGE_SIZE] = "";

    // 2. 计算相关数据长度
    u16 topic_len = strlen(topic_name);

    u8 pos = 0;
    /*
    【核心步骤一】 MQTT SUBSCRIBE 固定头 Fixed Header 组装
        1.1 报文类型 + 报文参数标志位 ==> 0x82
        1.2 数据包剩余长度 ==> variable_header_len + payload_len;
    */
    Subscribe_Package_Data[pos++] = 0x82;
    /*
    2 ==> Variable Header Len
    2 + topic_len ==> 2 + N MQTT 字符串对应订阅主题字符串
    1 ==> QoS 服务等级参数
    */
    Subscribe_Package_Data[pos++] = 2 + 2 + topic_len + 1;

    /*
    【核心步骤二】 MQTT SUBSCRIBE 可变头 Variable Header 组装
        2 字节报文 ID ，当前情况下是无效数据，随意填充
    */
    Subscribe_Package_Data[pos++] = 0;
    Subscribe_Package_Data[pos++] = 3;

    /*
    【核心步骤三】 MQTT CONNECT 有效载荷 Payload 组装
       1. 2 + N MQTT 字符串格式数据，对应订阅主题
       2. 当前主题对应的 QoS 服务等级
    */
    Subscribe_Package_Data[pos++] = topic_len >> 8;
    Subscribe_Package_Data[pos++] = topic_len & 0xFF;
    memcpy(Subscribe_Package_Data + pos, topic_name, topic_len);
    pos += topic_len;

    Subscribe_Package_Data[pos++] = qos;

    /*
    MCU 将数据包提交给 ESP8266 发送到云平台，订阅主题
    */
    USART3_SendBuffer(Subscribe_Package_Data, pos);
}
