#ifndef _MQTT_H
#define _MQTT_H

#include "common.h"
#include "usart3.h"

#define MQTT_PACKAGE_SIZE (128)

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
                               u16 keep_alive);

/**
 * @brief STM32 通过 ESP8266 发布数据到 ThingsCloud 云平台，数据包格式按照
 *          PUBLISH 报文要求完成。要求用户提供对应上报信息主题 topic_name 和
 *          有效载荷字符串
 * 
 * @param topic_name 上报信息主题字符串，当前 ThingsCloud 要求为 "attributes"
 * @param payload    JSON 格式字符串有效载荷内容
 */
void MQTT_Publish_Package_Send(const char *topic_name, const char *payload);

/**
 * @brief STM32 通过 ESP8266 发布订阅主题数据包到 ThingsCloud 云平台，用于订阅
 *          MQTT 服务器云平台下发属性主题，可以接收到 ThingsCloud 下发指令
 * 
 * @param topic_name 订阅主题对应字符串
 * @param qos        当前订阅主题对应的 QoS 等级，有 0 1 2。需要根据服务器要求提供
 */
void MQTT_Subscribe_Package_Send(const char *topic_name, u8 qos);

#endif
