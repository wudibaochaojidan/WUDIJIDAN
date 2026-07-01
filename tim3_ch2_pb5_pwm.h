#ifndef _TIM3_CH2_PB5_PWM_H
#define _TIM3_CH2_PB5_PWM_H

#include "common.h"

/**
 * @brief 配置 TIM3 通道 CH2 对应 PB5 引脚输出 PWM
 *      时钟使能 TIM3 GPIOB AFIO
 *      GPIOB PB5 配置为复用推挽输出模式，速度 2 MHz
 *      AFIO 配置 TIM3_CH2 引脚重映射关系
 *      TIM3 CH2 配置 PWM 输出
 * 
 * @param psc 定时预分频倍数
 * @param arr 自动重装载寄存器
 */
void TIM3_CH2_PB5_PWM(u16 psc, u16 arr);


/**
 * @brief 设置 TIM3 通道 CH2 对 CCR2 寄存器数据，用于控制 PWM 占空比
 * 
 * @param ccr 赋值给 TIM3->CCR2 寄存器的数据，与 CNT 进行比较控制占空比
 */
void TIM3_CH2_PB5_CCR2(u16 ccr);
#endif
