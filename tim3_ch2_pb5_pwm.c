#include "tim3_ch2_pb5_pwm.h"

void TIM3_CH2_PB5_PWM(u16 psc, u16 arr)
{
    // 1. RCC 时钟使能
    RCC->APB2ENR |= APB2_IOPB_CLOCK_ENABLE | APB2_AFIO_CLOCK_ENABLE;
    RCC->APB1ENR |= APB1_TIM3_CLOCK_ENABLE;

    /*
    2. GPIOB 对应 PB5 引脚工作模式，对应
    GPIOB CRL 低位控制寄存器
    */
    GPIOB->CRL &= ~(0x0F << 20);
    GPIOB->CRL |= (AF_Push_Pull_Output_Speed_2MHz << 20);

    /*
    3. AFIO 配置 TIM3 输出硬件重映像，选择部分重映像方式
    */
    AFIO->MAPR &= ~(0x03 << 10);
    AFIO->MAPR |= (0x02 << 10);

    /*
    4. TIM3 定时器配置
        4.1 基础 PSC + ARR 控制定时器工作周期
    */
    TIM3->PSC = psc - 1;
    TIM3->ARR = arr - 1;

    /*
    4.2 CCMR1 配置当前 CH2 对应的工作模式内容
        OC2CE 外部触发关闭          ==> 0
        OC2M  所需工作模式以为 PWM1 ==> 110
        OC2PE 预装载关闭状态        ==> 0
        OC2FE 开启PWM 快速模式      ==> 1
        CC2S  选择开启 OC(Output Compare 模式) ==> 00
    
    寄存器二进制位内容 0110 0100 ==> 0x64
     */
    TIM3->CCMR1 &= ~(0xFF << 8); // 清除针对于 CH2 相关配置
    TIM3->CCMR1 &= ~(0x01 << 15); // OC2CE 为 0
    TIM3->CCMR1 |= (0x06 << 12);  // OC2M 为 110
    TIM3->CCMR1 &= ~(0x01 << 11); // OC2PE 为 0
    TIM3->CCMR1 |= (0x01 << 10);  // OC2FE 为 1
    TIM3->CCMR1 &= ~(0x03 << 8);  // CC2S 开启 CH2 对应 OC(Output Compare 模式)
    
    // 4.3 对应当前 CH2 的 CCR2 默认为 0 
    TIM3->CCR2 = 0;

    // 4.4 CCER 开启当前 TIM3_CH2 输出使能，同时设置有效电平为【低电平】
    TIM3->CCER |= (0x01 << 5); // CC2P 设置输出模式下有效电平为【低电平】
    TIM3->CCER |= (0x01 << 4); // CC2E 开启当前 OC2 输出模式

    // 5. 启动定时器，设置为周期性定时器
    TIM3->CR1 &= ~(0x01 << 3); // OPM 设置为 0，周期性定时器    
    TIM3->CR1 &= ~(0x01 << 4); // DIR 设置计数方向为向上计数
    TIM3->CR1 |= 0x01;
}

void TIM3_CH2_PB5_CCR2(u16 ccr)
{
    /*
    根据当前定时器配置分析
        1. 选择 CNT 向上计数模式
        2. 选择 PWM1 工作模式，对应 CNT < CCR 输出有效电平，否则无需电平
        3. 设置当前有效电平为【低电平】

    CCR 数据越大，对应 PWM 有效电平占空比越高。
    */
    TIM3->CCR2 = ccr;
}
